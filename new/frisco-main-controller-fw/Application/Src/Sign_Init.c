/*
 * Sign_Init.c
 *
 *  Created on: Nov 13, 2017
 *      Author: mma
 */
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
#include "../Middlewares/ACI/NTCIP/NTCIP.h"
#include "Password.h"
#include "Debug_Flags.h"
#include "uSD.h"
#include "Debug_Log_Msg.h"



extern RADAR_SETUP Radar_Setup;
//extern const unsigned char Angry_Face_15[137];
//extern const unsigned char Test_15[137];
//extern const unsigned char Test_2_15[137];
//extern const unsigned char Test_3_15[137];
//extern const unsigned char Test_4_15[137];

uint32_t BusEnumerationTime = 0;
bool BusEnumerationComplete = false;
uint8_t PanelBusEnumerationCompleteFlags = 0;
//static uint32_t panel_enum_start_time;
uint32_t panel_enum_start_time;

#define PANEL_BUS_ENUM_START_DELAY_MS			50
#define PANEL_BUS_ENUMERATION_TIMEOUT_MS		600

bool Enumerate_Frisco_Comm_Chip(void);
bool Enumerate_Bus_Modules(void);
void Low_Level_App_Update(void);
void Init_Light_Sensor_Manager(void);
void Send_Startup_Notification(uint8_t target, uint8_t verbose);
void Send_Module_Enum_Success_Notifications(void);
void List_Scheduled_Operations(void);
void Dump_SDCard_File_Sizes(void);

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

Sign_Init_State_t sinit_state = signinit_Start;

#ifdef VISUALIZE_STARTUP_MUTEX_ACQUISITION
#define startmuxprintf	aci_print
#else
#define startmuxprintf noprintf
#endif

//static uint32_t last_init_time = 0;
uint32_t last_init_time = 0;
#include "FW_Version.h"
void LogStartupTime(uint8_t tabs)
{
	if(IS_YDBG_FLAG_SET(Y_Debug_Log_Startup_Time))
	{
		while(tabs--) //aci_print("\t"); //FCPutc('\t');
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","%8ld\r\n", GetMsTicks() - last_init_time);//VR NOTE: Come back later
#else
		formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"%8ld\r\n", GetMsTicks() - last_init_time);//VR NOTE: Come back later
#endif
		last_init_time = GetMsTicks();
	}
	else
	{
		//aci_print("\r\n");
	}
}

extern uint32_t Get_Reset_Reason(void); // ZGy_Note
extern void Set_EXTI_Flags_Val(void);   // ZGy_Note

#define MPPT_STARTUP_WAIT_INIT_ms         20000 // ZGy_Note_MPPT
#define MPPT_CHECKIF_PRESERNT_TIMEOUT_ms   3000 // ZGy_Note_MPPT
//static uint32_t MPPT_StartTime;                 // ZGy_Note_MPPT
uint32_t MPPT_StartTime;                 // ZGy_Note_MPPT

#include "Frisco.h"
extern FRISCO Frisco;
extern void bsp_refresh_iwdg(void);
void InitApplication(void)
{
	uint32_t moduleInitTimestamp = 0;

	do
	{
		bsp_refresh_iwdg();				//Fixup endless reboot by damage of uSD Card. Stave Rui
		switch(sinit_state)
		{
		case signinit_Start:
			last_init_time = GetMsTicks();
			Frisco.Modules = Init_Module_Driver();
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Init Application Start.\r\n" );
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Init Application Start.\r\n" );
#endif
			// Clear task runtimes array
			memset(Task_Runtimes, 0, sizeof(Task_Runtimes));
			++sinit_state;
			break;
		case signinit_Init_Password:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				// Init the password.
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Initialization","Initializing the Password.\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing the Password.\r\n");
#endif
				Init_Password();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(7);
				++sinit_state;
			}
			else
			{
				HAL_Delay(10);
			}
			break;
		case signinit_CommChipInit:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Frisco COMM.\r\n" );
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Frisco COMM.\r\n" );
#endif

			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_RS232Init:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing External RS232 Port.\r\n" );
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing External RS232 Port.\r\n" );
#endif
			Init_External_RS232();
			LogStartupTime(6);
			++sinit_state;
			break;
		case signinit_InitSensorBus:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Sensor Bus\r\n" );
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Sensor Bus\r\n" );
#endif
			Init_Sensor_Bus();
			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_InitAppData:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Initialization","Initializing App Data\r\n" );
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing App Data\r\n" );
#endif
				Init_App_Data();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(10);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_InituSD:
			// NOTE: uSD must be checked and mounted before 'Init_Frisco()' and 'Init_Camera()' as they will init different if uSD is present
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing uSD\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing uSD\r\n");
#endif
			Init_uSD();
			Init_uSD_Updater();
			LogStartupTime(10);
			++sinit_state;
			break;
		case signinit_InitFrisco:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Frisco\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Frisco\r\n");
#endif
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				Init_Frisco();
				Frisco_AES_Init();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_MPPTInit:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Initialization","Initializing MPPT\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing MPPT\r\n");
#endif
				Init_Internal_Charger();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(8);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_InitFriscoCommBus:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Frisco Comm Bus\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Frisco Comm Bus\r\n");
#endif
			Init_Frisco_Comm_Bus();
			LogStartupTime(6);
			++sinit_state;
			break;
		case signinit_InitCamera:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Camera\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Camera\r\n");
#endif
			Init_Camera();
			LogStartupTime(10);
			++sinit_state;
			break;
		case signinit_InitDisplay:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Display\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Display\r\n");
#endif
			Init_Display();
			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_InitLightSensorManager:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Initialization","Initializing Ambient Light Sensor\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Ambient Light Sensor\r\n");
#endif
				Init_Light_Sensor_Manager();
				++sinit_state;
				UnlockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG);
				LogStartupTime(6);
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_InitSignState:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Sign State\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Sign State\r\n");
#endif
			InitSignState();
			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_TurnOnRadarPower:
			if(App_Data.Radar_Enable)
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Initialization","Powering Up Radar\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Powering Up Radar\r\n");
#endif
				Turn_On_Radar_Power();
				LogStartupTime(8);
			}
			++sinit_state;
			break;
		case signinit_RtcInit:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing RTC\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing RTC\r\n");
#endif
			RtcInit();
			LogStartupTime(8);
			++sinit_state;
			break;
		case signinit_InitTempSensor:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Initialization","Initializing Temp Sensor\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Temp Sensor\r\n");
#endif
			Init_Temp_Sensor();
			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_Ymodem_Init:
			//startmuxprintf("}");
			if(LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Initialization","Initializing Ymodem Download Capabilities\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Initialization,"Initializing Ymodem Download Capabilities\r\n");
#endif
				YmodemInit();
				UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
				LogStartupTime(5);
				++sinit_state;
			}
			else
			{
				HAL_Delay(10);
			}
			break;
		case signinit_StartEnumerations:
			sinit_state = signinit_StartModuleBusEnumeration;
			break;
		case signinit_StartModuleBusEnumeration:
		{
			uint32_t Reset_Reason = Get_Reset_Reason();
			//if(Reset_Reason & RCC_CSR_PORRSTF) // When software reset do not show message "Starting PMG" // ZGy_Note
			//{
				if(!Is_uSD_Mounted())
				{
					Show_Text_Screen("Error:", "SD Card", 2);
				}
				else
				{
					Set_Display_Flashing(true); // ZGy_Note
					Show_Text_Screen("Starting", "PMG", 2);
				}
			//}
			moduleInitTimestamp = GetMsTicks();
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Enumeration","Enumerate Module Bus\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Enumeration,"Enumerate Module Bus\r\n");
#endif
			sinit_state = signinit_ModuleBusEnum;
			break;
		}
		case signinit_ModuleBusEnum:
			if(Enumerate_Bus_Modules())
			{
				// Module Bus Enumeration Complete
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Enumeration","Completed Module Bus Enumeration\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Enumeration,"Completed Module Bus Enumeration\r\n");
#endif
				LogStartupTime(6);
				sinit_state = signinit_StartPanelEnumeration;
			}
			else
			{
//				if(TimesUp(moduleInitTimestamp) && LockSMutex(&SPI2_LED2_Mutex, DISPLAY_MUTEX_TAG))
				if(!TimesUp(moduleInitTimestamp))
					break;
				if(LockSMutex(&SPI2_LED2_Mutex, DISPLAY_MUTEX_TAG))
				{
					Low_Level_App_Update();
					if(Display_Update_Thread())
					{
						moduleInitTimestamp = GetMsTicks() + DISPLAY_UPDATE_PERIOD_MS;
					}
					// Unlock the mutex in the SPI TX complete handler
				}
			}
			break;
		case signinit_StartPanelEnumeration:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Enumeration","Enumerate Display Panel Bus\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Enumeration,"Enumerate Display Panel Bus\r\n");
#endif
			// This timeout shouldn't be necessary--something must be delaying display startup.
			HAL_Delay(PANEL_BUS_ENUM_START_DELAY_MS);
			moduleInitTimestamp = GetMsTicks();
			panel_enum_start_time = pmdGetMsTicks();
			sinit_state = signinit_PanelBusEnum;
			break;
		case signinit_PanelBusEnum:
			if(Enumerate_Display_Panels())
			{
				sinit_state = signinit_WaitForPanelBusEnumComplete;
			}
			else if(TimedOut(panel_enum_start_time, PANEL_BUS_ENUMERATION_TIMEOUT_MS))
			{
				sinit_state = signinit_PanelEnumTimedOut;
			}
			break;
		case signinit_WaitForPanelBusEnumComplete:
			if(PanelBusEnumerationCompleteFlags == 0x03)
			{
				Send_Panel_Enum_Complete_Message();
				LogStartupTime(10);
				sinit_state = signinit_StartCommChipEnum;
			}
			break;
		case signinit_PanelEnumTimedOut:
			Show_Text_Screen("Panel", "Timeout", 2);
			moduleInitTimestamp = GetMsTicks();
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","Initialization","ERROR:  Display Panel Enumeration Timed Out.\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"ERROR:  Display Panel Enumeration Timed Out.\r\n");
#endif
			LogStartupTime(10);
			sinit_state = signinit_StartCommChipEnum;
			break;
		case signinit_StartCommChipEnum:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller", "Enumeration","Enumerating Frisco COM\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Enumeration,"Enumerating Frisco COM\r\n");
#endif
			sinit_state = signinit_CommChipEnum;
			break;
		case signinit_CommChipEnum:
			if(Enumerate_Frisco_Comm_Chip())
			{
				LogStartupTime(7);
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Enumeration","Frisco COM Enumeration Complete.\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Enumeration,"Frisco COM Enumeration Complete.\r\n");
#endif
				++sinit_state;
				MPPT_StartTime =  GetMsTicks(); // ZGy_Note_MPP
			}
			break;
		case signinit_StartMPPTEnumeration:
			//startmuxprintf("Enumerating the MPPT\r\n");
			MPPT_nRST_GPIO_Port->ODR |= MPPT_nRST_Pin;              // ZGy_Note_MPPT
			if(TimedOut(MPPT_StartTime, MPPT_STARTUP_WAIT_INIT_ms)) // ZGy_Note_MPPT
			{
				MPPT_StartTime =  GetMsTicks(); // ZGy_Note_MPPT
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Power_Manager", "Mod_Info","Testing for Power Manager presence.\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Mod_Info,"Testing for Power Manager presence.\r\n");
#endif
				ProbeIf_MPPT_I2C_Present();
				++sinit_state;
			}
			break;
		case signinit_TestForMPPTPresence:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, SBUS_INIT_TAG))
			{
				// aci_print("}==> Testing for Power Manager presence.\r\n");  // ZGy_Note_MPPT
				// if(Is_MPPT_I2C_Detected())       // ZGy_Note_MPPT
				if(CheckIf_MPPT_I2C_Present())      // ZGy_Note_MPPT
				{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
					formatlog("Power_Manager", "Mod_Info","Power Manager present.\r\n");
#else
					formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Mod_Info,"Power Manager present.\r\n");
#endif

					++sinit_state;
					UnlockSMutex(&Sensor_Bus_Master.Mutex, SBUS_INIT_TAG);  // ZGy_Note_MPPT
				}
				else if(TimedOut(MPPT_StartTime, MPPT_CHECKIF_PRESERNT_TIMEOUT_ms))
				{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
					formatlog("Power_Manager", "Mod_Info","MPPT Power Manager Not Present\r\n");
#else
					formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Mod_Info,"MPPT Power Manager Not Present\r\n");
#endif
					sinit_state = signinit_MPPT_Enumeration_Complete;
					UnlockSMutex(&Sensor_Bus_Master.Mutex, SBUS_INIT_TAG);   // ZGy_Note_MPPT
				}
				else
					UnlockSMutex(&Sensor_Bus_Master.Mutex, SBUS_INIT_TAG);    // ZGy_Note_MPPT
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_MPPTEnum:
			// We do not need to lock the sensor bus because the sensor bus master will handle this automatically
			if(Enumerate_Sensor_Bus())
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Power_Manager","Enumeration","MPPT Enumeration Complete.\r\n");
#else
				formatlog(MSG_MOD_ID_Power_Manager,MSG_LOG_ID_Enumeration,"MPPT Enumeration Complete.\r\n");
#endif
				++sinit_state;
			}
			else
			{
				HAL_Delay(1);
			}
			break;
		case signinit_MPPT_Enumeration_Complete:
//			if(Is_Power_Manager_Present())
			{
				Send_MPPT_Enum_Complete_Message();
			}
			++sinit_state;
			break;
		case signinit_EnumerationsComplete:
			++sinit_state;
			break;
		case signinit_ClearDisplays:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","Initialization","Clearing The Display\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"Clearing The Display\r\n");
#endif
			Clear_Displays();
			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_InitData_Log:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller","Initialization","Initializing Data Log\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"Initializing Data Log\r\n");
#endif
				Init_Data_Log();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(7);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_DoStartupFlagChecks:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller","Initialization","Check for Startup Flags\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"Check for Startup Flags\r\n");
#endif
				Do_Startup_Flag_Checks();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(7);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_Module_Post_Enum_Configuration:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","Initialization","Module-Specific Initialization\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"Module-Specific Initialization\r\n");
#endif
			// Notify modules of enumeration success
			Send_Module_Enum_Success_Notifications();

			// Perform Module-specific configuration
			if(Is_Option_Present(ModId_Gpio_Module))
			{
				Configure_GPIO_Module();
			}
			LogStartupTime(6);
			++sinit_state;
			break;
		case signinit_NTCIP_Init:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","Initialization","NTCIP Initialization\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"NTCIP Initialization\r\n");
#endif
			NTCIP_Init();
			LogStartupTime(7);
			++sinit_state;
			break;
		case signinit_Init_Sched_FW_Install:
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","Initialization","Init Scheduled Firmware Install\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"Init Scheduled Firmware Install\r\n");
#endif
#warning 	TODO:  Need to load scheduled install file once RTC Alarms have been refactored.
//			Catalog_Sched_FW_Install_Files();
			LogStartupTime(6);
			++sinit_state;
			break;
		case signinit_SendStartupNotification:
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller","Initialization","Sending the Startup Notification\r\n");
#else
				formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization,"Sending the Startup Notification\r\n");
#endif
				// Broadcast the startup notification message
				Send_Startup_Notification(SubSys_Broadcast, 0);
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
			break;
		case signinit_InitComplete:
			Set_Display_Flashing(false);    // ZGy_Note
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog("Main_Controller","Initialization", "Init Complete\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization, "Init Complete\r\n");
#endif
			LogStartupTime(8);
			if(IS_YDBG_FLAG_SET(Y_Debug_Log_Startup_Time))//VR NOTE: Come back later
			{
				//aci_print("\r\n\t\t\t\t\t\t\t---------------------------------------\r\n");
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
				formatlog("Main_Controller", "Time","Startup Time:  \t%8ld\r\n", GetMsTicks());
#else
				formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Time,"Startup Time:  \t%8ld\r\n", GetMsTicks());
#endif
			}
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization, "Sign Init Complete\r\n");
#else
			formatlog(MSG_MOD_ID_Main_Controller,MSG_LOG_ID_Initialization, "Sign Init Complete\r\n");
#endif
			sinit_state = signinit_RunControllerApp;
			if(IS_XDBG_FLAG_SET(X_Debug_Log_Scheduled_Ops))
			{
				List_Scheduled_Operations();
			}
			if(IS_YDBG_FLAG_SET(Y_Debug_Log_SDCard_Files))
			{
				Dump_SDCard_File_Sizes();
			}
#ifdef TEST_LAT_LON_DECODE  // ZGy_Note
			void Test_LatLonDecode(void);

			Test_LatLonDecode();
#endif
			break;
		case signinit_RunControllerApp:
			// We shouldn't have gotten here, but at least there's no annoying compiler warning!
			break;
		}

		if(sinit_state >= signinit_StartEnumerations)
		{
			// Update low level App resources
			Low_Level_App_Update();
		}

	}while(sinit_state < signinit_RunControllerApp);

	// Place the controller board in the installed options list
	Save_Controller_As_Option();

	if(IS_XDBG_FLAG_SET(X_Debug_Log_Installed_Options))//VR NOTE: COME BACK LATER
	{
#if ((MINOR_FW_VERSION<23)||((MINOR_FW_VERSION==23)&&(SUB_FW_VERSION==0)&&(TEST_FW_VERSION<15)))
		formatlog("Main_Controller", "Installed_Opt","Log Installed Options.\r\n");
#else
		formatlog(MSG_MOD_ID_Main_Controller, MSG_LOG_ID_Installed_Opt,"Log Installed Options.\r\n");
#endif
		Log_Installed_Options();
	}
}
