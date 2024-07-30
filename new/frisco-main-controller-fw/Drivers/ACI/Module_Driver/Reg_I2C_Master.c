/*
 * Reg_I2C_Master.c
 *
 *  Created on: Jan 30, 2018
 *      Author: mikea
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "main.h"
#include "stm32f7xx_hal.h"
#include "RingBuf8.h"
#include "MsgQ.h"
#include "Module_Bus.h"
#include "Module_Driver.h"
#include "Testpoints.h"
#include "I2C_Bus_Master.h"
#include "Debug_Config.h"
#include "IMsgQueue.h"
#include "Debug_Flags.h"
#include "I2C_Bus_Watchdog.h"
#include "Debug_Log_Msg.h"


#if defined(LOW_LEVEL_I2C_DEBUG) || defined(PASSIVE_LOW_LEVEL_I2C_DEBUG)
#define PS(c)	RB8_PushOverwrite(&State_buf, c)
#else
#define PS(c)
#endif

static volatile uint8_t Dump_I2C_State_Buf = 0;

extern MODULE_DRIVER Module_Driver;
extern MODULE_BUS Module_Bus;

RingBuf8 State_buf;

#define MAX_NUM_RX_MESSAGES		24
#define MAX_NUM_TX_MESSAGES		32

#define I2C_MSG_BASE_SIZE	4

#define REG_I2C_CR1_INIT	(I2C_CR1_PE | I2C_CR1_RXIE | I2C_CR1_ADDRIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE | I2C_CR1_ERRIE)
#define REG_I2C_TIMEOUTR_INIT	(0)
#define REG_I2C_TIMINGR_INIT	0x007074AF // 0x00909FCE // ZGy_Note
#define STATE_TIMEOUT_MS		200 // 50 // Fix the time out issue
#define MESSAGE_HOLD_TIMEOUT_MS	100
#define MESSAGE_HOLD_TIME		7
#define MIN_IDLE_DELAY_MS		3
#define TASK_PASS_TIMEOUT		1000 // 20 // ZGy_Note

void SetState(I2C_Bus_Master *bm, I2C_BUS_STATE state)
{
	bm->Current_State = state;
	bm->Last_State_Change_Time = GetMsTicks();
	PS('>');
	PS('i' + bm->Current_State);
}

void Restore_I2C_Hal(I2C_Bus_Master *bm)
{
	bm->pi2c->CR1 = I2C_CR1_PE;
	bm->pi2c->CR2 = I2C_CR2_AUTOEND;
	bm->pi2c->TIMINGR = REG_I2C_TIMINGR_INIT; // 0x00909FCE; // ZGy_Note
	bm->pi2c->TIMEOUTR = 0;
}

void I2C_Bus_Master_Init(I2C_Bus_Master *bm, uint32_t Mutex_ID, I2C_TypeDef *_pi2c, Bus_RX_Callback_t RxCallback, const char *name, GPIO_TypeDef *SDA_Port, uint16_t SDA_Pin, GPIO_TypeDef *SCL_Port, uint16_t SCL_Pin)
{
	bm->Mutex.ID = Mutex_ID;
	bm->Mutex.Lock_Client = 0;
	bm->StateUpdatePeriod = IBM_IDLE_UPDATE_PERIOD;
	SetState(bm, ibm_I2C_IDLE);
	bm->KickBusProcess = 0;
	bm->Last_State_Change_Time = GetMsTicks();
	bm->RxData_Callback = RxCallback;
	bm->pi2c = _pi2c;
	bm->SDA_Port = SDA_Port;
	bm->SDA_Pin = SDA_Pin;
	bm->SCL_Port = SCL_Port;
	bm->SCL_Pin = SCL_Pin;
    bm->TxMsgQ = Create_MsgQ(MAX_NUM_TX_MESSAGES);
    IMsgQ_Init(&bm->RxIMsgQ, MAX_NUM_RX_MESSAGES, I2C_MAX_PACKET_SIZE, FBAlloc_Create);
    bm->Name = name;
    bm->Txfr_Count = 0;
    bm->Last_Txfr_Count = 0;
    bm->Task_Pass_Count = 0;

    uint8_t *pID = (uint8_t*)&bm->ID;

    switch((uint32_t)_pi2c)
    {
    case (uint32_t)I2C1:
    	*pID = IBM_MODULE_BUS_ID;
    	break;
    case (uint32_t)I2C2:
    	*pID = IBM_SENSOR_BUS_ID;
    	break;
    case (uint32_t)I2C3:
    	*pID = IBM_PANEL_BUS_2_ID;
    	break;
    case (uint32_t)I2C4:
    	*pID = IBM_PANEL_BUS_1_ID;
    	break;
    }

    RB8_Init(&State_buf);
    RB8_PushOverwrite(&State_buf, '!');
    RB8_PushOverwrite(&State_buf, '#');
    RB8_PushOverwrite(&State_buf, 'S');
    RB8_PushOverwrite(&State_buf, '?');

    bm->pi2c->CR1 |= I2C_CR1_PE;

    bm->LockedUp = 0;
}

void Reset_I2C_Peripheral(I2C_Bus_Master *bm)
{
	bm->pi2c->CR1 &= ~I2C_CR1_PE;
	HAL_Delay(1);
	bm->pi2c->CR1 |= I2C_CR1_PE;
	Restore_I2C_Hal(bm);
	SetState(bm, ibm_I2C_IDLE);
	Unlock_I2C_Bus(bm);
}

void Reset_I2C_Bus(I2C_Bus_Master *bm)
{
	// Disable and enable the I2C peripheral
	bm->pi2c->CR1 &= ~I2C_CR1_PE;
	HAL_Delay(1);
	bm->pi2c->CR1 |= I2C_CR1_PE;

	// If the SDA pin is stuck low, but the SCL pin is high, then cycle the SCL pin 9 times to reset most device I2C stacks.
	uint8_t SDA_Pos = Get_Bit_Pos(bm->SDA_Pin);
	uint8_t SCL_Pos = Get_Bit_Pos(bm->SCL_Pin);

	// Set pins to input
	bm->SDA_Port->MODER &= ~(0x03 << (SDA_Pos * 2));
	bm->SCL_Port->MODER &= ~(0x03 << (SCL_Pos * 2));

	if(!(bm->SDA_Port->IDR & (0x01 << SDA_Pos)) && (bm->SCL_Port->IDR & (0x01 << SCL_Pos)))
	{
		// Set pin to output, high
		bm->SCL_Port->ODR |= 0x01 << SCL_Pos;
		bm->SCL_Port->MODER |= 0x01 << (SCL_Pos * 2);

		uint8_t i;

		for(i=0; i<9; ++i)
		{
			bm->SCL_Port->ODR &= ~(0x01 << SCL_Pos);
			HAL_Delay(1);
			bm->SCL_Port->ODR |= (0x01 << SCL_Pos);
			HAL_Delay(1);
		}
	}

	// Set pins back to I2C Function
	bm->SDA_Port->MODER &= ~(0x03 << (SDA_Pos * 2));
	bm->SCL_Port->MODER &= ~(0x03 << (SCL_Pos * 2));
	bm->SDA_Port->MODER |= (0x02 << (SDA_Pos * 2));
	bm->SCL_Port->MODER |= (0x02 << (SCL_Pos * 2));

	Restore_I2C_Hal(bm);
	SetState(bm, ibm_I2C_IDLE);
	Unlock_I2C_Bus(bm);
}

bool Is_I2C_Bus_Locked_Up(I2C_Bus_Master *bm)
{
//	// If the SDA pin is stuck low, but the SCL pin is high, then the bus is locked up.
//	uint8_t SDA_Pos = Get_Bit_Pos(bm->SDA_Pin);
//	uint8_t SCL_Pos = Get_Bit_Pos(bm->SCL_Pin);
//
//	if(!(bm->SDA_Port->IDR & (0x01 << SDA_Pos)) && (bm->SCL_Port->IDR & (0x01 << SCL_Pos)))
//	{
//		return true;
//	}

	return false;
}

/*********************************************************************************************************
 * @note	Only the Reg_I2C messages use interrupt-based I2C--all others use blocking mode only.  Also
 * 			no I2C communications are initiated from interrupt handlers.  Thus thread safety is preserved
 * 			by ensuring that no Reg_I2C messages are in flight before beginning blocking mode comms.
 *********************************************************************************************************/
bool Is_I2C_Bus_Idle(I2C_Bus_Master *bm)
{
	return (bm->Current_State == ibm_I2C_IDLE) ? 1 : 0;
}

/*********************************************************************************************************
 * @note		Determine if slave is present on bus.
 * @param bm					Pointer to I2C_Bus_Master instance.
 * @param Addr					I2C slave address.
 * @param port					Module bus port
 *********************************************************************************************************/
uint8_t I2C_Bus_Master_Is_Slave_Present(I2C_Bus_Master *bm, uint8_t Addr)
{
	bm->pi2c->CR1 = I2C_CR1_PE;
	bm->pi2c->ICR |= I2C_ICR_NACKCF | I2C_ICR_STOPCF;
	bm->pi2c->CR2 = Addr << 1 | I2C_CR2_STOP | (1 << I2C_CR2_NBYTES_Pos);
	bm->pi2c->CR2 |= I2C_CR2_START;

	HAL_Delay(2);

	if(bm->pi2c->ISR & I2C_ISR_STOPF && !(bm->pi2c->ISR & I2C_ISR_NACKF))
	{
		bm->pi2c->ICR |= I2C_ICR_NACKCF | I2C_ICR_STOPCF;
		return 1;
	}

	bm->pi2c->ICR |= I2C_ICR_NACKCF | I2C_ICR_STOPCF;
	return 0;
}

#include "FW_Version.h"
/*********************************************************************************************************
 * @note		Response expected messages have proven to be unreliable since slave response times vary
 * 				and may lead to timeout.  Interrupt-driven response is preferred.
 * @param bm					Pointer to I2C_Bus_Master instance.
 * @param Addr					I2C slave address.
 * @param port					Module bus port
 * @param hdr					Pointer to packet header
 * @param pkt					Pointer to packet payload buffer
 * @param response_expected		0 - No response expected.
 * 								1 - Delayed response expected--slave needs some time to respond (deprecated)
 * 								2 - Immediate response expected.  Used only for interrupt response queries.
 *********************************************************************************************************/
uint8_t I2C_Bus_Master_Send(I2C_Bus_Master *bm, uint8_t Addr, uint8_t port, I2C_Packet_Header_Type *hdr, uint8_t *pkt, uint8_t response_expected)
{
	uint8_t res = 0;
	I2C_Message *msg = (I2C_Message*)pmdMalloc(I2C_MSG_BASE_SIZE + IBM_PACKET_HDR_SIZE + hdr->Data_Length + IBM_CRC_SIZE);

	if(msg)
	{
		res = 1;
	}
	msg->Addr = Addr;
	msg->Port = port;
	msg->Size = hdr->Data_Length + IBM_PACKET_HDR_SIZE + IBM_CRC_SIZE;
	msg->Response_Expected = response_expected;
	memcpy(msg->Buf, hdr, IBM_PACKET_HDR_SIZE);
	memcpy(msg->Buf + IBM_PACKET_HDR_SIZE, pkt, hdr->Data_Length);
	msg->Buf[IBM_PACKET_HDR_SIZE + hdr->Data_Length] = Get_CRC8(msg->Buf, IBM_PACKET_HDR_SIZE + hdr->Data_Length);

	Push_MsgQ(bm->TxMsgQ, msg);

	bm->KickBusProcess = 1;

	if(IS_XDBG_FLAG_SET(X_Debug_I2C_Stack))
	{
		char str[100];
		sprintf(str,"I2C_Bus_Master_Send:   %s, Port %d (%02X) Message Sent (%d bytes)!\r\n", bm->Name, (int)(bm->Port + 1), msg->Addr,(int)msg->Size);
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
		formatlog("Main_Controller","I2C",str);
#else
		formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,str);
#endif
		sprintf(str,"Message Sent (%d bytes)!\r\n",IBM_PACKET_HDR_SIZE + hdr->Data_Length + 1);
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
		formatlog("Main_Controller","I2C",str);
#else
		formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,str);
#endif
	}
	return res;
}

void Log_Packet(uint8_t *pkt)
{
	I2C_Packet_Header_Type *hdr = (I2C_Packet_Header_Type*)pkt;

	for(uint8_t i=0; i<hdr->Data_Length + IBM_PACKET_HDR_SIZE + IBM_CRC_SIZE; ++i)
	{
		if((i & 0x07) == 0)
		{
			aci_print("| ");
		}
		aci_print("%02X ", pkt[i]);

		if((i & 0x07) == 0x07)
		{
			aci_print("|\r\n");
		}
	}
}

static int i2c_print_n=0;
void I2C_Bus_Master_Tasks(I2C_Bus_Master *bm)
{
	++bm->Task_Pass_Count;

	// Process incoming messages
	while(IMsgQ_Get_Num_Packets(&bm->RxIMsgQ))
	{
		uint8_t *qpacket = (uint8_t*)IMsgQ_Peek(&bm->RxIMsgQ);
		uint16_t Packet_Size = ((I2C_Packet_Header_Type*)qpacket)->Data_Length + IBM_PACKET_HDR_SIZE + IBM_CRC_SIZE;

		// Copy the message and pass it along to the message processor
		uint8_t *packet = (uint8_t*)pmdMalloc(Packet_Size);
		memcpy((void*)packet, (void*)qpacket, Packet_Size);

		// Now that we have a copy, process it, and pop it off the queue only if processing is successful, otherwise
		// it will remain in the queue until the next pass.
		I2C_Packet_Header_Type *hdr = (I2C_Packet_Header_Type*)packet;

#ifdef LOW_LEVEL_I2C_DEBUG
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
		formatlog("Main_Controller","I2C","Incoming I2C Packet.\r\n");
#else
		formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"Incoming I2C Packet.\r\n");
#endif
		//Log_Packet(packet);
#endif

		if(hdr->Command == 0x7F && hdr->Packet_Number == 0xFF && hdr->Packet_Total == 0xFF && hdr->Data_Length == 0xFF)
		{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","I2C","ERROR:  %s I2C NULL Response Received\r\n", bm->Name);
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"ERROR:  %s I2C NULL Response Received\r\n", bm->Name);
#endif
			IMsgQ_Pop(&bm->RxIMsgQ);
		}
		else if(Get_CRC8((uint8_t*)hdr, Packet_Size) == 0)	// Check that the CRC is correct
		{
			if(Process_Message(bm, bm->Port, packet))
			{
				// Pop it off of the queue if we succeeded in processing the message.
				IMsgQ_Pop(&bm->RxIMsgQ);
			}
		}
		else
		{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","I2C","ERROR:  %s Invalid CRC received from address %02X\r\n", bm->Name, bm->Slave_Address >> 1);
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"ERROR:  %s Invalid CRC received from address %02X\r\n", bm->Name, bm->Slave_Address >> 1);
#endif
			if(msgflags.I2C)
				Log_Hex_Data(packet, Packet_Size);
			IMsgQ_Pop(&bm->RxIMsgQ);
		}



		if(IS_XDBG_FLAG_SET(X_Debug_I2C_Stack))
		{
			// Log it
			int i;
			//aci_print("\r\n-----------------------------------------------\r\n");
			char str[100];
			sprintf(str,"%s Port %d (%02X) Received %d bytes.\r\n",bm->Name, bm->Port + 1, bm->Slave_Address >> 1, Packet_Size);
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","I2C",str);
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,str);
#endif
			//aci_print("-----------------------------------------------\r\n");
			for(i=0; i<Packet_Size; ++i)
			{
				if(i && ((i & 0x000F) == 0))
				{
				//	aci_print("\r\n");
				}
				//aci_print("%02X ", bm->RX_Packet[i]);
			}
			//aci_print("\r\n-----------------------------------------------\r\n");
		}
	}

	switch(bm->Current_State)
	{
	case ibm_I2C_RX:
	case ibm_I2C_TX:
	case ibm_I2C_RX_RESTART:
		if(bm->Last_Txfr_Count == bm->Txfr_Count)
		{
			if(bm->Task_Pass_Count > TASK_PASS_TIMEOUT)
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller","I2C","Error:  %s Task Pass Count Timeout!\r\n", bm->Name);
#else
				formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"Error:  %s Task Pass Count Timeout!\r\n", bm->Name);
#endif
				Set_I2C_Bus_Function_Test_Trigger(bm);
				bm->Current_State = ibm_I2C_IDLE;
				Unlock_I2C_Bus(bm);
				return;
			}
		}
		else
		{
			bm->Last_Txfr_Count = bm->Txfr_Count;
		}
		break;
	case ibm_I2C_IDLE:
		if(bm->Mutex.Lock_Client == (uint32_t)bm)
		{
			// The bus should have been unlocked at this point.
			Unlock_I2C_Bus(bm);
		}

		if(bm->pi2c->ISR & I2C_ISR_BUSY)
		{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","I2C","Error:  %s BUSY While IDLE!\r\n", bm->Name);
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"Error:  %s BUSY While IDLE!\r\n", bm->Name);
#endif
			Set_I2C_Bus_Function_Test_Trigger(bm);
			return;
		}

		// Is the bus locked up?
		if(!(bm->SDA_Port->IDR & bm->SDA_Pin) || !(bm->SCL_Port->IDR & bm->SCL_Pin))
		{
			Set_I2C_Bus_Function_Test_Trigger(bm);
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","I2C","Error: %s %s LockUp Detected!!\r\n", bm->Name,((bm->pi2c->ISR & I2C_ISR_BUSY) ? "Busy" : ""));
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"Error: %s %s LockUp Detected!!\r\n", bm->Name,((bm->pi2c->ISR & I2C_ISR_BUSY) ? "Busy" : ""));
#endif
			return;
		}

		if(!Is_MsgQ_Empty(bm->TxMsgQ))
		{
			if(TimedOut(bm->Last_State_Change_Time, MIN_IDLE_DELAY_MS))
			{
				PS('b');
				I2C_Message *msg = (I2C_Message*)Get_Next_MsgQ_Msg(bm->TxMsgQ);
				Pop_MsgQ(bm->TxMsgQ);
				if(msg && msg->Size <= I2C_MSG_MAX_SIZE && Lock_I2C_Bus(bm))
				{
					PS('c');
					// Copy packet
					memcpy((uint8_t*)(bm->TX_Packet), msg->Buf, msg->Size);
					bm->Slave_Address = msg->Addr << 1;
					bm->Response_Expected = msg->Response_Expected;
					bm->Port = msg->Port;

					//	This is not necessary unless we decide to use the UART to send/receive messages to/from modules.
//					if(msg->Port != (eMODULE_PORTS)Module_Bus.Active_Module_Port)
//					{
//						Switch_Module_Port(msg->Port);
//					}

					SetState(bm, ibm_I2C_TX);
					bm->Task_Pass_Count = 0;
					bm->tbuf = bm->TX_Packet;
					bm->tsize = msg->Size;
					pmdFree(msg);
	
					bm->StateUpdatePeriod = IBM_ACTIVE_UPDATE_PERIOD;
	
					if(IS_XDBG_FLAG_SET(X_Debug_I2C_Stack))
					{
						// Log it
						int i;
						//aci_print("\r\n-----------------------------------------------\r\n");
						char str[100];
						sprintf(str,"%s Port %d (%02X), Sending %d bytes.\r\n", bm->Name, bm->Port + 1, bm->Slave_Address >> 1, bm->tsize);
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
						formatlog("Main_Controller","I2C",str);
#else
						formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,str);
#endif
						//aci_print("-----------------------------------------------\r\n");
						for(i=0; i<bm->tsize; ++i)
						{
							if(i && ((i & 0x000F) == 0))
							{
								//aci_print("\r\n");
							}
							//aci_print("%02X ", bm->TX_Packet[i]);
						}
						//aci_print("\r\n-----------------------------------------------\r\n");
					}
	
					bm->pi2c->CR1 = REG_I2C_CR1_INIT;
					bm->pi2c->TIMEOUTR = REG_I2C_TIMEOUTR_INIT;
					bm->pi2c->TIMINGR = REG_I2C_TIMINGR_INIT;
					bm->pi2c->CR2 &= ~I2C_CR2_RD_WRN;		// Set to TX mode
					bm->pi2c->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
					bm->pi2c->CR2 &= ~I2C_CR2_SADD;			// Clear the address field
					bm->pi2c->CR2 = bm->Slave_Address | (bm->tsize << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
					bm->pi2c->CR1 |= I2C_CR1_TXIE;
		//				bm->pi2c->TXDR = *(bm->tbuf)++;
		//				--(bm->tsize);
					bm->pi2c->CR2 |= I2C_CR2_START;
//					Unlock_I2C_Bus(bm);		//added By Steve Rui. Need to be careful
				}
				else
				{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
					formatlog("Main_Controller","I2C","I2C Size too large: %d\r\n",msg->Size);
#else
					formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"I2C Size too large: %d\r\n",msg->Size);
#endif
					Unlock_I2C_Bus(bm);
				}
			}
		}
		else 	// Message Queue is Empty
		{
			bm->StateUpdatePeriod = IBM_IDLE_UPDATE_PERIOD;
			Unlock_I2C_Bus(bm);
		}
		break;
	case ibm_I2C_TXHOLD:	// Slave nacked because it was not ready to receive another message, so delay and restart.
		PS('H');
		if(TimedOut(bm->Hold_Start_Time, MESSAGE_HOLD_TIMEOUT_MS))
		{
			bm->tbuf = NULL;
			bm->tsize = 0;
		}
		else if(TimedOut(bm->Last_State_Change_Time, MESSAGE_HOLD_TIME))
		{
			TP_L;
			SetState(bm, ibm_I2C_TX);
			bm->pi2c->CR1 = REG_I2C_CR1_INIT;
			bm->pi2c->TIMEOUTR = REG_I2C_TIMEOUTR_INIT;
			bm->pi2c->TIMINGR = REG_I2C_TIMINGR_INIT;
			bm->pi2c->CR2 &= ~(I2C_CR2_RD_WRN | I2C_CR2_NBYTES | I2C_CR2_SADD);		// Set to TX mode, Zero the NBYTES field, Clear the address field
			bm->pi2c->CR2 = bm->Slave_Address | (bm->tsize << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
			bm->pi2c->CR1 |= I2C_CR1_TXIE;
			bm->pi2c->CR2 |= I2C_CR2_START;
		}
		break;
	case ibm_I2C_MSG_TX_COMPLETE:
		bm->tbuf = NULL;
		bm->tsize = 0;
#ifdef USE_LEGACY_I2C_IMPLEMENTATION
		if(bm->Response_Expected == 1 || bm->Response_Expected == 2)
#else
		if(bm->Response_Expected == 1)
#endif
		{
			HAL_Delay(3);
			bm->rsize = 0;
			bm->rbuf = bm->RX_Packet;
			SetState(bm, ibm_I2C_RX);
			bm->Task_Pass_Count = 0;
			bm->pi2c->CR2 |= bm->Slave_Address | I2C_CR2_RD_WRN;
			bm->pi2c->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
			bm->pi2c->CR2 |= ((IBM_PACKET_HDR_SIZE) << I2C_CR2_NBYTES_Pos);
			bm->pi2c->CR2 &= ~I2C_CR2_RELOAD;
			bm->pi2c->CR2 &= ~I2C_CR2_AUTOEND;
			bm->pi2c->CR1 |= I2C_CR1_RXIE;
			bm->pi2c->CR2 |= I2C_CR2_START;

			PS('Q');
		}
		else
		{
			SetState(bm, ibm_I2C_IDLE);
			Unlock_I2C_Bus(bm);
			// Kick the process again in case buffer not empty
			bm->KickBusProcess = 1;
		}
		break;
	}

#if defined(LOW_LEVEL_I2C_DEBUG) || defined(PASSIVE_LOW_LEVEL_I2C_DEBUG)
#ifndef LOW_LEVEL_I2C_DEBUG
	if(Dump_I2C_State_Buf)
#endif
	{
		while(!RB8_IsEmpty(&State_buf))
		{
			__io_putchar(RB8_Pop(&State_buf));
			Dump_I2C_State_Buf = 0;
		}
	}
#endif

	if(bm->Error)
	{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
		formatlog("Main_Controller","I2C","I2C Err:  0x%lX\r\n", bm->Error);
#else
		formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"I2C Err:  0x%lX\r\n", bm->Error);
#endif
		bm->Error = 0;
	}

//	if(TimedOut(bm->Last_State_Change_Time, STATE_TIMEOUT_MS) && Is_I2C_Bus_Locked_Up(bm))
//	{
//		aci_print("\r\nERROR:  I2C Bus %s Locked Up--Performing Emergency Bus Reset\r\n", bm->Name);
//		Reset_I2C_Bus(bm);
//	}

//	//bm->Current_State != ibm_I2C_TX &&
	if(bm->Current_State != ibm_I2C_IDLE && bm->Current_State != ibm_I2C_TX && TimedOut(bm->Last_State_Change_Time, STATE_TIMEOUT_MS))
	{
		Set_I2C_Bus_Function_Test_Trigger(bm);
		Unlock_I2C_Bus(bm);
		i2c_print_n++;
		if(i2c_print_n<10000)
			return;
		i2c_print_n=0;
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
		formatlog("Main_Controller","I2C","ERROR:  I2C Bus %s Locked Up--Performing Bus Reset\r\n", bm->Name);
#else
		formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_I2C,"ERROR:  I2C Bus %s Locked Up--Performing Bus Reset\r\n", bm->Name);
#endif
	}
}


// uncomment this to debug I2Cx handler.
// Be sure to comment and copy any changes to I2C_Int_Handler.Impl
//#define USE_REG_I2C2_HDLR_DEBUG

#ifdef USE_REG_I2C1_HDLR_DEBUG

#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C1
#define BM				Module_Bus_Master
#define I2CHANDLER		I2C1_EV_IRQHandler
#define I2CERRHANDLER	I2C1_ER_IRQHandler

#elif defined(USE_REG_I2C2_HDLR_DEBUG)

#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C2
#define BM				Sensor_Bus_Master
#define I2CHANDLER		I2C2_EV_IRQHandler
#define I2CERRHANDLER	I2C2_ER_IRQHandler

#elif defined(USE_REG_I2C3_HDLR_DEBUG)

#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C3
#define BM				Panel_Bus_2_Master
#define I2CHANDLER		I2C3_EV_IRQHandler
#define I2CERRHANDLER	I2C3_ER_IRQHandler

#elif defined(USE_REG_I2C4_HDLR_DEBUG)

#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C4
#define BM				Panel_Bus_1_Master
#define I2CHANDLER		I2C4_EV_IRQHandler
#define I2CERRHANDLER	I2C4_ER_IRQHandler

#endif


#if defined(USE_REG_I2C1_HDLR_DEBUG) || defined(USE_REG_I2C2_HDLR_DEBUG) || defined(USE_REG_I2C3_HDLR_DEBUG) || defined(USE_REG_I2C4_HDLR_DEBUG)
#ifdef RELEASE_BUILD
#warning ERROR:  DEBUG I2C HANDLER ENABLED!
#else
#warning WARNING:  DEBUG I2C HANDLER ENABLED!
#endif
void I2CHANDLER(void)
{
	uint32_t status = I2CPERIF->ISR;

	PS('G');
	if((I2CPERIF->CR1 & I2C_CR1_TXIE) && (status & I2C_ISR_TXIS))
	{
		PS('t');
		// TX buffer empty--automatically cleared on write
		if(BM.tsize)
		{
			I2CPERIF->TXDR = *(BM.tbuf)++;
			--(BM.tsize);
			PS('T');
		}
		else
		{
			I2CPERIF->CR1 &= ~I2C_CR1_TXIE;
			BM.tbuf = NULL;
			PS('D');
		}
	}
	if((I2CPERIF->CR1 & I2C_CR1_TXIE) && (status & I2C_ISR_TXE))
	{
		PS('V');
	}
	if(status & I2C_ISR_RXNE)
	{
		 // Read buffer not empty--automatically cleared on read
		*(BM.rbuf)++ = I2CPERIF->RXDR;
		++BM.rsize;
		PS('R');
	}
	if(status & I2C_ISR_STOPF)
	{
		// Stop condition detected (STOPCF to clear)
		PS('S');
		I2CPERIF->ICR |= I2C_ICR_STOPCF;
		if(BM.Current_State == ibm_I2C_RX)
		{
			SetState(&BM, ibm_I2C_RX_RESTART);
			PS('j');
		}
		else if(BM.Current_State == ibm_I2C_RX_RESTART)
		{
			BM.rbuf = 0;
			PS('i');
			if(BM.rsize)
			{
				I2C_Packet_Header_Type *hdr = (I2C_Packet_Header_Type*)BM.RX_Packet;
				uint16_t Packet_Size = hdr->Data_Length + IBM_PACKET_HDR_SIZE + IBM_CRC_SIZE;

				IMsgQ_Push(&BM.RxIMsgQ, (MODULE_BUS_PACKET*)BM.RX_Packet, MIN(Packet_Size, sizeof(MODULE_BUS_PACKET)));
				BM.rsize = 0;
				BM.rbuf = NULL;
			}
			SetState(&BM, ibm_I2C_IDLE);

			// Unlock the bus immediately so that immediate mode tasks can wait for the mutex lock
			Unlock_I2C_Bus(&BM);
		}
		else if(BM.Current_State == ibm_I2C_TX)
		{
			PS('o');
			I2CPERIF->CR1 &= ~I2C_CR1_TXIE;
			BM.tbuf = NULL;
			BM.tsize = 0;
#ifdef USE_LEGACY_I2C_IMPLEMENTATION
			if(BM.Response_Expected == 1 || BM.Response_Expected == 2)
			{
				SetState(&BM, ibm_I2C_MSG_TX_COMPLETE);
			}
#else
			if(BM.Response_Expected == 1)
			{
				SetState(&BM, ibm_I2C_MSG_TX_COMPLETE);
			}
			else if(BM.Response_Expected == 2)
			{
				PS('Q');
				BM.rsize = 0;
				BM.rbuf = BM.RX_Packet;
				SetState(&BM, ibm_I2C_RX);
				BM.pi2c->CR2 |= BM.Slave_Address | I2C_CR2_RD_WRN;
				BM.pi2c->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
				BM.pi2c->CR2 |= ((IBM_PACKET_HDR_SIZE) << I2C_CR2_NBYTES_Pos);
				BM.pi2c->CR2 &= ~I2C_CR2_RELOAD;
				BM.pi2c->CR2 &= ~I2C_CR2_AUTOEND;
				BM.pi2c->CR1 |= I2C_CR1_RXIE;
				BM.pi2c->CR2 |= I2C_CR2_START;
			}
#endif
			else
			{
				PS('M');
				SetState(&BM, ibm_I2C_IDLE);
				Unlock_I2C_Bus(&BM);
			}
		}
		else if(BM.Current_State != ibm_I2C_TXHOLD)
		{
			SetState(&BM, ibm_I2C_IDLE);
			Unlock_I2C_Bus(&BM);
		}

		// Kick the process
		BM.KickBusProcess = 1;
	}
	if(status & I2C_ISR_TC)
	{
		// Transfer Complete detected (clear on start bit or stop bit)
		PS('O');

		if(BM.Current_State == ibm_I2C_RX)
		{
			PS('g');
			I2CPERIF->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
			I2CPERIF->CR2 |= ((BM.RX_Packet[IBM_PKT_LEN_INDEX] + IBM_CRC_SIZE) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
			I2CPERIF->CR2 |= I2C_CR2_START;
		}
		else
		{
			PS('n');
			I2CPERIF->CR2 |= I2C_CR2_STOP;
		}
	}
	if(status & I2C_ISR_NACKF)
	{
		// NACK received (NACKCF to clear)
		PS('N');
		I2CPERIF->ICR |= I2C_ICR_NACKCF;
		if(BM.Current_State == ibm_I2C_TX)
		{
			if(BM.TX_Packet - BM.tbuf < 3 && BM.TX_Packet - BM.tbuf)
		{
				PS('U');
				TP_H;
				// Slave wasn't ready, delay, then restart transmit
				BM.tbuf = BM.TX_Packet;
				++BM.tsize;
				SetState(&BM, ibm_I2C_TXHOLD);
				BM.Hold_Start_Time = GetMsTicks();
		}
		else
		{
			BM.tbuf = NULL;
			BM.tsize = 0;
				SetState(&BM, ibm_I2C_IDLE);
			}
		}
		else if(BM.Current_State == ibm_I2C_RX)
		{
			BM.rbuf = NULL;
			BM.rsize = 0;
			SetState(&BM, ibm_I2C_IDLE);
		}
		Unlock_I2C_Bus(&BM);
	}
	if(status & I2C_ISR_BERR)
	{
		// Bus Error detected (BERRCF to clear)
		PS('E');
		I2CPERIF->ICR |= I2C_ICR_BERRCF;
	}
	if(status & I2C_ISR_BUSY)
	{
		// Bus busy received (Automatically cleared when STOPF received)
//		PS('B');
	}
	if(status & I2C_ISR_TIMEOUT)
	{
		// Timeout occurred (TIMOUTCF to clear)
		PS('I');
		I2CPERIF->ICR |= I2C_ICR_TIMOUTCF;
	}
}


void I2CERRHANDLER(void)
{
	uint32_t status = I2CPERIF->ISR;
	BM.Error = status & 0x0000FF00;
	PS('X');
	if(status & 3)
	{
		PS('x');
	}
	if(status & I2C_ISR_ARLO)
	{
		PS('L');
		I2CPERIF->ICR |= I2C_ICR_ARLOCF;
	}
	if(status & I2C_ISR_BERR)
	{
		PS('8');
		I2CPERIF->ICR |= I2C_ICR_BERRCF;
	}
	if(status & I2C_ISR_OVR)
	{
		PS('v');
		I2CPERIF->ICR |= I2C_ICR_OVRCF;
	}
	if(status & I2C_ISR_TIMEOUT)
	{
		PS('m');
		I2CPERIF->ICR |= I2C_ICR_TIMOUTCF;
	}
	if(status & I2C_ISR_PECERR)
	{
		PS('5');
		I2CPERIF->ICR |= I2C_ICR_PECCF;
	}
	if(status & I2C_ISR_ALERT)
	{
		PS('6');
		I2CPERIF->ICR |= I2C_ICR_ALERTCF;
	}
}
#endif


extern I2C_Bus_Master Sensor_Bus_Master;
extern I2C_Bus_Master Panel_Bus_2_Master;
extern I2C_Bus_Master Panel_Bus_1_Master;

#ifndef USE_REG_I2C1_HDLR_DEBUG
#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C1
#define BM				Module_Bus_Master
#define I2CHANDLER		I2C1_EV_IRQHandler
#define I2CERRHANDLER	I2C1_ER_IRQHandler
//#include "I2C_Int_Handler.Impl"
#endif

#ifndef USE_REG_I2C2_HDLR_DEBUG
#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C2
#define BM				Sensor_Bus_Master
#define I2CHANDLER		I2C2_EV_IRQHandler
#define I2CERRHANDLER	I2C2_ER_IRQHandler
//#include "I2C_Int_Handler.Impl"
#endif

#ifndef USE_REG_I2C3_HDLR_DEBUG
#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C3
#define BM				Panel_Bus_2_Master
#define I2CHANDLER		I2C3_EV_IRQHandler
#define I2CERRHANDLER	I2C3_ER_IRQHandler
//#include "I2C_Int_Handler.Impl"
#endif

#ifndef USE_REG_I2C4_HDLR_DEBUG
#undef I2CPERIF
#undef BM
#undef I2CHANDLER
#undef I2CERRHANDLER
#define I2CPERIF		I2C4
#define BM				Panel_Bus_1_Master
#define I2CHANDLER		I2C4_EV_IRQHandler
#define I2CERRHANDLER	I2C4_ER_IRQHandler
//#include "I2C_Int_Handler.Impl"
#endif


uint8_t Lock_I2C_Bus(I2C_Bus_Master *bm)
{
	uint8_t result = LockSMutex(&bm->Mutex, I2C_BUS_MASTER_MUTEX_TAG);

	if(result)
	{
		TP_H;
	}
	return result;
}

uint8_t Wait_For_I2C_Bus_Lock(I2C_Bus_Master *bm, uint32_t Timeout)
{
	uint8_t result = WaitForSMutex(&bm->Mutex, I2C_BUS_MASTER_MUTEX_TAG, Timeout);

	if(result)
	{
		TP_H;
	}
	return result;
}

