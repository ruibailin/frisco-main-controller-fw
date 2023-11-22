/*
 * low1.c
 *
 *  Created on: Nov 9, 2023
 *      Author: stever
 */


#include "1imp.h"
/*================================================================*/
/*-------------
 *
 * 	// Slot 1 Interrupt Handler
	//////////////////////////////
	if(Slot_1_Int_Flag && Is_Module_Bus_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
		//{
			formatlog("Main_Controller","Interrupt","I1\r\n");
		//}
		Module_Bus_INT_Callback(eMODULE_PORT_1, NULL);
		Slot_1_Int_Flag = 0;
	}

	// Slot 2 Interrupt Handler
	//////////////////////////////
	if(Slot_2_Int_Flag && Is_Module_Bus_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
		//{
			formatlog("Main_Controller","Interrupt","I2\r\n");
		//}
		Module_Bus_INT_Callback(eMODULE_PORT_2, NULL);
		Slot_2_Int_Flag = 0;
	}

	// Slot 3 Interrupt Handler
	//////////////////////////////
	if(Slot_3_Int_Flag && Is_Module_Bus_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
		//{
			formatlog("Main_Controller","Interrupt","I3\r\n");
		//}
		Module_Bus_INT_Callback(eMODULE_PORT_3, NULL);
		Slot_3_Int_Flag = 0;
	}

	// Slot 4 Interrupt Handler
	//////////////////////////////
	if(Slot_4_Int_Flag && Is_Module_Bus_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
		//{
			formatlog("Main_Controller","Interrupt","I4\r\n");
		//}
		Module_Bus_INT_Callback(eMODULE_PORT_4, NULL);
		Slot_4_Int_Flag = 0;
	}

	// Slot 5 Interrupt Handler
	//////////////////////////////
	if(Slot_5_Int_Flag && Is_Module_Bus_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
	//	{
			formatlog("Main_Controller","Interrupt","I5\r\n");
	//	}
		Module_Bus_INT_Callback(eMODULE_PORT_5, NULL);
		Slot_5_Int_Flag = 0;
	}

	// Slot 6 Interrupt Handler
	//////////////////////////////
	if(Slot_6_Int_Flag && Is_Module_Bus_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
	//	{
			formatlog("Main_Controller","Interrupt","I6\r\n");
	//	}
		Module_Bus_INT_Callback(eMODULE_PORT_6, NULL);
		Slot_6_Int_Flag = 0;
	}

	// Slot 7 Interrupt Handler
	//////////////////////////////
	if(Slot_7_Int_Flag && Is_Module_Bus_Ready())
	{
	//	if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
	//	{
			formatlog("Main_Controller","Interrupt","I7\r\n");
	//	}
		Module_Bus_INT_Callback(eMODULE_PORT_7, NULL);
		Slot_7_Int_Flag = 0;
	}
 *
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
	if(Module_Bus_Master.RxData_Callback==0x0L)
		return;
	if(!(TimesUp(ModuleBusUpdateTime) || Module_Bus_Master.KickBusProcess))
		return;
	if(IsSMutexLocked(&SPI2_LED2_Mutex))
		return;
	if(IsSMutexLocked(&Sensor_Bus_Master.Mutex))
		return;
	Module_Bus_Master.KickBusProcess = 0;
	Module_Bus_Tasks();
	ModuleBusUpdateTime = GetMsTicks() + Module_Bus_Master.StateUpdatePeriod;
}
void pmg_module_bus_scan_task(void *in);
void pmg_module_bus_scan_task(void *in)
{
	scan_i2c_msg();
	bool ret;
	ret=Is_Module_Bus_Ready();
	if(!ret)
		return;
	if(Slot_1_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_1, NULL);
		Slot_1_Int_Flag = 0;
	}
	if(Slot_2_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_2, NULL);
		Slot_2_Int_Flag = 0;
	}
	if(Slot_3_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_3, NULL);
		Slot_3_Int_Flag = 0;
	}
	if(Slot_4_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_4, NULL);
		Slot_4_Int_Flag = 0;
	}
	if(Slot_5_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_5, NULL);
		Slot_5_Int_Flag = 0;
	}
	if(Slot_6_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_6, NULL);
		Slot_6_Int_Flag = 0;
	}
	if(Slot_7_Int_Flag)
	{
		Module_Bus_INT_Callback(eMODULE_PORT_7, NULL);
		Slot_7_Int_Flag = 0;
	}


}
/*================================================================*/
/* end of low1.c */
