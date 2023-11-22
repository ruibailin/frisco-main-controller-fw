/*
 * low3.c
 *
 *  Created on: Nov 9, 2023
 *      Author: stever
 */


#include "1imp.h"
/*================================================================*/

/*--------------
 *
 * 	// LED 1 Interrupt Handler
	//////////////////////////////
	if(LED_1_Int_Flag && Is_Panel_Bus_1_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
	//	{
			formatlog("Main_Controller","Interrupt","ILED1\r\n");
	//	}
		Panel_Bus_1_INT_Callback();
		LED_1_Int_Flag = 0;
	}
 *
 * ----------------------*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Module_Bus.h"
#include "I2C_Bus_Master.h"
extern I2C_Bus_Master 	Module_Bus_Master;
extern I2C_Bus_Master	Sensor_Bus_Master;
extern I2C_Bus_Master 	Panel_Bus_1_Master;
extern I2C_Bus_Master 	Panel_Bus_2_Master;
static void scan_i2c_msg(void);
static void scan_i2c_msg()
{
	if(Panel_Bus_1_Master.RxData_Callback==0x0L)
		return;
	if(!(TimesUp(PanelBusUpdateTime) || Panel_Bus_1_Master.KickBusProcess))
		return;
	Panel_Bus_1_Master.KickBusProcess = 0;
	Panel_Bus_1_Tasks();
	PanelBusUpdateTime = GetMsTicks() + Panel_Bus_1_Master.StateUpdatePeriod;
	return;
}
void pmg_panel1_bus_scan_task(void *in);
void pmg_panel1_bus_scan_task(void *in)
{
	scan_i2c_msg();
	bool ret;
	ret=Is_Panel_Bus_1_Ready();
	if(!ret)
		return;
	if(!LED_1_Int_Flag)
		return;
	Panel_Bus_1_INT_Callback();
	LED_1_Int_Flag = 0;


}

/*================================================================*/
/* end of low3.c */
