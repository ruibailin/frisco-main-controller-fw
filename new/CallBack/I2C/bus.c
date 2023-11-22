/*
 * bus.c
 *
 *  Created on: Nov 22, 2023
 *      Author: stever
 */


#include "1imp.h"
/*================================================================*/
//Called by callback function:
//uint8_t Process_Message(I2C_Bus_Master *bm, uint8_t port, uint8_t *packet)
/*------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Module_Bus.h"
#include "I2C_Bus_Master.h"

extern I2C_Bus_Master 	Module_Bus_Master;
extern I2C_Bus_Master	Sensor_Bus_Master;
extern I2C_Bus_Master 	Panel_Bus_1_Master;
extern I2C_Bus_Master 	Panel_Bus_2_Master;

int I2C_Bus_Get_Task_Id(I2C_Bus_Master *bm);
int I2C_Bus_Get_Task_Id(I2C_Bus_Master *bm)
{
	int pid;
	if(bm==&Module_Bus_Master)
		pid=44;
	if(bm==&Panel_Bus_1_Master)
		pid=45;
	if(bm==&Panel_Bus_2_Master)
		pid=46;
	if(bm==&Sensor_Bus_Master)
		pid=47;
	return pid;		//Sersor Bus
}

#include "Debug_Log_Msg.h"
void i2c_bus_send_msg(I2C_Bus_Master *bm,uint8_t port, uint8_t *packet);
void i2c_bus_send_msg(I2C_Bus_Master *bm,uint8_t port, uint8_t *packet)
{
	int dest,ll;
	ll=(int)port;			//to send port number
	ll &= 0xFF;
	dest=I2C_Bus_Get_Task_Id(bm);
	formatlog("Main_Controller","I2C","Receive A Packet!");
	printf("%s %d %d\r\n",bm->Name,port,dest);
	eos_async_send(dest,EOS_I2C_BUS_EVENT,ll,packet);
}
/*------------------------------------*/
I2C_Bus_Master *Task_Get_I2C_Bus_Master(void);
I2C_Bus_Master *Task_Get_I2C_Bus_Master()
{
	I2C_Bus_Master *bm;
	int self;
	self=eos_get_self_pid();
	if(self==44)
		bm=&Module_Bus_Master;
	if(self==45)
		bm=&Panel_Bus_1_Master;
	if(self==46)
		bm=&Panel_Bus_2_Master;
	if(self==47)
		bm=&Sensor_Bus_Master;
	return bm;
}
extern uint8_t I2C_Bus_Continue_Process_Message(I2C_Bus_Master *bm, uint8_t port, uint8_t *packet);
void i2c_bus_deal_msg(void *in);
void i2c_bus_deal_msg(void *in)
{
	int ee, ll;
	I2C_Bus_Master *bm;
	ee=eos_get_event();
	if(ee!=EOS_I2C_BUS_EVENT)
		return;
	bm=Task_Get_I2C_Bus_Master();
	ll=eos_get_length();
	formatlog("Main_Controller","I2C","Deal A Packet!");
	printf("%s %d\r\n",bm->Name,ll);
	I2C_Bus_Continue_Process_Message(bm,ll,in);
}
/*================================================================*/
/* end of event.c */
