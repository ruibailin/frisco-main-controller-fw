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
void pmg_panel1_bus_scan_task(void *in);
void pmg_panel1_bus_scan_task(void *in)
{
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
