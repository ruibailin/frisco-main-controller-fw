/*
 * app1n.c
 *
 *  Created on: 09/11/2/23
 *      Author: Steve Rui
 */


#include "1imp.h"
/*================================================================*/
typedef enum
{
	APP1_INIT_STATE	= 0,
	APP1_IDLE_STATE,
	APP1_ENUM_STATE,
	APP1_WORK_STATE =APP1_ENUM_STATE+20
}App1_Machine_States;
#define APP1_WAIT_ENUM_MS		(1000*3)
#define APP1_CHECK_ENUM_MS		1000
#define APP1_NORMAL_WORK_MS		10
extern uint8_t	Firmware_Install_Active_Flag;
/*------------
 *
 * 		//  Processes networking keep alive broadcast
		//////////////////////////////////////////////
		if(TimesUp(EthernetBroadcastUpdateTime))
		{
			Networking_Tasks();
			EthernetBroadcastUpdateTime = GetMsTicks() + 15000;
		}
 *
 ------------------------*/
#include <stdint.h>
#include "Module_Bus.h"
extern MODULE_DRIVER Module_Driver;
static int pmg_check_ethernet(void);
static int pmg_check_ethernet()
{
	if(!Module_Driver.Ports[3].Is_Module_Attached)
		return 0;
	if(Module_Driver.Ports[3].Module.Module_Type_ID==5513)
		return 1;
	return 0;
}
void pmg_app10_task(void *in);
void pmg_app10_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		eos_set_timer(APP1_WAIT_ENUM_MS);
		eos_set_state(APP1_ENUM_STATE);
		break;
	case APP1_ENUM_STATE:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		ss=pmg_check_ethernet();
		if(ss==0)
			eos_set_state(APP1_IDLE_STATE);
		else
			eos_set_state(APP1_WORK_STATE);
		break;
		break;
	case APP1_WORK_STATE:
		eos_set_timer(15000);
		if(Firmware_Install_Active_Flag)
			break;
		Networking_Tasks();
		break;
	default:
		break;
	}
}


/*--------------
 *
 * 		//  Processes 4G tasks
		//////////////////////////////////////////////
		if(Is_4G_Module_Installed() && TimesUp(LTE_4G_Update_Time))
		{
			Monitor_Task(Task_4G);
			Run_4G_Tasks();
			LTE_4G_Update_Time = GetMsTicks() + LTE_4G_SAMPLE_PERIOD_MS;
			Update_Task_Runtime(Task_4G);
		}
 * ----------------------*/
extern uint8_t Get_Module_Port(uint16_t module_type);
static int pmg_check_4g()
{
	uint8_t port;
	port=Get_Module_Port(5760);
	if(port!=0xFF)
		return 1;
	port=Get_Module_Port(5709);
	if(port!=0xFF)
		return 1;
	return 0;
}
void pmg_app11_task(void *in);
void pmg_app11_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		eos_set_timer(APP1_WAIT_ENUM_MS);
		eos_set_state(APP1_ENUM_STATE);
		break;
	case APP1_ENUM_STATE:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		ss=pmg_check_4g();
		if(ss==0)
			eos_set_state(APP1_IDLE_STATE);
		else
			eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(LTE_4G_SAMPLE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		Run_4G_Tasks();
		break;
	default:
		break;
	}
}

/*--------------
 *
 * 		//  Service Radar Sensor
		/////////////////////////////////////////////
		if(Radar_Message_Waiting && App_Data.Radar_Enable)
		{
			Monitor_Task(Task_Radar);
			Process_Radar_Messages();
			Update_Task_Runtime(Task_Radar);
		}
 * ----------------------*/
void pmg_app12_task(void *in);
void pmg_app12_task(void *in)
{
	int ss;
	int state;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+0);
		break;
	case APP1_ENUM_STATE+0:
		state=sign_init_InitFrisco(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+1);
		break;
	case APP1_ENUM_STATE+1:
		state=sign_init_InitFriscoCommBus(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+2);
		break;
	case APP1_ENUM_STATE+2:
		state=sign_init_TurnOnRadarPower(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+3);
		break;
	case APP1_ENUM_STATE+3:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(APP1_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!App_Data.Radar_Enable)
			break;
		if(!Radar_Message_Waiting)
			break;
		Process_Radar_Messages();
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 		stats_task_send_records();              //Steve Rui,send radar stats data to server
 * ----------------------*/
void pmg_app13_task(void *in);
void pmg_app13_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		eos_set_timer(APP1_WAIT_ENUM_MS);
		eos_set_state(APP1_ENUM_STATE);
		break;
	case APP1_ENUM_STATE:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_IDLE_STATE);
		break;
	case APP1_WORK_STATE:
//		eos_set_timer(APP1_NORMAL_WORK_MS*100);
		if(Firmware_Install_Active_Flag)
			break;
//		stats_task_send_records();
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//	Read MPPT Sensor Data
		/////////////////////////////////////////////
		if(mppt.Is_MPPT_Installed && TimesUp(MPPTSampleUpdateTime))
		{
			// Messages are buffered, so no need to lock bus.
				//Is that right?  Need to verify this, in the meantime, we'll lock the bus
			Monitor_Task(Task_MPPT);
			Power_Manager_Update();
			Update_Task_Runtime(Task_MPPT);
			MPPTSampleUpdateTime = GetMsTicks() + MPPT_SAMPLE_PERIOD_MS;
		}
 *
 * ---------------------*/
#define MPPT_SAMPLE_PERIOD_MS				1000
void pmg_app14_task(void *in);
void pmg_app14_task(void *in)
{
	int ss;
	int state;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+0);
		break;
	case APP1_ENUM_STATE+0:
		state=sign_init_MPPTInit(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+1);
		break;
	case APP1_ENUM_STATE+1:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		if(!mppt.Is_MPPT_Installed)
			break;
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(MPPT_SAMPLE_PERIOD_MS);
		if(Firmware_Install_Active_Flag)
			break;
		Power_Manager_Update();
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//	Process GPIO Events
		/////////////////////////////////////////////
		if((GPIO_Active_Output_Ports || GPIO_Input_Active_Events) && TimesUp(GPIO_Update_Time))
		{
			Monitor_Task(Task_GPIO);
			Do_GPIO_Update();
			Update_Task_Runtime(Task_GPIO);
		}
 *
 * ---------------------*/
extern uint8_t Get_Module_Port(uint16_t module_type);
static int pmg_check_gpio()
{
	uint8_t port;
	port=Get_Module_Port(5516);
	if(port!=0xFF)
		return 1;
	return 0;
}
void pmg_app15_task(void *in);
void pmg_app15_task(void *in)
{
	int ss;
	int state;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+0);
		break;
	case APP1_ENUM_STATE+0:
		state=sign_init_Module_Post_Enum_Configuration(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+1);
		break;
	case APP1_ENUM_STATE+1:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		ss=pmg_check_gpio();
		if(ss==0)
			eos_set_state(APP1_IDLE_STATE);
		else
			eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(APP1_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!((GPIO_Active_Output_Ports || GPIO_Input_Active_Events)))
			break;
		if(!TimesUp(GPIO_Update_Time))
		Do_GPIO_Update();
		break;
	default:
		break;
	}
}
/*--------------
 *
 * 		//	Process Sequencer Update
		/////////////////////////////////////////////
		if(TimesUp(Sequencer_Update_Time))
		{
			Monitor_Task(Task_Composite_Sequence);
			Update_Composite_Sequence();
			Update_Task_Runtime(Task_Composite_Sequence);
		}
 *
 * ----------------------*/
void pmg_app16_task(void *in);
void pmg_app16_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		eos_set_timer(APP1_WAIT_ENUM_MS);
		eos_set_state(APP1_ENUM_STATE);
		break;
	case APP1_ENUM_STATE:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(APP1_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!TimesUp(Sequencer_Update_Time))
			break;
		Update_Composite_Sequence();
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 	int sta_no_work=0;
	int sta_work=0;
		// Service Stats Log Messages
		/////////////////////////////////////////////
		if(App_Data.Radar_Enable && TimesUp(StatsUpdateTime))
		{
			StatsUpdateTime = GetMsTicks() + STATS_SAMPLE_PERIOD_MS;
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				if(LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
				{
					Monitor_Task(Task_Stats);
					Stats_Port_Tasks();
					sta_work++;
					Update_Task_Runtime(Task_Stats);
					UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
				}
				else
				{
					sta_no_work++;
				}
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
			}
			else
			{
				sta_no_work++;
				if((sta_no_work&0x1FFF)==0)
				{
					sta_no_work=0;
					formatlog("Main_Controller","Stats","Sensor Bus,locked by %lx!Stats Port work %d!\r\n",\
						Sensor_Bus_Master.Mutex.Lock_Client,sta_work);
					sta_work=0;
				}
			}
		}
 *
 * ---------------------*/
void pmg_app17_task(void *in);
void pmg_app17_task(void *in)
{
	int ss;
	int state;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+0);
		break;
	case APP1_ENUM_STATE+0:
		state=sign_init_InituSD(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+1);
		break;
	case APP1_ENUM_STATE+1:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		if(!App_Data.Radar_Enable)
			break;
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(APP1_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			break;
		if(!LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
			break;
		}
		Stats_Port_Tasks();
		UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
		break;
	default:
		break;
	}
}
/*---------------
 *
 * 		//  Service Ambient Light Sensor
		/////////////////////////////////////////////
		if( (ALS_Interrupt_Flag || TimesUp(ALSUpdateTime)) && LockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG))
		{
			 uint8_t retries = 0;
			Monitor_Task(Task_ALS);

			if(ALS_Interrupt_Flag)
			{
				if(Update_Light_Sensor_Manager() || retries)
				{
					retries = 0;
					ALS_Interrupt_Flag = 0;
//#ifdef HAMMERED_SENSOR_BUS_TESTING
//					ALSUpdateTime = GetMsTicks();
//#endif
				}
				else
				{
					// If we fail to collect the sample start, we'll leave the interrupt flag alone
					// and retry on the next pass through the mainloop
					++retries;
				}
			}
			else
			{
				if(Start_Ambient_Light_Sensor_Sample_It() || retries)
				{
#ifdef HAMMERED_SENSOR_BUS_TESTING
					ALSUpdateTime = GetMsTicks() + 1000;
#else
					ALSUpdateTime = GetMsTicks() + App_Data.Lux_Measurement_Period_ms;
#endif
					retries = 0;
				}
				else
				{
					// If we fail to trigger the sample start, we'll leave the update time alone
					// and retry on the next pass through the mainloop
					++retries;
				}
			}
			UnlockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG);
			Update_Task_Runtime(Task_ALS);
		}
 *
 *
 * ---------------------*/
//  Service Ambient Light Sensor
void pmg_app18_task(void *in);
void pmg_app18_task(void *in)
{
	int ss;
	int state;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+0);
		break;
	case APP1_ENUM_STATE+0:
		state=sign_init_InitLightSensorManager(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APP1_ENUM_STATE+1);
		break;
	case APP1_ENUM_STATE+1:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(APP1_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!ALS_Interrupt_Flag)
			break;
		bool ret1;
		ret1=Update_Light_Sensor_Manager();
		if(!ret1)
			return;
		ALS_Interrupt_Flag = 0;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
//  Service Ambient Light Sensor
void pmg_app19_task(void *in);
void pmg_app19_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP1_INIT_STATE:
		eos_set_timer(APP1_WAIT_ENUM_MS);
		eos_set_state(APP1_ENUM_STATE);
		break;
	case APP1_ENUM_STATE:
		eos_set_timer(APP1_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP1_WORK_STATE);
		break;
	case APP1_WORK_STATE:
		eos_set_timer(APP1_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		if(!LockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG))
			break;
		bool ret1;
		ret1=Start_Ambient_Light_Sensor_Sample_It();
		UnlockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG);
		if(!ret1)
			break;
		eos_set_timer(App_Data.Lux_Measurement_Period_ms);
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of app1n.c */
