/*
 * init.c
 *
 *  Created on: Nov 10, 2023
 *      Author: stever
 *
 *  All functions come from  sinit_state in Sign_Init.c
 *  Function's name: EMUM name of sinit_state
 *  Function's body: case statement of sinit_state
 *	We can use find/replace to get all functions:
 *	1. replace "		case " with "int "
 *	2. Replace ":" with "(int sinit_state){"
 *	3. Replace "break;" with "return sinit_state;"
 *  4. Replace "int signinit_" with "int sign_init_" to avoid name conflict
 *
 */


#include "1imp.h"
/*================================================================*/
//void InitApplication(void)
//{
	uint32_t moduleInitTimestamp = 0;
#if 0
	do
	{
		bsp_refresh_iwdg();				//Fixup endless reboot by damage of uSD Card. Stave Rui
		switch(sinit_state)
		{
#endif
/*------------------------------------*/
int sign_init_Start(int sinit_state){
			last_init_time = GetMsTicks();
			Frisco.Modules = Init_Module_Driver();
			formatlog("Main_Controller", "Initialization","Init Application Start.\r\n" );
			// Clear task runtimes array
			memset(Task_Runtimes, 0, sizeof(Task_Runtimes));
			++sinit_state;
return sinit_state;}
int sign_init_Init_Password(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				// Init the password.
				formatlog("Main_Controller", "Initialization","Initializing the Password.\r\n");
				Init_Password();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(7);
				++sinit_state;
			}
			else
			{
				HAL_Delay(10);
			}
return sinit_state;}
int sign_init_CommChipInit(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Frisco COMM.\r\n" );

			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_RS232Init(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing External RS232 Port.\r\n" );
			Init_External_RS232();
			LogStartupTime(6);
			++sinit_state;
return sinit_state;}
int sign_init_InitSensorBus(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Sensor Bus\r\n" );
			Init_Sensor_Bus();
			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_InitAppData(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				formatlog("Main_Controller", "Initialization","Initializing App Data\r\n" );
				Init_App_Data();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(10);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
return sinit_state;}
int sign_init_InituSD(int sinit_state){
			// NOTE: uSD must be checked and mounted before 'Init_Frisco()' and 'Init_Camera()' as they will init different if uSD is present
			formatlog("Main_Controller", "Initialization","Initializing uSD\r\n");
			Init_uSD();
			Init_uSD_Updater();
			LogStartupTime(10);
			++sinit_state;
return sinit_state;}
int sign_init_InitFrisco(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Frisco\r\n");
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
return sinit_state;}
int sign_init_MPPTInit(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				formatlog("Main_Controller", "Initialization","Initializing MPPT\r\n");
				Init_Internal_Charger();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(8);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
return sinit_state;}
int sign_init_InitFriscoCommBus(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Frisco Comm Bus\r\n");
			Init_Frisco_Comm_Bus();
			LogStartupTime(6);
			++sinit_state;
return sinit_state;}
int sign_init_InitCamera(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Camera\r\n");
			Init_Camera();
			LogStartupTime(10);
			++sinit_state;
return sinit_state;}
int sign_init_InitDisplay(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Display\r\n");
			Init_Display();
			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_InitLightSensorManager(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG))
			{
				formatlog("Main_Controller", "Initialization","Initializing Ambient Light Sensor\r\n");
				Init_Light_Sensor_Manager();
				++sinit_state;
				UnlockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG);
				LogStartupTime(6);
			}
			else
			{
				HAL_Delay(2);
			}
return sinit_state;}
int sign_init_InitSignState(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Sign State\r\n");
			InitSignState();
			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_TurnOnRadarPower(int sinit_state){
			if(App_Data.Radar_Enable)
			{
				formatlog("Main_Controller", "Initialization","Powering Up Radar\r\n");
				Turn_On_Radar_Power();
				LogStartupTime(8);
			}
			++sinit_state;
return sinit_state;}
int sign_init_RtcInit(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing RTC\r\n");
			RtcInit();
			LogStartupTime(8);
			++sinit_state;
return sinit_state;}
int sign_init_InitTempSensor(int sinit_state){
			formatlog("Main_Controller", "Initialization","Initializing Temp Sensor\r\n");
			Init_Temp_Sensor();
			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_Ymodem_Init(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
			{
				formatlog("Main_Controller", "Initialization","Initializing Ymodem Download Capabilities\r\n");
				YmodemInit();
				UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
				LogStartupTime(5);
				++sinit_state;
			}
			else
			{
				HAL_Delay(10);
			}
return sinit_state;}
int sign_init_StartEnumerations(int sinit_state){
			sinit_state = signinit_StartModuleBusEnumeration;
return sinit_state;}
int sign_init_StartModuleBusEnumeration(int sinit_state){
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
			formatlog("Main_Controller", "Enumeration","Enumerate Module Bus\r\n");
			sinit_state = signinit_ModuleBusEnum;
return sinit_state;}
		}
int sign_init_ModuleBusEnum(int sinit_state){
			if(Enumerate_Bus_Modules())
			{
				// Module Bus Enumeration Complete
				formatlog("Main_Controller", "Enumeration","Completed Module Bus Enumeration\r\n");
				LogStartupTime(6);
				sinit_state = signinit_StartPanelEnumeration;
			}
			else
			{
//				if(TimesUp(moduleInitTimestamp) && LockSMutex(&SPI2_LED2_Mutex, DISPLAY_MUTEX_TAG))
				if(!TimesUp(moduleInitTimestamp))
		return sinit_state;
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
return sinit_state;}
int sign_init_StartPanelEnumeration(int sinit_state){
			formatlog("Main_Controller", "Enumeration","Enumerate Display Panel Bus\r\n");
			// This timeout shouldn't be necessary--something must be delaying display startup.
			HAL_Delay(PANEL_BUS_ENUM_START_DELAY_MS);
			moduleInitTimestamp = GetMsTicks();
			panel_enum_start_time = pmdGetMsTicks();
			sinit_state = signinit_PanelBusEnum;
return sinit_state;}
int sign_init_PanelBusEnum(int sinit_state){
			if(Enumerate_Display_Panels())
			{
				sinit_state = signinit_WaitForPanelBusEnumComplete;
			}
			else if(TimedOut(panel_enum_start_time, PANEL_BUS_ENUMERATION_TIMEOUT_MS))
			{
				sinit_state = signinit_PanelEnumTimedOut;
			}
return sinit_state;}
int sign_init_WaitForPanelBusEnumComplete(int sinit_state){
			if(PanelBusEnumerationCompleteFlags == 0x03)
			{
				Send_Panel_Enum_Complete_Message();
				LogStartupTime(10);
				sinit_state = signinit_StartCommChipEnum;
			}
return sinit_state;}
int sign_init_PanelEnumTimedOut(int sinit_state){
			Show_Text_Screen("Panel", "Timeout", 2);
			moduleInitTimestamp = GetMsTicks();
			formatlog("Main_Controller","Initialization","ERROR: Display Panel Enumeration Timed Out.\r\n");
			LogStartupTime(10);
			sinit_state = signinit_StartCommChipEnum;
return sinit_state;}
int sign_init_StartCommChipEnum(int sinit_state){
			formatlog("Main_Controller", "Enumeration","Enumerating Frisco COM\r\n");
			sinit_state = signinit_CommChipEnum;
return sinit_state;}
int sign_init_CommChipEnum(int sinit_state){
			if(Enumerate_Frisco_Comm_Chip())
			{
				LogStartupTime(7);
				formatlog("Main_Controller", "Enumeration","Frisco COM Enumeration Complete.\r\n");
				++sinit_state;
				MPPT_StartTime =  GetMsTicks(); // ZGy_Note_MPP
			}
return sinit_state;}
int sign_init_StartMPPTEnumeration(int sinit_state){
			//startmuxprintf("Enumerating the MPPT\r\n");
			MPPT_nRST_GPIO_Port->ODR |= MPPT_nRST_Pin;              // ZGy_Note_MPPT
			if(TimedOut(MPPT_StartTime, MPPT_STARTUP_WAIT_INIT_ms)) // ZGy_Note_MPPT
			{
				MPPT_StartTime =  GetMsTicks(); // ZGy_Note_MPPT
				formatlog("Power_Manager", "Mod_Info","Testing for Power Manager presence.\r\n");
				ProbeIf_MPPT_I2C_Present();
				++sinit_state;
			}
return sinit_state;}
int sign_init_TestForMPPTPresence(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, SBUS_INIT_TAG))
			{
				// aci_print("}==> Testing for Power Manager presence.\r\n");  // ZGy_Note_MPPT
				// if(Is_MPPT_I2C_Detected())       // ZGy_Note_MPPT
				if(CheckIf_MPPT_I2C_Present())      // ZGy_Note_MPPT
				{
					formatlog("Power_Manager", "Mod_Info","Power Manager present.\r\n");

					++sinit_state;
					UnlockSMutex(&Sensor_Bus_Master.Mutex, SBUS_INIT_TAG);  // ZGy_Note_MPPT
				}
				else if(TimedOut(MPPT_StartTime, MPPT_CHECKIF_PRESERNT_TIMEOUT_ms))
				{
					formatlog("Power_Manager", "Mod_Info","MPPT Power Manager Not Present\r\n");
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
return sinit_state;}
int sign_init_MPPTEnum(int sinit_state){
			// We do not need to lock the sensor bus because the sensor bus master will handle this automatically
			if(Enumerate_Sensor_Bus())
			{
				formatlog("Power_Manager","Enumeration","MPPT Enumeration Complete.\r\n");
				++sinit_state;
			}
			else
			{
				HAL_Delay(1);
			}
return sinit_state;}
int sign_init_MPPT_Enumeration_Complete(int sinit_state){
//			if(Is_Power_Manager_Present())
			{
				Send_MPPT_Enum_Complete_Message();
			}
			++sinit_state;
return sinit_state;}
int sign_init_EnumerationsComplete(int sinit_state){
			++sinit_state;
return sinit_state;}
int sign_init_ClearDisplays(int sinit_state){
			formatlog("Main_Controller","Initialization","Clearing The Display\r\n");
			Clear_Displays();
			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_InitData_Log(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				formatlog("Main_Controller","Initialization","Initializing Data Log\r\n");
				Init_Data_Log();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(7);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
return sinit_state;}
int sign_init_DoStartupFlagChecks(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				formatlog("Main_Controller","Initialization","Check for Startup Flags\r\n");
				Do_Startup_Flag_Checks();
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				LogStartupTime(7);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
return sinit_state;}
int sign_init_Module_Post_Enum_Configuration(int sinit_state){
			formatlog("Main_Controller","Initialization","Module-Specific Initialization\r\n");
			// Notify modules of enumeration success
			Send_Module_Enum_Success_Notifications();

			// Perform Module-specific configuration
			if(Is_Option_Present(ModId_Gpio_Module))
			{
				Configure_GPIO_Module();
			}
			LogStartupTime(6);
			++sinit_state;
return sinit_state;}
int sign_init_NTCIP_Init(int sinit_state){
			formatlog("Main_Controller","Initialization","NTCIP Initialization\r\n");
			NTCIP_Init();
			LogStartupTime(7);
			++sinit_state;
return sinit_state;}
int sign_init_Init_Sched_FW_Install(int sinit_state){
			formatlog("Main_Controller","Initialization","Init Scheduled Firmware Install\r\n");
#warning 	TODO:  Need to load scheduled install file once RTC Alarms have been refactored.
//			Catalog_Sched_FW_Install_Files();
			LogStartupTime(6);
			++sinit_state;
return sinit_state;}
int sign_init_SendStartupNotification(int sinit_state){
			//startmuxprintf("}");
			if(LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
			{
				formatlog("Main_Controller","Initialization","Sending the Startup Notification\r\n");
				// Broadcast the startup notification message
				Send_Startup_Notification(SubSys_Broadcast, 0);
				UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
				++sinit_state;
			}
			else
			{
				HAL_Delay(2);
			}
return sinit_state;}
int sign_init_InitComplete(int sinit_state){
			Set_Display_Flashing(false);    // ZGy_Note
			formatlog("Main_Controller","Initialization", "Init Complete\r\n");
			LogStartupTime(8);
			if(IS_YDBG_FLAG_SET(Y_Debug_Log_Startup_Time))//VR NOTE: Come back later
			{
				//aci_print("\r\n\t\t\t\t\t\t\t---------------------------------------\r\n");
				formatlog("Main_Controller", "Time","Startup Time:  \t%8ld\r\n", GetMsTicks());
			}
			formatlog("Main_Controller","Initialization", "Sign Init Complete\r\n");
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
return sinit_state;}
int sign_init_RunControllerApp(int sinit_state){
			// We shouldn't have gotten here, but at least there's no annoying compiler warning!
return sinit_state;}
#if 0
if(sinit_state >= signinit_StartEnumerations)
{
	// Update low level App resources
	Low_Level_App_Update();
}

}while(sinit_state < signinit_RunControllerApp);
#endif
/*------------------------------------*/
int sign_init_is_end(int sinit_state)
{
	if(sinit_state < signinit_RunControllerApp)
		return 0;
	// Place the controller board in the installed options list
	Save_Controller_As_Option();

	if(IS_XDBG_FLAG_SET(X_Debug_Log_Installed_Options))//VR NOTE: COME BACK LATER
	{
		formatlog("Main_Controller", "Installed_Opt","Log Installed Options.\r\n");
		Log_Installed_Options();
	}
	return 1;
}

/*================================================================*/
/* end of init.c */
