/*
 * I2C3_Int_Handler.c
 *
 *  Created on: Mar 28, 2024
 *      Author: stever
 */

#include "1imp.h"
/*================================================================*/
void I2C3_EV_IRQHandler(void);
void I2C3_EV_IRQHandler(void)
{
	TPIRQH;
	uint32_t status = I2C3->ISR;

	PS('G');
	if((I2C3->CR1 & I2C_CR1_TXIE) && (status & I2C_ISR_TXIS))
	{
		PS('t');
		// TX buffer empty--automatically cleared on write
		if(Panel_Bus_2_Master.tsize)
		{
			I2C3->TXDR = *(Panel_Bus_2_Master.tbuf)++;
			--(Panel_Bus_2_Master.tsize);
			++Panel_Bus_2_Master.Txfr_Count;
			PS('T');
		}
		else
		{
			I2C3->CR1 &= ~I2C_CR1_TXIE;
			Panel_Bus_2_Master.tbuf = NULL;
			PS('D');
		}
	}
	if((I2C3->CR1 & I2C_CR1_TXIE) && (status & I2C_ISR_TXE))
	{
		PS('V');
	}
	if(status & I2C_ISR_RXNE)
	{
		 // Read buffer not empty--automatically cleared on read
		*(Panel_Bus_2_Master.rbuf)++ = I2C3->RXDR;
		++Panel_Bus_2_Master.rsize;
			++Panel_Bus_2_Master.Txfr_Count;
		PS('R');
	}
	if(status & I2C_ISR_STOPF)
	{
		// Stop condition detected (STOPCF to clear)
		PS('S');
		I2C3->ICR |= I2C_ICR_STOPCF;
		if(Panel_Bus_2_Master.Current_State == ibm_I2C_RX)
		{
			SetState(&Panel_Bus_2_Master, ibm_I2C_RX_RESTART);
			PS('j');
		}
		else if(Panel_Bus_2_Master.Current_State == ibm_I2C_RX_RESTART)
		{
			Panel_Bus_2_Master.rbuf = 0;
			PS('i');
			if(Panel_Bus_2_Master.rsize)
			{
				I2C_Packet_Header_Type *hdr = (I2C_Packet_Header_Type*)Panel_Bus_2_Master.RX_Packet;
				uint16_t Packet_Size = hdr->Data_Length + IBM_PACKET_HDR_SIZE + IBM_CRC_SIZE;

				IMsgQ_Push(&Panel_Bus_2_Master.RxIMsgQ, (MODULE_BUS_PACKET*)Panel_Bus_2_Master.RX_Packet, MIN(Packet_Size, sizeof(MODULE_BUS_PACKET)));
				Panel_Bus_2_Master.rsize = 0;
				Panel_Bus_2_Master.rbuf = NULL;
				++Panel_Bus_2_Master.Txfr_Count;
			}
			SetState(&Panel_Bus_2_Master, ibm_I2C_IDLE);
			
			// Unlock the bus immediately so that immediate mode tasks can wait for the mutex lock
			Unlock_I2C_Bus(&Panel_Bus_2_Master);
		}
		else if(Panel_Bus_2_Master.Current_State == ibm_I2C_TX)
		{
			PS('o');
			I2C3->CR1 &= ~I2C_CR1_TXIE;
			Panel_Bus_2_Master.tbuf = NULL;
			Panel_Bus_2_Master.tsize = 0;
#ifdef USE_LEGACY_I2C_IMPLEMENTATION
			if(Panel_Bus_2_Master.Response_Expected == 1 || Panel_Bus_2_Master.Response_Expected == 2)
			{
				SetState(&Panel_Bus_2_Master, ibm_I2C_MSG_TX_COMPLETE);
			}
#else
			if(Panel_Bus_2_Master.Response_Expected == 1)
			{
				SetState(&Panel_Bus_2_Master, ibm_I2C_MSG_TX_COMPLETE);
			}
			else if(Panel_Bus_2_Master.Response_Expected == 2)
			{
				PS('Q');
				Panel_Bus_2_Master.rsize = 0;
				Panel_Bus_2_Master.rbuf = Panel_Bus_2_Master.RX_Packet;
				SetState(&Panel_Bus_2_Master, ibm_I2C_RX);
				Panel_Bus_2_Master.pi2c->CR2 |= Panel_Bus_2_Master.Slave_Address | I2C_CR2_RD_WRN;
				Panel_Bus_2_Master.pi2c->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
				Panel_Bus_2_Master.pi2c->CR2 |= ((IBM_PACKET_HDR_SIZE) << I2C_CR2_NBYTES_Pos);
				Panel_Bus_2_Master.pi2c->CR2 &= ~I2C_CR2_RELOAD;
				Panel_Bus_2_Master.pi2c->CR2 &= ~I2C_CR2_AUTOEND;
				Panel_Bus_2_Master.pi2c->CR1 |= I2C_CR1_RXIE;
				Panel_Bus_2_Master.pi2c->CR2 |= I2C_CR2_START;
			}
#endif
			else
			{
				PS('M');
				SetState(&Panel_Bus_2_Master, ibm_I2C_IDLE);
				Unlock_I2C_Bus(&Panel_Bus_2_Master);
			}
		}
		else if(Panel_Bus_2_Master.Current_State != ibm_I2C_TXHOLD)
		{
			SetState(&Panel_Bus_2_Master, ibm_I2C_IDLE);
			Unlock_I2C_Bus(&Panel_Bus_2_Master);
		}
		
		// Kick the process
		Panel_Bus_2_Master.KickBusProcess = 1;
	}
	if(status & I2C_ISR_TC)
	{
		// Transfer Complete detected (clear on start bit or stop bit)
		PS('O');

		if(Panel_Bus_2_Master.Current_State == ibm_I2C_RX)
		{
			PS('g');
			I2C3->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
			I2C3->CR2 |= ((Panel_Bus_2_Master.RX_Packet[IBM_PKT_LEN_INDEX] + IBM_CRC_SIZE) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
			I2C3->CR2 |= I2C_CR2_START;
		}
		else
		{
			PS('n');
			I2C3->CR2 |= I2C_CR2_STOP;
		}
	}
	if(status & I2C_ISR_NACKF)
	{
		// NACK received (NACKCF to clear)
		PS('N');
		I2C3->ICR |= I2C_ICR_NACKCF;
		if(Panel_Bus_2_Master.Current_State == ibm_I2C_TX)
		{
			if(Panel_Bus_2_Master.TX_Packet - Panel_Bus_2_Master.tbuf < 3 && Panel_Bus_2_Master.TX_Packet - Panel_Bus_2_Master.tbuf)
		{
				PS('U');
				// Slave wasn't ready, delay, then restart transmit
				Panel_Bus_2_Master.tbuf = Panel_Bus_2_Master.TX_Packet;
				++Panel_Bus_2_Master.tsize;
				SetState(&Panel_Bus_2_Master, ibm_I2C_TXHOLD);
				Panel_Bus_2_Master.Hold_Start_Time = GetMsTicks();
		}
		else
		{
			Panel_Bus_2_Master.tbuf = NULL;
			Panel_Bus_2_Master.tsize = 0;
				SetState(&Panel_Bus_2_Master, ibm_I2C_IDLE);
			}
		}
		else if(Panel_Bus_2_Master.Current_State == ibm_I2C_RX)
		{
			Panel_Bus_2_Master.rbuf = NULL;
			Panel_Bus_2_Master.rsize = 0;
			SetState(&Panel_Bus_2_Master, ibm_I2C_IDLE);
		}
		Unlock_I2C_Bus(&Panel_Bus_2_Master);
	}
	if(status & I2C_ISR_BERR)
	{
		// Bus Error detected (BERRCF to clear)
		PS('E');
		I2C3->ICR |= I2C_ICR_BERRCF;
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
		I2C3->ICR |= I2C_ICR_TIMOUTCF;
	}
	TPIRQL;
}

/*================================================================*/
void I2C3_ER_IRQHandler(void);
void I2C3_ER_IRQHandler(void)
{
	TPIRQH;
	uint32_t status = I2C3->ISR;
	Panel_Bus_2_Master.Error = status & 0x0000FF00;
	PS('X');
	if(status & 3)
	{
		PS('x');
	}
	if(status & I2C_ISR_ARLO)
	{
		PS('L');
		I2C3->ICR |= I2C_ICR_ARLOCF;
	}
	if(status & I2C_ISR_BERR)
	{
		PS('8');
		I2C3->ICR |= I2C_ICR_BERRCF;
	}
	if(status & I2C_ISR_OVR)
	{
		PS('v');
		I2C3->ICR |= I2C_ICR_OVRCF;
	}
	if(status & I2C_ISR_TIMEOUT)
	{
		PS('m');
		I2C3->ICR |= I2C_ICR_TIMOUTCF;
	}
	if(status & I2C_ISR_PECERR)
	{
		PS('5');
		I2C3->ICR |= I2C_ICR_PECCF;
	}
	if(status & I2C_ISR_ALERT)
	{
		PS('6');
		I2C3->ICR |= I2C_ICR_ALERTCF;
	}
	TPIRQL;
}
/*================================================================*/
