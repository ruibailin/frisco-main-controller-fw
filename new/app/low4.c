/*
 * low4.c
 *
 *  Created on: Nov 9, 2023
 *      Author: stever
 */


#include "1imp.h"
/*================================================================*/

/*-------------
 *
 *
 * 	// LED 2 Interrupt Handler
	//////////////////////////////
	if(LED_2_Int_Flag && Is_Panel_Bus_2_Ready())
	{
	//	if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
	//	{
			formatlog("Main_Controller","Interrupt","ILED2\r\n");
	//	}
		Panel_Bus_2_INT_Callback();
		LED_2_Int_Flag = 0;
	}
 *
 * -----------------------*/
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
	if(Panel_Bus_2_Master.RxData_Callback==0x0L)
		return;
	if(!(TimesUp(PanelBus2UpdateTime) || Panel_Bus_2_Master.KickBusProcess))
		return;
	Panel_Bus_2_Master.KickBusProcess = 0;
	Panel_Bus_2_Tasks();
	PanelBus2UpdateTime = GetMsTicks() + Panel_Bus_2_Master.StateUpdatePeriod;
	return;
}

void pmg_panel2_bus_scan_task(void *in);
void pmg_panel2_bus_scan_task(void *in)
{
	scan_i2c_msg();
	bool ret;
	ret=Is_Panel_Bus_2_Ready();
	if(!ret)
		return;
	if(!LED_2_Int_Flag)
		return;
	Panel_Bus_2_INT_Callback();
	LED_2_Int_Flag = 0;
}
/*================================================================*/
/* end of low4.c */
