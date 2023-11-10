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
void pmg_sensor_bus_scan_task(void *in);
void pmg_sensor_bus_scan_task(void *in)
{
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
