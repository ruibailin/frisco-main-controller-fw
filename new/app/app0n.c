/*
 * appn.c
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */


#include "1imp.h"
/*================================================================*/
typedef enum
{
	APP0_INIT_STATE	= 0,
	APP0_ENUM_STATE,
	APP0_WORK_STATE
}App0_Machine_States;
#define APP0_WAIT_ENUM_MS		(1000*3)
#define APP0_CHECK_ENUM_MS		1000
#define APP0_NORMAL_WORK_MS		10
extern uint8_t	Firmware_Install_Active_Flag;
/*----------------
 * 	if(Firmware_Install_Active_Flag)
	{
		// 	Run Firmware Update State Machine
		/////////////////////////////////////////////
		if(TimesUp(FirmwareUpdateTime) && LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			Monitor_Task(Task_Firmware_Update);
			Process_Module_Firmware_Install_State(); // ZGy_Note
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
			FirmwareUpdateTime = pmdGetMsTicks() + FIRMWARE_UPDATE_PERIOD_MS;
			Update_Task_Runtime(Task_Firmware_Update);
		}
	}
--------------------*/
extern void Process_Module_Firmware_Install_State(void);
void pmg_app00_task(void *in);
void pmg_app00_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);

		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(FIRMWARE_UPDATE_PERIOD_MS);
		if(!Firmware_Install_Active_Flag)
			break;
		LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		Process_Module_Firmware_Install_State();
		UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		break;
	default:
		break;
	}
}

/*---------------
		// 	Handle USB Thumb Drive Host Tasks
		/////////////////////////////////////////////
		// Must run fast--1-ms update time
		// TODO:  Slow down if no thumb drive present
		if(TimesUp(USB_Thumbdrive_Host_Update_Time))
		{
			Monitor_Task(Task_USB_Thumbdrive_Host);
			MX_USB_HS_HOST_Process(); // ZGy_Note
			Update_Task_Runtime(Task_USB_Thumbdrive_Host);
			USB_Thumbdrive_Host_Update_Time = GetMsTicks() + USB_THUMBDRIVE_UPDATE_TIME_MS;
		}
 ---------------------*/
extern void MX_USB_HS_HOST_Process(void);
void pmg_app01_task(void *in);
void pmg_app01_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(USB_THUMBDRIVE_UPDATE_TIME_MS);
		if(Firmware_Install_Active_Flag)
			break;
		MX_USB_HS_HOST_Process();
		break;
	default:
		break;
	}
}

/*--------------
 * 		// 	Handle USB Net Host Tasks
		/////////////////////////////////////////////
		// Must run fast--2-ms update time
		// TODO:  Slow down if no connection active (search Networking_Turbo_Boost)
		if(USB_Net_Host_Active && TimesUp(USB_Net_Host_Update_Time))
		{
			TPEH;
			Monitor_Task(Task_USB_Net_Host);
			MX_USB_FS_HOST_Process(); // ZGy_Note
			Update_Task_Runtime(Task_USB_Net_Host);
			USB_Net_Host_Update_Time = GetMsTicks() + USB_HOST_UPDATE_TIME_MS;
			TPEL;
		}
 *
 * ----------------------*/
void pmg_app02_task(void *in);
void pmg_app02_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		if(!USB_Net_Host_Active)
			break;
		eos_set_timer(USB_HOST_UPDATE_TIME_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(USB_HOST_UPDATE_TIME_MS);
		if(Firmware_Install_Active_Flag)
			break;
		MX_USB_FS_HOST_Process();
		break;
	default:
		break;
	}
}
/*----------------
 * 		// 	Handle Ethernet Tasks
		/////////////////////////////////////////////
		//  Must run fast--1-ms update time // Run open-loop
		// TODO:  Slow down if no connection active
		if(Networking.Ethernet_Interface_Up && TimesUp(Ethernet_Update_Time))//Networking_Turbo_Boost || TimesUp(Networking_Update_Time))
		{
			Monitor_Task(Task_Ethernet);
			Ethernet_Tasks();
			Update_Task_Runtime(Task_Ethernet);
			Ethernet_Update_Time = GetMsTicks() + ETHERNET_UPDATE_TIME_MS;
		}
 *
 * --------------------*/
void pmg_app03_task(void *in);
void pmg_app03_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		if(!Networking.Ethernet_Interface_Up)
			break;
		eos_set_timer(ETHERNET_UPDATE_TIME_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(ETHERNET_UPDATE_TIME_MS);
		if(Firmware_Install_Active_Flag)
			break;

		Ethernet_Tasks();
		break;
	default:
		break;
	}
}

/*--------------
 *
 *		// 	Handle WiFi Tasks
		/////////////////////////////////////////////
		if((pass == 0) && (Networking.Radar_Up))
		{
			MX_USB_HOST_FS_Init();
			//MX_USBH_Start();
			//MX_USB_HOST_Init();
			pass = 1;
			return;
		}
		if(Networking.WIFI_Interface_Up && TimesUp(WiFi_Update_Time) && (pass == 1))
		{
			Monitor_Task(Task_WiFi);
			WIFI_Tasks();
			Update_Task_Runtime(Task_WiFi);
			WiFi_Update_Time = GetMsTicks() + WIFI_UPDATE_TIME_MS;
		}
 * ----------------------*/
void pmg_app04_task(void *in);
void pmg_app04_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		if(!Networking.Radar_Up)
			break;
		MX_USB_HOST_FS_Init();
		if(!Networking.WIFI_Interface_Up)
			break;
		eos_set_timer(WIFI_UPDATE_TIME_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(WIFI_UPDATE_TIME_MS);
		if(Firmware_Install_Active_Flag)
			break;
		WIFI_Tasks();
		break;
	default:
		break;
	}
}

/*--------------
 *
 * 		// 	Handle 4G Tasks
		/////////////////////////////////////////////
		if(Networking.LTE_Interface_Up && TimesUp(LTE_Update_Time))
		{
			Monitor_Task(Task_LTE);
			LTE_Tasks();
			Update_Task_Runtime(Task_LTE);
			LTE_Update_Time = GetMsTicks() + LTE_UPDATE_TIME_MS;
		}
 * ----------------------*/
void pmg_app05_task(void *in);
void pmg_app05_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		if(!Networking.LTE_Interface_Up)
			break;
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(LTE_UPDATE_TIME_MS);
		if(Firmware_Install_Active_Flag)
			break;
		LTE_Tasks();
		break;
	default:
		break;
	}
}
/*-------------
 *
 * 		//  Processes AC Stats Output
		//////////////////////////////////////////////
		if(Sending_AC_Stats_Flag && TimesUp(StatsUploadTaskTime))
		{
			Monitor_Task(Task_Send_AC_Stats);
			Update_AC_Stats_State();
			Update_Task_Runtime(Task_Send_AC_Stats);
			StatsUploadTaskTime = GetMsTicks() + STATS_UPLOAD_TASK_PERIOD_MS;
		}
 * -----------------------*/
#define STATS_UPLOAD_TASK_PERIOD_MS			100
void pmg_app06_task(void *in);
void pmg_app06_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(STATS_UPLOAD_TASK_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!Sending_AC_Stats_Flag)
			break;
		Update_AC_Stats_State();
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 		//  Processes Bluetooth messages
		//////////////////////////////////////////////
		if(Is_Bluetooth_Module_Installed() && TimesUp(BluetoothSampleUpdateTime) && LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			Monitor_Task(Task_Bluetooth);
			Bluetooth_Tasks();
			BluetoothSampleUpdateTime = GetMsTicks() + BLUETOOTH_SAMPLE_PERIOD_MS;
			Update_Task_Runtime(Task_Bluetooth);
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		}
 * ----------------------*/
void pmg_app07_task(void *in);
void pmg_app07_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		if(!Is_Bluetooth_Module_Installed())
			break;
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		if(!LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
			break;
		Bluetooth_Tasks();
		UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 		//  Processes Ethernet module tasks
		//////////////////////////////////////////////
		if(Ethernet_Module_Installed() && TimesUp(EthernetSampleUpdateTime))
		{
			Monitor_Task(Task_Ethernet_Module);
			Ethernet_Module_Tasks();
			EthernetSampleUpdateTime = GetMsTicks() + Ethernet_SAMPLE_PERIOD_MS;
			Update_Task_Runtime(Task_Ethernet_Module);
		}
 * ----------------------*/
void pmg_app08_task(void *in);
void pmg_app08_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		uint8_t ret;
		ret=Ethernet_Module_Installed();
		if(!ret)
			break;
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(Ethernet_SAMPLE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		Ethernet_Module_Tasks();
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app09_task(void *in);
void pmg_app09_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP0_INIT_STATE:
		eos_set_timer(APP0_WAIT_ENUM_MS);
		eos_set_state(APP0_ENUM_STATE);
		break;
	case APP0_ENUM_STATE:
		eos_set_timer(APP0_CHECK_ENUM_MS);
		eos_set_state(APP0_WORK_STATE);
		break;
	case APP0_WORK_STATE:
		eos_set_timer(APP0_NORMAL_WORK_MS*100);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of appn.c */
