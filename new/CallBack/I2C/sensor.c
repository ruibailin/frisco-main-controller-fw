/*
 * sensor.c
 *
 *  Created on: Nov 21, 2023
 *      Author: stever
 */


#include "1imp.h"
/*================================================================*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Module_Bus.h"
#include "I2C_Bus_Master.h"
/*------------------------------------*/
static uint8_t  Sensor_Bus_Data_Rx(uint8_t port, I2C_Packet_Header_Type *hdr, uint8_t *pkt);
static uint8_t  Sensor_Bus_Data_Rx(uint8_t port, I2C_Packet_Header_Type *hdr, uint8_t *pkt)
{
	int self,ll;
	self=47;
	ll=(int)port;			//to send port number
	ll &= 0xFF;
	eos_async_send_extra(self,EOS_I2C_BUS_EVENT,ll,hdr,pkt);
	return 1;
}


extern uint8_t Sensor_Bus_Data_Callback(uint8_t port, I2C_Packet_Header_Type *hdr, uint8_t *pkt);
void Sensor_Bus_Data_Tx(void *in);
void Sensor_Bus_Data_Tx(void *in)
{
	void *pkt;
	int ll,ee;
	ee=eos_get_event();
	if(ee!=EOS_I2C_BUS_EVENT)
		return;
	ll=eos_get_length();
	pkt=eos_get_extra_data();
	Sensor_Bus_Data_Callback((uint8_t)ll,in,pkt);
}

/*------------------------------------*/
//It's better to replace the following function:
//uint8_t Process_Message(I2C_Bus_Master *bm, uint8_t port, uint8_t *packet)
/*------------------------------------*/
extern I2C_Bus_Master	Sensor_Bus_Master;

void Sensor_Bus_Init_Rx_CallBack(void);
void Sensor_Bus_Init_Rx_CallBack()
{
	Sensor_Bus_Master.RxData_Callback = Sensor_Bus_Data_Rx;
}
/*================================================================*/
/* end of sensor.c */
