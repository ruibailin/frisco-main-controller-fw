/*
 * app4n.c
 *
 *  Created on: 09/11/2/23
 *      Author: Steve Rui
 */


#include "1imp.h"
/*================================================================*/
typedef enum
{
	APPN_INIT_STATE	= 0,
	APPN_ENUM_STATE,
	APPN_WORK_STATE
}App1_Machine_States;
extern uint8_t	Firmware_Install_Active_Flag;
/*---------------
 *
 * 		// Module UART Passthru
		///////////////////////////////////////////
		if(IS_YDBG_FLAG_SET(Y_Debug_Enable_Module_UART_Log) && Is_Module_Uart_Passthru_Empty())
		{
			Monitor_Task(Task_Module_UART_Passthru);
			Process_Module_Uart_Log_Passthru();
			Update_Task_Runtime(Task_Module_UART_Passthru);
		}
 *
 * ---------------------*/
void pmg_app40_task(void *in);
void pmg_app40_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(IS_YDBG_FLAG_SET(Y_Debug_Enable_Module_UART_Log) && Is_Module_Uart_Passthru_Empty())
			Process_Module_Uart_Log_Passthru();
	default:
		break;
	}
}


/*----------------
 *
 * 		//	 Run Tests
		///////////////////////////////////////////
		if(IS_XDBG_FLAG_SET(X_Debug_AC_Message_Test_Parsing))
		{
			if(Do_Test_Message_Parsing && TimesUp(MessageTestTime))
			{
				Do_Test_Message_Parsing = Test_Msg_Parser();
				MessageTestTime = GetMsTicks() + 1000;
			}
		}
 *
 * --------------------*/
void pmg_app41_task(void *in);
void pmg_app41_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(1000);
		if(Firmware_Install_Active_Flag)
			break;
		if(!IS_XDBG_FLAG_SET(X_Debug_AC_Message_Test_Parsing))
			break;
		if(!Do_Test_Message_Parsing)
			break;
		Do_Test_Message_Parsing = Test_Msg_Parser();
	default:
		break;
	}
}

/*--------------
 *
 * 		//	 Run Alert Tests
		///////////////////////////////////////////
		if(TimesUp(AlertMessageTestTime))
		{
			void Do_Alert_Message_Testing(void);
			Do_Alert_Message_Testing();
			AlertMessageTestTime = GetMsTicks() + 3600000l;
		}
 *
 *
 * ----------------------*/
extern void Do_Alert_Message_Testing(void);
void pmg_app42_task(void *in);
void pmg_app42_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(3600000);
		if(Firmware_Install_Active_Flag)
			break;
		Do_Alert_Message_Testing();
	default:
		break;
	}
}
/*--------------
 *
 * 		if(first_pass)
		{
			if(App_Data.Auto_Start_Demo_Mode)
			{
				Set_Menu_Active(true);
				Start_Demo_Mode();
			}
			first_pass = 0;
		}
 *
 * ----------------------*/
void pmg_app43_task(void *in);
void pmg_app43_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!first_pass)
			break;
		first_pass = 0;
		if(!App_Data.Auto_Start_Demo_Mode)
			break;
		Set_Menu_Active(true);
		Start_Demo_Mode();
		break;
	default:
		break;
	}
}

//The following tasks come from Low_Level_App_Update();
/*---------------
 *
 * 	//  Service Module Bus State Machine
	/////////////////////////////////////////////
	if(TimesUp(ModuleBusUpdateTime) || Module_Bus_Master.KickBusProcess)
	{
		// Make sure these two busses are unlocked in case we need to access a sensor.
		// We don't need to lock the module bus because those messages are buffered and
		// the handler locks the bus as needed.
		if(!IsSMutexLocked(&SPI2_LED2_Mutex))
		{
			if(!IsSMutexLocked(&Sensor_Bus_Master.Mutex))
			{
				Module_Bus_Master.KickBusProcess = 0;
				Module_Bus_Tasks();
				ModuleBusUpdateTime = GetMsTicks() + Module_Bus_Master.StateUpdatePeriod;
			}
		}
	}
 *
 *
 * ---------------------*/
void pmg_app44_task(void *in);
void pmg_app44_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(!(TimesUp(ModuleBusUpdateTime) || Module_Bus_Master.KickBusProcess))
			break;
		if(IsSMutexLocked(&SPI2_LED2_Mutex))
			break;
		if(IsSMutexLocked(&Sensor_Bus_Master.Mutex))
			break;
		Module_Bus_Master.KickBusProcess = 0;
		Module_Bus_Tasks();
		ModuleBusUpdateTime = GetMsTicks() + Module_Bus_Master.StateUpdatePeriod;
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 	//  Service Panel Bus State Machine
	/////////////////////////////////////////////
	if(TimesUp(PanelBusUpdateTime) || Panel_Bus_1_Master.KickBusProcess)
	{
		Panel_Bus_1_Master.KickBusProcess = 0;
		Panel_Bus_1_Tasks();
		PanelBusUpdateTime = GetMsTicks() + Panel_Bus_1_Master.StateUpdatePeriod;
	}
 *
 * ----------------------*/
void pmg_app45_task(void *in);
void pmg_app45_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(!(TimesUp(PanelBusUpdateTime) || Panel_Bus_1_Master.KickBusProcess))
			break;
		Panel_Bus_1_Master.KickBusProcess = 0;
		Panel_Bus_1_Tasks();
		PanelBusUpdateTime = GetMsTicks() + Panel_Bus_1_Master.StateUpdatePeriod;
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 	//  Service Panel Bus State Machine
	/////////////////////////////////////////////
	if(TimesUp(PanelBus2UpdateTime) || Panel_Bus_2_Master.KickBusProcess)
	{
		Panel_Bus_2_Master.KickBusProcess = 0;
		Panel_Bus_2_Tasks();
		PanelBus2UpdateTime = GetMsTicks() + Panel_Bus_2_Master.StateUpdatePeriod;
	}
 *
 * ---------------------*/
void pmg_app46_task(void *in);
void pmg_app46_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(!(TimesUp(PanelBus2UpdateTime) || Panel_Bus_2_Master.KickBusProcess))
			break;
		Panel_Bus_2_Master.KickBusProcess = 0;
		Panel_Bus_2_Tasks();
		PanelBus2UpdateTime = GetMsTicks() + Panel_Bus_2_Master.StateUpdatePeriod;
		break;
	default:
		break;
	}
}
/*-------------
 *
 * 	//  Sensor Bus State Machine
	/////////////////////////////////////////////
	if(TimesUp(SensorBusUpdateTime) || Sensor_Bus_Master.KickBusProcess)
	{
		if(!Sensor_Bus_Master.LockedUp)
		{
			Sensor_Bus_Master.KickBusProcess = 0;
			Sensor_Bus_Tasks();
			SensorBusUpdateTime = GetMsTicks() + Sensor_Bus_Master.StateUpdatePeriod;
		}
	}
 *
 * -----------------------*/
void pmg_app47_task(void *in);
void pmg_app47_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(!(TimesUp(SensorBusUpdateTime) || Sensor_Bus_Master.KickBusProcess))
			break;
		if(Sensor_Bus_Master.LockedUp)
			break;
		Sensor_Bus_Master.KickBusProcess = 0;
		Sensor_Bus_Tasks();
		SensorBusUpdateTime = GetMsTicks() + Sensor_Bus_Master.StateUpdatePeriod;
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 	//  Pet the Watchdog
	/////////////////////////////////////////////
	if(TimesUp(WDPetTime))
	{
		PetWatchdog();
		WDPetTime = GetMsTicks() + WDPETPERIOD;
	}
 *
 * ----------------------*/
void pmg_app48_task(void *in);
void pmg_app48_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(WDPETPERIOD);
		PetWatchdog();
		break;
	default:
		break;
	}
}
/*--------------
 *
 *
 * 	//  Menu Idle Timeout
	/////////////////////////////////////////////
	if(!Demo_Mode_Active && Menu.Is_Active && TimesUp(MenuIdleTimeoutTime) && !Menu_Timeout_Override)
	{
		Exit_Menu();
	}
 *
 *
 * ----------------------*/
void pmg_app49_task(void *in);
void pmg_app49_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(Demo_Mode_Active && Menu.Is_Active)
			break;
		if(!Menu.Is_Active)
			break;
		if(Menu_Timeout_Override)
			break;
		if(!TimesUp(MenuIdleTimeoutTime))
			break;
		Exit_Menu();
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of app4n.c */