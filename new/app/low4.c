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
void pmg_panel2_bus_scan_task(void *in);
void pmg_panel2_bus_scan_task(void *in)
{
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
