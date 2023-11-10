/*
 * app3n.c
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
/*--------------
 *
 * 		// 	Firmware Update
		/////////////////////////////////////////////
		if(Firmware_Update_Flag && TimesUp(FirmwareUpdateTime) && LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			Monitor_Task(Task_Prep_For_Firmware_Update);
			Firmware_Update_State();
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
			FirmwareUpdateTime = pmdGetMsTicks() + FIRMWARE_UPDATE_PERIOD_MS;
			Update_Task_Runtime(Task_Prep_For_Firmware_Update);
		}
 *
 * ----------------------*/
void pmg_app30_task(void *in);
void pmg_app30_task(void *in)
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
		eos_set_timer(FIRMWARE_UPDATE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!Firmware_Update_Flag)
			break;
		if(!LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
			break;
		Firmware_Update_State();
		UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
	default:
		break;
	}
}


/*---------------
 *
 * 		// 	Log Data to Non-Volatile Memory
		/////////////////////////////////////////////
		if((App_Data.Capture_Log_Data && (Data_Log_Update_Flag || TimesUp(DataLoggerUpdateTime))) && LockSMutex(&Sensor_Bus_Master.Mutex, DATA_LOG_MUTEX_TAG))
		{
			Monitor_Task(Task_DataLogger);
			Data_Log_Update_Flag = Update_Data_Logger();
			UnlockSMutex(&Sensor_Bus_Master.Mutex, DATA_LOG_MUTEX_TAG);
			Update_Task_Runtime(Task_DataLogger);
#ifdef HAMMERED_SENSOR_BUS_TESTING
			DataLoggerUpdateTime = GetMsTicks() + 20000; //App_Data.Data_Logger_Period_min * 1000;
#else
			DataLoggerUpdateTime = GetMsTicks() + App_Data.Data_Logger_Period_min * 60000;
#endif
		}
 *
 *
 * -----------*/
void pmg_app31_task(void *in);
void pmg_app31_task(void *in)
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
		if(!App_Data.Capture_Log_Data)
			break;
		if(!LockSMutex(&Sensor_Bus_Master.Mutex, DATA_LOG_MUTEX_TAG))
			break;
		Data_Log_Update_Flag = Update_Data_Logger();
		UnlockSMutex(&Sensor_Bus_Master.Mutex, DATA_LOG_MUTEX_TAG);

		if(Data_Log_Update_Flag)
			break;
		eos_set_timer(App_Data.Data_Logger_Period_min * 60000);
		break;
	default:
		break;
	}
}

/*---------------
 *
 * 		//	 Send AC Messages
		///////////////////////////////////////////
		if(Serial_Output_Buffer_Msg_Pending())
		{
//			aci_print("<S>");
			Monitor_Task(Task_SendMsg);
			Process_Outgoing_Serial_Messages();
			Update_Task_Runtime(Task_SendMsg);
		}
 *
 * ---------------------*/
void pmg_app32_task(void *in);
void pmg_app32_task(void *in)
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
		uint8_t ret;
		ret=Serial_Output_Buffer_Msg_Pending();
		if(!ret)
			break;
		Process_Outgoing_Serial_Messages();
	default:
		break;
	}
}
/*--------------
 *
 *  	//	 Send MPPT Data
		///////////////////////////////////////////
		if(MPPT_Data_TX_Period && TimesUp(MPPT_Update_Time))
		{
			// Messages are buffered, so no need to lock bus.
			Monitor_Task(Task_MPPT_Data_Send);
			Process_Power_Manager_Data_Messages();
			Update_Task_Runtime(Task_MPPT_Data_Send);

			if(Rapid_MPPT_Test_Divider)
			{
				MPPT_Update_Time = GetMsTicks() + MPPT_Data_TX_Period * 1000 / Rapid_MPPT_Test_Divider;
			}
			else
			{
				MPPT_Update_Time = GetMsTicks() + MPPT_Data_TX_Period * 1000;
			}
		}
 *
 * ----------------------*/
void pmg_app33_task(void *in);
void pmg_app33_task(void *in)
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
		if(!MPPT_Data_TX_Period)
			break;
		Process_Power_Manager_Data_Messages();
		if(Rapid_MPPT_Test_Divider)
		{
			eos_set_timer(MPPT_Data_TX_Period * 1000 / Rapid_MPPT_Test_Divider);
		}
		else
		{
			eos_set_timer(MPPT_Data_TX_Period * 1000);
		}
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//	 Multi-Packet Processing
		///////////////////////////////////////////
		if(Multi_Packet_Incoming_Msg_In_Flight && TimesUp(Multi_Packet_Timeout_Time))
		{
			Monitor_Task(Task_Multi_Packet_Timeout);
			Multi_Packet_Timeout();
			Update_Task_Runtime(Task_Multi_Packet_Timeout);
		}
 *
 * ---------------------*/
void pmg_app34_task(void *in);
void pmg_app34_task(void *in)
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
		if(!Multi_Packet_Incoming_Msg_In_Flight)
			break;
		if(!TimesUp(Multi_Packet_Timeout_Time))
			break;
		Multi_Packet_Timeout();
	default:
		break;
	}
}
/*----------------
 *
 * 		//	 AC Reset Msg Handler
		///////////////////////////////////////////
		if(Menu_Map_Update_Reset_Handler)
		{
			// Delay until ACK message has been sent.
			if(All_Messages_Sent())
			{
				Reset_Sign();
			}
		}
 *
 *
 * --------------------*/
void pmg_app35_task(void *in);
void pmg_app35_task(void *in)
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
		eos_set_timer(APPN_NORMAL_WORK_MS*100);
		if(Firmware_Install_Active_Flag)
			break;
		if(!Menu_Map_Update_Reset_Handler)
			break;
		bool ret;
		ret=All_Messages_Sent();
		if(!ret)
			break;
		Reset_Sign();
	default:
		break;
	}
}
/*--------------
 *
 * 		//	 Maintain the External RS232 port
		///////////////////////////////////////////
		if(TimesUp(External_RS232_Update_Time))
		{
			Monitor_Task(Task_External_RS232);
			External_RS232_Tasks();
			Update_Task_Runtime(Task_External_RS232);
			External_RS232_Update_Time = GetMsTicks() + EXTERNAL_RS232_UPDATE_PERIOD_MS;
		}
 *
 * ----------------------*/
void pmg_app36_task(void *in);
void pmg_app36_task(void *in)
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
		eos_set_timer(EXTERNAL_RS232_UPDATE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		External_RS232_Tasks();
	default:
		break;
	}
}
/*---------------
 *
 * 		//	 Update Module RTCs
		///////////////////////////////////////////
		if(TimesUp(Module_RTC_Update_Time))
		{
			Monitor_Task(Task_Module_RTC_Update);
			Send_RTC_Update_To_All_Modules();
			Update_Task_Runtime(Task_Module_RTC_Update);
			Module_RTC_Update_Time = GetMsTicks() + MODULE_RTC_UPDATE_PERIOD_MS;
		}
 *
 * ---------------------*/
void pmg_app37_task(void *in);
void pmg_app37_task(void *in)
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
		eos_set_timer(MODULE_RTC_UPDATE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		Send_RTC_Update_To_All_Modules();
	default:
		break;
	}
}
/*--------------
 *
 * 		//	 Run I2C Watchdogs
		///////////////////////////////////////////
		if((Trigger_I2C_Bus_Function_Test || TimesUp(I2C_Bus_Watchdog_Time)))
		{
			TP_H;
			Monitor_Task(Task_I2C_Bus_Watchdog);

//			 uint8_t bus_trigger_flags = 0;

			//TODO:  Need to do a round-robin over the busses even if another bus is locked up.
//			 uint8_t bus_flag = 1;

			if(!Trigger_I2C_Bus_Function_Test && TimesUp(I2C_Bus_Watchdog_Time))
			{
				// If we got here because it is I2C_Bus_Watchdog_Time
				// then we want to visit each of the watchdogs once (unless they're locked up).
				Trigger_I2C_Bus_Function_Test = 0x0F;
				I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_TIME_MS;
			}
//			else
//			{
//				bus_trigger_flags |= Trigger_I2C_Bus_Function_Test;
//			}

//			Trigger_I2C_Bus_Function_Test = 0;

//			while(bus_trigger_flags && !(bus_flag & bus_trigger_flags) && bus_flag < IBM_MAX_BUS_ID)
//			{
//				bus_flag >> 1;
//			}
//
//			switch(bus_flag)

			if((Trigger_I2C_Bus_Function_Test & IBM_MODULE_BUS_ID) && TestLockSMutex(&Module_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
			{
				//////    Module Bus
				if(I2C_Bus_Function_Test(&Module_Bus_Master))
				{
					TestUnlockSMutex(&Module_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
					Trigger_I2C_Bus_Function_Test &= ~IBM_MODULE_BUS_ID;
				}
				else
				{
					// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
					I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
				}
			}
			else if((Trigger_I2C_Bus_Function_Test & IBM_SENSOR_BUS_ID) && TestLockSMutex(&Sensor_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
			{
				//////    Sensor Bus
				if(I2C_Bus_Function_Test(&Sensor_Bus_Master))
				{
					TestUnlockSMutex(&Sensor_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
					Trigger_I2C_Bus_Function_Test &= ~IBM_SENSOR_BUS_ID;
				}
				else
				{
					// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
					I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
				}
			}
			else if((Trigger_I2C_Bus_Function_Test & IBM_PANEL_BUS_2_ID) && TestLockSMutex(&Panel_Bus_2_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
			{
				//////    Panel Bus 2
				if(I2C_Bus_Function_Test(&Panel_Bus_2_Master))
				{
					TestUnlockSMutex(&Panel_Bus_2_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
					Trigger_I2C_Bus_Function_Test &= ~IBM_PANEL_BUS_2_ID;
				}
				else
				{
					// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
					I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
				}
			}
			else if((Trigger_I2C_Bus_Function_Test & IBM_PANEL_BUS_1_ID) && TestLockSMutex(&Panel_Bus_1_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
			{
				//////    Panel Bus 1
				if(I2C_Bus_Function_Test(&Panel_Bus_1_Master))
				{
					TestUnlockSMutex(&Panel_Bus_1_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
					Trigger_I2C_Bus_Function_Test &= ~IBM_PANEL_BUS_1_ID;
				}
				else
				{
					// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
					I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
				}
			}

			Update_Task_Runtime(Task_I2C_Bus_Watchdog);
			TP_L;
		}


 *
 *
 * ----------------------*/
static void check_i2c_bus()
{
	if(Trigger_I2C_Bus_Function_Test)
		return;
	if(!TimesUp(I2C_Bus_Watchdog_Time))
		return;
	// If we got here because it is I2C_Bus_Watchdog_Time
	// then we want to visit each of the watchdogs once (unless they're locked up).
	Trigger_I2C_Bus_Function_Test = 0x0F;
	I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_TIME_MS;
}
static void check_module_bus()
{
	if(!(Trigger_I2C_Bus_Function_Test & IBM_MODULE_BUS_ID))
		return;
	if(!TestLockSMutex(&Module_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
		return;
	//////    Module Bus

	uint8_t ret;
	ret=I2C_Bus_Function_Test(&Module_Bus_Master);
	if(ret)
	{
		TestUnlockSMutex(&Module_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
		Trigger_I2C_Bus_Function_Test &= ~IBM_MODULE_BUS_ID;
	}
	else
	{
		I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
	}
}

static void check_sensor_bus()
{
	if(!(Trigger_I2C_Bus_Function_Test & IBM_SENSOR_BUS_ID))
		return;
	if(!TestLockSMutex(&Sensor_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
		return;
	uint8_t ret;
	ret= I2C_Bus_Function_Test(&Sensor_Bus_Master);
	if(ret)
	{
		TestUnlockSMutex(&Sensor_Bus_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
		Trigger_I2C_Bus_Function_Test &= ~IBM_SENSOR_BUS_ID;
	}
	else
	{
		// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
		I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
	}
}

static void check_panel2_bus()
{
	if(!(Trigger_I2C_Bus_Function_Test & IBM_PANEL_BUS_2_ID))
		return;
	if(!TestLockSMutex(&Panel_Bus_2_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
		return;
	uint8_t ret;
	ret= I2C_Bus_Function_Test(&Panel_Bus_2_Master);
	if(ret)
	{
		TestUnlockSMutex(&Panel_Bus_2_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
		Trigger_I2C_Bus_Function_Test &= ~IBM_PANEL_BUS_2_ID;
	}
	else
	{
		// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
		I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
	}
}

static void check_panel1_bus()
{
	if(!(Trigger_I2C_Bus_Function_Test & IBM_PANEL_BUS_1_ID))
		return;
	if(!TestLockSMutex(&Panel_Bus_1_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG))
		return;
	uint8_t ret;
	ret= I2C_Bus_Function_Test(&Panel_Bus_1_Master);
	if(ret)
	{
		TestUnlockSMutex(&Panel_Bus_1_Master.Mutex, I2C_WATCHDOG_MUTEX_TAG);
		Trigger_I2C_Bus_Function_Test &= ~IBM_PANEL_BUS_1_ID;
	}
	else
	{
		// Bus locked up, so leave the mutex locked and keep hammering until it the bus is freed!
		I2C_Bus_Watchdog_Time = GetMsTicks() + I2C_BUS_WATCHDOG_LOCKED_UP_RETRY_TIME_MS;
	}
}
void pmg_app38_task(void *in);
void pmg_app38_task(void *in)
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
		if(!(Trigger_I2C_Bus_Function_Test || TimesUp(I2C_Bus_Watchdog_Time)))
			break;
		check_i2c_bus();
		check_module_bus();
		check_sensor_bus();
		check_panel2_bus();
		check_panel1_bus();
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 		//	 Power Control Task
		///////////////////////////////////////////
		if(TimesUp(PowerControl_Update_Time))
		{
			Monitor_Task(Task_Power_Control);
			PowerControlTask();
			Update_Task_Runtime(Task_Power_Control);
			PowerControl_Update_Time = GetMsTicks() + POWER_CONTROL_PERIOD_MS;
		}
 *
 * ----------------------*/
void pmg_app39_task(void *in);
void pmg_app39_task(void *in)
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
		eos_set_timer(POWER_CONTROL_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		PowerControlTask();
	default:
		break;
	}
}
/*================================================================*/
/* end of app3n.c */
