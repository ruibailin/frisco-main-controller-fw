/*
 * app2n.c
 *
 *  Created on: 09/11/2023
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
/*-------------
 *
 * 		//  Service 433 MHz Wireless Module
		/////////////////////////////////////////////
		if(_433_Key_Pressed_Flag && !IsSMutexLocked(&Sensor_Bus_Master.Mutex))
		{
			_433_Key_Pressed_Flag = 0;

			Monitor_Task(Task_433);
			_433_State_Machine();
			pmm_fob_check_change();
			Update_Task_Runtime(Task_433);
		}
 *
 * -----------------------*/
void pmg_app20_task(void *in);
void pmg_app20_task(void *in)
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
		if(!_433_Key_Pressed_Flag)
			break;
		if(IsSMutexLocked(&Sensor_Bus_Master.Mutex))
			break;
		_433_Key_Pressed_Flag = 0;
		_433_State_Machine();
		pmm_fob_check_change();
		break;
	default:
		break;
	}
}


/*--------------
 *
 * 		if(_433_Receiver.Menu_Active)
		{
			Monitor_Task(Task_433_Menu);
			_433_Menu_State_Machine();
			pmm_fob_check_change();
			Update_Task_Runtime(Task_433_Menu);
		}
 *
 * ----------------------*/
void pmg_app21_task(void *in);
void pmg_app21_task(void *in)
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
		if(!_433_Receiver.Menu_Active)
			break;
		_433_Menu_State_Machine();
		pmm_fob_check_change();
		break;
	default:
		break;
	}
}

/*-------------
 *
 * 		pmm_check_change_task();
 *
 * -----------------------*/
void pmg_app22_task(void *in);
void pmg_app22_task(void *in)
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
		pmm_check_change_task();
		break;
	default:
		break;
	}
}
/*----------------
 *
 * 		//  Temp Sensor Update
		/////////////////////////////////////////////
		if(TempSensorConvComplete_Flag || TimesUp(TempSensorUpdateTime))
		{
			Monitor_Task(Task_Temp_Sensor);
			ADC_Sensor_Tasks();
			Update_Task_Runtime(Task_Temp_Sensor);

		//		const char *tmp_unit_strings[] = {"�C", "�F", " K", "�R"};
		//		aci_print("Temperature:  %5.1f%s\r\n", Get_Temperature(), tmp_unit_strings[App_Data.Temp_Unit]);

			if(TempSensorConvComplete_Flag)
			{
				TempSensorConvComplete_Flag = 0;
			}
			else
			{
				if(Get_Sys_Temperature() == 451.0)
				{
					// Temperature hasn't been updated yet.
					TempSensorUpdateTime = GetMsTicks() + 100;
				}
				else
				{
					TempSensorUpdateTime = GetMsTicks() + 1000; //TEMP_SAMPLE_PERIOD_MS;
				}
			}
		}
 *
 *
 * --------------------*/
//  Temp Sensor Update
void pmg_app23_task(void *in);
void pmg_app23_task(void *in)
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
		ADC_Sensor_Tasks();
		float temp;
		temp=Get_Sys_Temperature();
		if(temp == 451.0)
			eos_set_timer(100);
		else
			eos_set_timer(1000);
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app24_task(void *in);
void pmg_app24_task(void *in)
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
		if(!TempSensorConvComplete_Flag)
			break;
		TempSensorConvComplete_Flag=0;
		eos_async_send(23,23,0,0x0L);		//run ADC_Sensor_Tasks();
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//  Update the Sign State
		/////////////////////////////////////////////
		if(!Demo_Mode_Active && TimesUp(SignStateTime) )
		{

			Monitor_Task(Task_SignState);
			UpdateSignState();
			Update_Task_Runtime(Task_SignState);
			SignStateTime = GetMsTicks() + SIGN_STATE_UPDATE_PERIOD_MS;
		}
 *
 * ---------------------*/
void pmg_app25_task(void *in);
void pmg_app25_task(void *in)
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
		eos_set_timer(SIGN_STATE_UPDATE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(Demo_Mode_Active)
			break;
		UpdateSignState();
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//  Demo Mode Update
		/////////////////////////////////////////////
		if(Demo_Mode_Active && TimesUp(DemoModeUpdateTime))
		{
			Monitor_Task(Task_Demo);
			Update_Demo_Mode();
			Update_Task_Runtime(Task_Demo);
			DemoModeUpdateTime = GetMsTicks() + DEMO_MODE_UPDATE_PERIOD_MS;
		}
 *
 * ---------------------*/
void pmg_app26_task(void *in);
void pmg_app26_task(void *in)
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
		eos_set_timer(DEMO_MODE_UPDATE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!Demo_Mode_Active)
			break;
		Update_Demo_Mode();
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//  Update the RTC
		/////////////////////////////////////////////
		if(TimesUp(rtcUpdateTime) && LockSMutex(&SPI2_LED2_Mutex, RTC_MUTEX_TAG))	// ymodem hangs on to mutex for duration of file download
		{
			Monitor_Task(Task_RTC);
			uint8_t alarm_check = RtcTask();
			Update_Task_Runtime(Task_RTC);
			rtcUpdateTime = GetMsTicks() + RTC_UPDATE_PERIOD_MS;



			if(alarm_check)
			{
				Check_For_RTC_Alarms();
			}

			UnlockSMutex(&SPI2_LED2_Mutex, RTC_MUTEX_TAG);
		}
 *
 * ---------------------*/
void pmg_app27_task(void *in);
void pmg_app27_task(void *in)
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
		eos_set_timer(RTC_UPDATE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!LockSMutex(&SPI2_LED2_Mutex, RTC_MUTEX_TAG))
			break;
		uint8_t alarm_check;
		alarm_check = RtcTask();
		if(alarm_check)
		{
			Check_For_RTC_Alarms();
		}
		UnlockSMutex(&SPI2_LED2_Mutex, RTC_MUTEX_TAG);
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//  Update the Display
		/////////////////////////////////////////////
		if (TimesUp(DisplayTime) && LockSMutex(&SPI2_LED2_Mutex, DISPLAY_MUTEX_TAG))	// ymodem hangs on to mutex for duration of file download
		{

			if(1)
			{
				Monitor_Task(Task_Display);
				if(Display_Update_Thread())
				{
					DisplayTime = GetMsTicks() + DISPLAY_UPDATE_PERIOD_MS;
				}
				Update_Task_Runtime(Task_Display);
				// Unlock the mutex in the SPI TX complete handler
			}
		}
 *
 * ---------------------*/
void pmg_app28_task(void *in);
void pmg_app28_task(void *in)
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
		ret=Display_Update_Thread();
		if(!ret)
			break;
		eos_set_timer(((DISPLAY_UPDATE_PERIOD_MS/10)+1)*10);
		break;
	default:
		break;
	}
}
/*-------------
 *
 * 		// Parse external commands (unless Ymodem transfer in progress)
		//  Process Ymodem Download or External AC Messages
		//    They share CMD_PORT_RX and CMD_PORT_TX
		//    Ymodem takes precedence if a download is in progress
		/////////////////////////////////////////////
		if(YmodemXferIsInProgress() && LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			Monitor_Task(Task_Ymodem);
			YmodemRun();
			Update_Task_Runtime(Task_Ymodem);
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		}
		//else if(!FC_Command_Input_Buffer_Is_Empty())
		else if(Uart3_Data_Ready() || Incoming_Messages_Pending())
		{
			// Make sure these two busses are unlocked in case we need to access a sensor.
			// We don't need to lock the module bus because those messages are buffered and
			// the handler locks the bus as needed.
			if(!IsSMutexLocked(&SPI2_LED2_Mutex))
			{
				if(!IsSMutexLocked(&Sensor_Bus_Master.Mutex))
				{
					Monitor_Task(Task_MsgParser);

					Parse_External_Command();

					Update_Task_Runtime(Task_MsgParser);
				}
			}
		}
 *
 *
 * -----------------------*/
void pmg_app29_task(void *in);
void pmg_app29_task(void *in)
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
	{
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		uint8_t ret;
		ret=YmodemXferIsInProgress();
		if(ret)
		{
			if(!LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
				break;
			YmodemRun();
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
			break;
		}
		uint8_t ret1;
		ret1=Uart3_Data_Ready();
		bool ret2;
		ret2=Incoming_Messages_Pending();
		if(!(ret1||ret2))
			break;
		if(IsSMutexLocked(&SPI2_LED2_Mutex))
			break;
		if(IsSMutexLocked(&Sensor_Bus_Master.Mutex))
			break;
		Parse_External_Command();
		break;
	}
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of app2n.c */
