/*
 * low2.c
 *
 *  Created on: Nov 9, 2023
 *      Author: stever
 */


#include "1imp.h"
/*================================================================*/

/*------------
 *
 * 	// MPPT Interrupt Handler
	//////////////////////////////
	if(MPPT_Int_Flag && Is_Sensor_Bus_Ready())
	{
		if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
		{
			formatlog("Main_Controller","Interrupt","IMPPT\r\n");
		}
		Sensor_Bus_INT_Callback();
		MPPT_Int_Flag = 0;
	}
 *
 * ------------------------*/
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
	if(Sensor_Bus_Master.RxData_Callback==0x0L)
		return;
	if(!(TimesUp(SensorBusUpdateTime) || Sensor_Bus_Master.KickBusProcess))
		return;
	if(Sensor_Bus_Master.LockedUp)
		return;
	Sensor_Bus_Master.KickBusProcess = 0;
	Sensor_Bus_Tasks();
	SensorBusUpdateTime = GetMsTicks() + Sensor_Bus_Master.StateUpdatePeriod;
}
void pmg_sensor_bus_scan_task(void *in);
void pmg_sensor_bus_scan_task(void *in)
{
	scan_i2c_msg();
	bool ret;
	ret=Is_Sensor_Bus_Ready();
	if(!ret)
		return;
	if(MPPT_Int_Flag)
	{
		Sensor_Bus_INT_Callback();
		MPPT_Int_Flag = 0;
	}
}
/*================================================================*/
/* end of low2.c */
