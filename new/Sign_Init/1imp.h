/*
 * 1imp.h
 *
 *  Created on: Nov 10, 2023
 *      Author: stever
 */

#ifndef SIGN_INIT_1IMP_H_
#define SIGN_INIT_1IMP_H_
/*================================================================*/
#include "2exp.h"
/*------------------------------------*/
#include <stdio.h>
#define ini_print(x...)  	printf(x);
#define ini_debug(x...)  	printf(x);
#define ini_alarm(x...)   	printf(x);
#define ini_panic(x...)		printf(x);
/*------------------------------------*/
//The following come from Application/Src/Sign_Init.c
/*------------------------------------*/
#include <string.h>
#include "../Firmware_Update/Firmware_Update_Master.h"
#include "Power_Manager/Power_Manager.h"
#include "Frisco.h"
#include "Testpoints.h"
#include "Cameras/Camera.h"
#include "Radar/Radar.h"
#include "util.h"
#include "../Sensors/Light_Sensor.h"
#include "../Middlewares/ACI/Digit_Display.h"
#include "App.h"
#include "Digit_Displays/Display_I2C_Bus.h"
#include "App_Data.h"
#include "Registration_Data.h"
#include "RTC/RtcDrv.h"
#include "Graphics.h"
#include "Radar/Radar_Init.h"
#include "../Middlewares/ACI/Resources/Resources.h"
#include "Text_Messages.h"
#include "Mutex.h"
#include "../Module_Driver/Module_Hardware_Drivers/Bluetooth_Module.h"
#include "Sensors/ADC_Sensor.h"
#include "../uSD/uSD.h"
#include "Installed_Options.h"
#include "../Firmware_Update/Ymodem.h"
#include "../Firmware_Update/Check_Firmware_Update.h"
#include "../Module_Driver/Sensor_Bus.h"
#include "Debug_Config.h"
#include "../Module_Driver/Module_Hardware_Drivers/Frisco_Comm_Driver.h"
#include "Module_Hardware_Drivers/GPIO_Module.h"
#include "../UART/External_RS232.h"
#include "Data_Logger.h"
#include "../AES/Frisco_AES.h"
#include "../uSD_Update/uSD_Update.h"
//#include "../Middlewares/ACI/NTCIP/NTCIP.h"	//Stupid head file
extern void NTCIP_Init(void);
#include "Password.h"
#include "Debug_Flags.h"
#include "uSD.h"
#include "Debug_Log_Msg.h"



extern RADAR_SETUP Radar_Setup;

extern uint32_t BusEnumerationTime;       // = 0;
extern bool BusEnumerationComplete;       // = false;
extern uint8_t PanelBusEnumerationCompleteFlags;       // = 0;
//static uint32_t panel_enum_start_time;
extern uint32_t panel_enum_start_time;

#define PANEL_BUS_ENUM_START_DELAY_MS			50
#define PANEL_BUS_ENUMERATION_TIMEOUT_MS		600

extern bool Enumerate_Frisco_Comm_Chip(void);
extern bool Enumerate_Bus_Modules(void);
extern void Low_Level_App_Update(void);
extern void Init_Light_Sensor_Manager(void);
extern void Send_Startup_Notification(uint8_t target, uint8_t verbose);
extern void Send_Module_Enum_Success_Notifications(void);
extern void List_Scheduled_Operations(void);
extern void Dump_SDCard_File_Sizes(void);

typedef enum
{
	signinit_Start,
	signinit_RtcInit,
	signinit_InituSD,
	signinit_Init_Password,
	signinit_CommChipInit,
	signinit_RS232Init,
	signinit_InitSensorBus,
	signinit_InitAppData,
	signinit_InitFrisco,
	signinit_MPPTInit,
	signinit_InitFriscoCommBus,
	signinit_InitCamera,
	signinit_InitDisplay,
	signinit_InitLightSensorManager,
	signinit_InitSignState,
	signinit_TurnOnRadarPower,
	signinit_InitTempSensor,
	signinit_StartEnumerations,
	signinit_StartModuleBusEnumeration,
	signinit_ModuleBusEnum,
	signinit_StartPanelEnumeration,
	signinit_PanelBusEnum,
	signinit_WaitForPanelBusEnumComplete,
	signinit_StartCommChipEnum,
	signinit_CommChipEnum,
	signinit_StartMPPTEnumeration,
	signinit_TestForMPPTPresence,
	signinit_MPPTEnum,
	signinit_MPPT_Enumeration_Complete,
	signinit_EnumerationsComplete,
	signinit_ClearDisplays,
	signinit_InitData_Log,
	signinit_DoStartupFlagChecks,
	signinit_Module_Post_Enum_Configuration,
	signinit_Ymodem_Init,
	signinit_NTCIP_Init,
	signinit_Init_Sched_FW_Install,
	signinit_SendStartupNotification,
	signinit_InitComplete,
	signinit_PanelEnumTimedOut,
	signinit_RunControllerApp
}Sign_Init_State_t;

extern Sign_Init_State_t sinit_state;       // = signinit_Start;

#ifdef VISUALIZE_STARTUP_MUTEX_ACQUISITION
#define startmuxprintf	aci_print
#else
#define startmuxprintf noprintf
#endif

//static uint32_t last_init_time = 0;
extern uint32_t last_init_time;       // = 0;

extern void LogStartupTime(uint8_t tabs);

extern uint32_t Get_Reset_Reason(void); // ZGy_Note
extern void Set_EXTI_Flags_Val(void);   // ZGy_Note

#define MPPT_STARTUP_WAIT_INIT_ms         20000 // ZGy_Note_MPPT
#define MPPT_CHECKIF_PRESERNT_TIMEOUT_ms   3000 // ZGy_Note_MPPT
extern uint32_t MPPT_StartTime;                 // ZGy_Note_MPPT

#include "Frisco.h"
extern FRISCO Frisco;
extern void bsp_refresh_iwdg(void);
/*================================================================*/
#endif /* SIGN_INIT_1IMP_H_ */
