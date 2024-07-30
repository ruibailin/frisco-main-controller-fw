/*
 * I2C1_Int_Handler.c
 *
 *  Created on: Mar 28, 2024
 *      Author: stever
 */

#include "1imp.h"
/*================================================================*/
void I2C1_EV_IRQHandler(void);
void I2C1_EV_IRQHandler(void)
{
	TPIRQH;
	uint32_t status = I2C1->ISR;

	PS('G');
	if((I2C1->CR1 & I2C_CR1_TXIE) && (status & I2C_ISR_TXIS))
	{
		PS('t');
		// TX buffer empty--automatically cleared on write
		if(Module_Bus_Master.tsize)
		{
			I2C1->TXDR = *(Module_Bus_Master.tbuf)++;
			--(Module_Bus_Master.tsize);
			++Module_Bus_Master.Txfr_Count;
			PS('T');
		}
		else
		{
			I2C1->CR1 &= ~I2C_CR1_TXIE;
			Module_Bus_Master.tbuf = NULL;
			PS('D');
		}
	}
	if((I2C1->CR1 & I2C_CR1_TXIE) && (status & I2C_ISR_TXE))
	{
		PS('V');
	}
	if(status & I2C_ISR_RXNE)
	{
		 // Read buffer not empty--automatically cleared on read
		*(Module_Bus_Master.rbuf)++ = I2C1->RXDR;
		++Module_Bus_Master.rsize;
			++Module_Bus_Master.Txfr_Count;
		PS('R');
	}
	if(status & I2C_ISR_STOPF)
	{
		// Stop condition detected (STOPCF to clear)
		PS('S');
		I2C1->ICR |= I2C_ICR_STOPCF;
		if(Module_Bus_Master.Current_State == ibm_I2C_RX)
		{
			SetState(&Module_Bus_Master, ibm_I2C_RX_RESTART);
			PS('j');
		}
		else if(Module_Bus_Master.Current_State == ibm_I2C_RX_RESTART)
		{
			Module_Bus_Master.rbuf = 0;
			PS('i');
			if(Module_Bus_Master.rsize)
			{
				I2C_Packet_Header_Type *hdr = (I2C_Packet_Header_Type*)Module_Bus_Master.RX_Packet;
				uint16_t Packet_Size = hdr->Data_Length + IBM_PACKET_HDR_SIZE + IBM_CRC_SIZE;

				IMsgQ_Push(&Module_Bus_Master.RxIMsgQ, (MODULE_BUS_PACKET*)Module_Bus_Master.RX_Packet, MIN(Packet_Size, sizeof(MODULE_BUS_PACKET)));
				Module_Bus_Master.rsize = 0;
				Module_Bus_Master.rbuf = NULL;
				++Module_Bus_Master.Txfr_Count;
			}
			SetState(&Module_Bus_Master, ibm_I2C_IDLE);
			
			// Unlock the bus immediately so that immediate mode tasks can wait for the mutex lock
			Unlock_I2C_Bus(&Module_Bus_Master);
		}
		else if(Module_Bus_Master.Current_State == ibm_I2C_TX)
		{
			PS('o');
			I2C1->CR1 &= ~I2C_CR1_TXIE;
			Module_Bus_Master.tbuf = NULL;
			Module_Bus_Master.tsize = 0;
#ifdef USE_LEGACY_I2C_IMPLEMENTATION
			if(Module_Bus_Master.Response_Expected == 1 || Module_Bus_Master.Response_Expected == 2)
			{
				SetState(&Module_Bus_Master, ibm_I2C_MSG_TX_COMPLETE);
			}
#else
			if(Module_Bus_Master.Response_Expected == 1)
			{
				SetState(&Module_Bus_Master, ibm_I2C_MSG_TX_COMPLETE);
			}
			else if(Module_Bus_Master.Response_Expected == 2)
			{
				PS('Q');
				Module_Bus_Master.rsize = 0;
				Module_Bus_Master.rbuf = Module_Bus_Master.RX_Packet;
				SetState(&Module_Bus_Master, ibm_I2C_RX);
				Module_Bus_Master.pi2c->CR2 |= Module_Bus_Master.Slave_Address | I2C_CR2_RD_WRN;
				Module_Bus_Master.pi2c->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
				Module_Bus_Master.pi2c->CR2 |= ((IBM_PACKET_HDR_SIZE) << I2C_CR2_NBYTES_Pos);
				Module_Bus_Master.pi2c->CR2 &= ~I2C_CR2_RELOAD;
				Module_Bus_Master.pi2c->CR2 &= ~I2C_CR2_AUTOEND;
				Module_Bus_Master.pi2c->CR1 |= I2C_CR1_RXIE;
				Module_Bus_Master.pi2c->CR2 |= I2C_CR2_START;
			}
#endif
			else
			{
				PS('M');
				SetState(&Module_Bus_Master, ibm_I2C_IDLE);
				Unlock_I2C_Bus(&Module_Bus_Master);
			}
		}
		else if(Module_Bus_Master.Current_State != ibm_I2C_TXHOLD)
		{
			SetState(&Module_Bus_Master, ibm_I2C_IDLE);
			Unlock_I2C_Bus(&Module_Bus_Master);
		}
		
		// Kick the process
		Module_Bus_Master.KickBusProcess = 1;
	}
	if(status & I2C_ISR_TC)
	{
		// Transfer Complete detected (clear on start bit or stop bit)
		PS('O');

		if(Module_Bus_Master.Current_State == ibm_I2C_RX)
		{
			PS('g');
			I2C1->CR2 &= ~I2C_CR2_NBYTES; 		// Zero the NBYTES field
			I2C1->CR2 |= ((Module_Bus_Master.RX_Packet[IBM_PKT_LEN_INDEX] + IBM_CRC_SIZE) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
			I2C1->CR2 |= I2C_CR2_START;
		}
		else
		{
			PS('n');
			I2C1->CR2 |= I2C_CR2_STOP;
		}
	}
	if(status & I2C_ISR_NACKF)
	{
		// NACK received (NACKCF to clear)
		PS('N');
		I2C1->ICR |= I2C_ICR_NACKCF;
		if(Module_Bus_Master.Current_State == ibm_I2C_TX)
		{
			if(Module_Bus_Master.TX_Packet - Module_Bus_Master.tbuf < 3 && Module_Bus_Master.TX_Packet - Module_Bus_Master.tbuf)
		{
				PS('U');
				// Slave wasn't ready, delay, then restart transmit
				Module_Bus_Master.tbuf = Module_Bus_Master.TX_Packet;
				++Module_Bus_Master.tsize;
				SetState(&Module_Bus_Master, ibm_I2C_TXHOLD);
				Module_Bus_Master.Hold_Start_Time = GetMsTicks();
		}
		else
		{
			Module_Bus_Master.tbuf = NULL;
			Module_Bus_Master.tsize = 0;
				SetState(&Module_Bus_Master, ibm_I2C_IDLE);
			}
		}
		else if(Module_Bus_Master.Current_State == ibm_I2C_RX)
		{
			Module_Bus_Master.rbuf = NULL;
			Module_Bus_Master.rsize = 0;
			SetState(&Module_Bus_Master, ibm_I2C_IDLE);
		}
		Unlock_I2C_Bus(&Module_Bus_Master);
	}
	if(status & I2C_ISR_BERR)
	{
		// Bus Error detected (BERRCF to clear)
		PS('E');
		I2C1->ICR |= I2C_ICR_BERRCF;
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
		I2C1->ICR |= I2C_ICR_TIMOUTCF;
	}
	TPIRQL;
}

/*================================================================*/
void I2C1_ER_IRQHandler(void);
void I2C1_ER_IRQHandler(void)
{
	TPIRQH;
	uint32_t status = I2C1->ISR;
	Module_Bus_Master.Error = status & 0x0000FF00;
	PS('X');
	if(status & 3)
	{
		PS('x');
	}
	if(status & I2C_ISR_ARLO)
	{
		PS('L');
		I2C1->ICR |= I2C_ICR_ARLOCF;
	}
	if(status & I2C_ISR_BERR)
	{
		PS('8');
		I2C1->ICR |= I2C_ICR_BERRCF;
	}
	if(status & I2C_ISR_OVR)
	{
		PS('v');
		I2C1->ICR |= I2C_ICR_OVRCF;
	}
	if(status & I2C_ISR_TIMEOUT)
	{
		PS('m');
		I2C1->ICR |= I2C_ICR_TIMOUTCF;
	}
	if(status & I2C_ISR_PECERR)
	{
		PS('5');
		I2C1->ICR |= I2C_ICR_PECCF;
	}
	if(status & I2C_ISR_ALERT)
	{
		PS('6');
		I2C1->ICR |= I2C_ICR_ALERTCF;
	}
	TPIRQL;
}
/*================================================================*/
