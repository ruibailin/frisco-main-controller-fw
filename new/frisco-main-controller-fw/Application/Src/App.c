/*
 * App.c
 *
 *  Created on: Sep 15, 2017
 *      Author: mikea
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "Power_Manager/Power_Manager.h"
#include "Sensor_Bus.h"
#include "App.h"
#include "Module_Driver/I2C_Bus_Master.h"
#include "Frisco.h"
#include "Module_Hardware_Drivers/433_Wireless_Receiver.h"
#include "../Digit_Display.h"
#include "../Display_Files/Page/Page.h"
#include "../Display_Files/File/File.h"
#include "../Display_Files/Display_Files.h"
#include "../Radar/Radar.h"
#include "../Radar/Stats_Port.h"
#include "util.h"
#include "FriscoDefs.h"
#include "Module_Hardware_Drivers/Wifi_Module.h"
#include "Module_Hardware_Drivers/3G_Module.h"
#include "../Cameras/Camera.h"
#include "../Sensors/Light_Sensor.h"
#include "Panel_Bus.h"
#include "App_Data.h"
#include "Testpoints.h"
#include "fatfs.h"
#include "RTC/RtcDrv.h"
#include "Mutex.h"
#include "../Menu/Menu.h"
#include "../Msg/Machine_Command_Processor.h"
#include "Sensors/ADC_Sensor.h"
#include "../Firmware_Update/Ymodem.h"
#include "Module_Hardware_Drivers/GPIO_Module.h"
#include "../Flash/W25Q_Flash.h"
#include "../UART/External_RS232.h"
#include "Module_Hardware_Drivers/Bluetooth_Module.h"
#include "Module_Hardware_Drivers/Ethernet_Module.h"
#include "../Middlewares/ACI/Networking/Networking.h"
#include "../Middlewares/ACI/Networking/Ethernet/Ethernet.h"
#include "../Middlewares/ACI/Networking/WIFI/WIFI.h"
#include "../Middlewares/ACI/Networking/LTE/LTE_if.h"
#include "Sequencer.h"
#include "Timestamp.h"
#include "Debug_Flags.h"
#include "usb_host.h"
#include "Module_Hardware_Drivers/4G_Module.h"
#include "PowerControl.h"
#include "../UART/Module_UART.h"
#include "I2C_Bus_Watchdog.h"
#include "../Firmware_Update/Firmware_Update_Master.h"

#include "Stats_Buffed.h"              //Steve Rui
#include "Parameter_Map_Alert.h"		//Steve Rui
#include "Debug_Log_Msg.h"

extern Mutex_t 				SPI2_LED2_Mutex;
extern I2C_Bus_Master 		Module_Bus_Master;
extern I2C_Bus_Master 		Sensor_Bus_Master;
//extern I2C_Bus_Master 		Frisco_Comm_Bus_Master;
extern I2C_Bus_Master 		Panel_Bus_1_Master;
extern I2C_Bus_Master 		Panel_Bus_2_Master;
extern uint8_t 				Radar_Message_Waiting;
extern uint8_t 				Demo_Mode_Active;
extern uint8_t 				Menu_Timeout_Override;
extern MPPT 				mppt;
extern MENU 				Menu;
extern uint8_t				Firmware_Update_Flag;
extern uint8_t 				Sending_AC_Stats_Flag;
extern _433_RECEIVER 		_433_Receiver;
extern uint8_t				Do_Test_Message_Parsing;
extern uint8_t 				Multi_Packet_Incoming_Msg_In_Flight;
extern uint32_t 			Multi_Packet_Timeout_Time;
extern uint8_t 				USB_Net_Host_Active;

volatile uint8_t _433_Key_Pressed_Flag = 0;

volatile uint8_t Slot_1_Int_Flag = 0;
volatile uint8_t Slot_2_Int_Flag = 0;
volatile uint8_t Slot_3_Int_Flag = 0;
volatile uint8_t Slot_4_Int_Flag = 0;
volatile uint8_t Slot_5_Int_Flag = 0;
volatile uint8_t Slot_6_Int_Flag = 0;
volatile uint8_t Slot_7_Int_Flag = 0;
uint8_t Slot_X_Enum_Flag[MAX_NUM_MODULE_PORTS]; // = {1, 1, 0, 0, 1, 0, 0}; // ZGy_Note

volatile uint8_t LED_1_Int_Flag = 0;
volatile uint8_t LED_2_Int_Flag = 0;
volatile uint8_t MPPT_Int_Flag = 0;
volatile uint8_t Power_Down_Request_Flag = 0;

volatile uint8_t ALS_Interrupt_Flag = 0;
volatile uint8_t TempSensorConvComplete_Flag = 0;
volatile uint8_t GPIO_Input_Active_Events = 0;
volatile uint8_t Trigger_I2C_Bus_Function_Test = 0;
uint8_t pass = 0;



const char *Task_Strings[] = {"LowLevel",
							"Radar",
							"MPPT",
							"Stats",
							"ALS",
							"433",
							"Temp_Sensor",
							"Wifi",
							"3G",
							"SignState",
							"Demo",
							"RTC",
							"Display",
							"FS",
							"Ymodem",
							"MsgParser",
							"DataLogger",
							"SendMsg",
							"ModuleBus",
							"PanelBus_1",
							"PanelBus_2",
							"Watchdog",
							"MPPT_Data_Send",
							"External_RS232",
							"Firmware_Update",
							"Prep_For_Firmware_Update",
							"Send_AC_Stats",
							"Bluetooth",
							"Ethernet_Module",
							"Composite_Sequence",
							"GPIO",
							"433_Menu",
							"Module_RTC_Update",
							"Radar Distance Update",
							"Sensor_Bus_Function_Test",
							"Ethernet",
							"LTE",
							"WiFi",
							"USB_Net_Host",
							"USB_Thumbdrive_Host",
							"Multi_Packet_Timeout",
							"Power_Control",
							"Module_UART_Passthru",
							"4G",
							"Mainloop"};

#define TASK_RUNTIME_LPALPHA		10
#define TASK_RUNTIME_FP_DIGITS		6

static uint8_t first_pass = 1;

Task_Runtimes_t Task_Runtimes[Task_Num_Tasks];
static uint32_t TaskStartTime = 0;
static Tasks_Enum_t CurTask = 0;

uint8_t 				Data_Log_Update_Flag = 0;
uint32_t 				reg_rcc_csr_copy;
uint32_t 				BusStateUpdatePeriod;
uint32_t 				SignStateTime = 0;
uint32_t 				ModuleBusUpdateTime = 0;
uint32_t 				PanelBusUpdateTime = 0;
uint32_t 				PanelBus2UpdateTime = 0;
uint32_t 				SensorBusUpdateTime = 0;
uint32_t				 DisplayTime = 0;
uint32_t 				HeartbeatTime = 0;
uint32_t 				WDPetTime = 0;
uint32_t 				WifiUpdateTime = 6000;
uint32_t 				ThreeGUpdateTime = 6000;
uint32_t 				ALSUpdateTime = 0;
uint32_t 				fatFsUpdateTime = 0;
uint32_t 				rtcUpdateTime = RTC_UPDATE_PERIOD_MS;
uint32_t				 DemoModeUpdateTime = 0;
#ifdef HAMMERED_SENSOR_BUS_TESTING
uint32_t 				DataLoggerUpdateTime = 10000;	// Allow 45 seconds to start up and capture initial samples
#else
uint32_t 				DataLoggerUpdateTime = 45000;	// Allow 45 seconds to start up and capture initial samples
#endif
uint32_t 				MPPTSampleUpdateTime = 0;
uint32_t 				MenuIdleTimeoutTime = MAX_APP_TIME;
uint32_t 				StatsUpdateTime = 0;
uint32_t 				MessageTestTime = 0;
uint32_t 				TempSensorUpdateTime = 0;
uint32_t 				MPPT_Update_Time = 0;
uint32_t 				External_RS232_Update_Time = 0;
uint32_t 				Menu_Map_Update_Reset_Handler = 0;
uint32_t 				FirmwareUpdateTime = 0;
uint32_t 				BluetoothSampleUpdateTime = 0;
uint32_t 				EthernetSampleUpdateTime = 0;
uint32_t				NTCIP_Update_Time = 10000;
uint32_t				Module_RTC_Update_Time = 8000;
uint32_t				Ethernet_Update_Time = 0;
uint32_t				LTE_Update_Time = 0;
uint32_t				WiFi_Update_Time = 0;
uint32_t				USB_Net_Host_Update_Time = 0;
uint32_t				USB_Thumbdrive_Host_Update_Time = 0;
uint32_t				Radar_Distance_Update_Time = RADAR_DISTANCE_UPDATE_TIME_MS;
uint32_t				I2C_Bus_Watchdog_Time = I2C_BUS_WATCHDOG_TIME_MS;
uint32_t 				LTE_4G_Update_Time = 0;
uint32_t				PowerControl_Update_Time = MAX_APP_TIME;
uint32_t				StatsUploadTaskTime = 0;
uint32_t				AlertMessageTestTime = 240000; //3600000l;
uint32_t				EthernetBroadcastUpdateTime = 0;

void Low_Level_App_Update(void);
void Update_Demo_Mode(void);
void Exit_Menu(void);
uint8_t Test_Msg_Parser(void);
void Check_For_RTC_Alarms(void);
void Process_Power_Manager_Data_Messages(void);
void Start_Demo_Mode(void);
void Update_AC_Stats_State(void);
void Process_Incoming_Multi_Packet_Timeout(void);
void Multi_Packet_Timeout(void);
void Sensor_Bus_Function_Test(void);
uint8_t Update_Data_Logger(void);

static volatile uint32_t Reset_Reason = 0;

void LogResetReason(uint32_t rcc_csr)
{
	if(rcc_csr & (1<<31)) formatlog("Main_Controller","Initialization","Reset Reason:Low-power reset\r\n");
	if(rcc_csr & (1<<30)) formatlog("Main_Controller","Initialization","Reset Reason:Window watchdog reset\r\n");
	if(rcc_csr & (1<<29)) formatlog("Main_Controller","Initialization","Reset Reason:Independent watchdog reset\r\n");
	if(rcc_csr & (1<<28)) formatlog("Main_Controller","Initialization","Reset Reason:Software reset\r\n");
	if(rcc_csr & (1<<27)) formatlog("Main_Controller","Initialization","Reset Reason:Power-on/Power-down reset\r\n");
	if(rcc_csr & (1<<26)) formatlog("Main_Controller","Initialization","Reset Reason:Pin reset\r\n");
	if(rcc_csr & (1<<25)) formatlog("Main_Controller","Initialization","Reset Reason:Brown out reset\r\n");
	Reset_Reason = rcc_csr;
}

uint32_t Get_Reset_Reason(void)
{
	return Reset_Reason;
}

void Init_Task_Monitor(void)
{
//	for(uint8_t i=0; i<Task_Num_Tasks; ++i)
//	{
//		Task_Runtimes[i].Runcount = 0;
//		Task_Runtimes[i].Runtime = 0;
//		Task_Runtimes[i].Maxtime = 0;
//	}
	memset(Task_Runtimes, 0, sizeof(Task_Runtimes));
}

static void Monitor_Task(Tasks_Enum_t task)
{
	TaskStartTime = pmdGetMsTicks();
	CurTask = task;
}

static void Update_Task_Runtime(Tasks_Enum_t task)
{
	if(task < Task_Num_Tasks)
	{
		if(Task_Runtimes[task].Runtime)
		{
			uint32_t runtime = pmdGetMsTicks() - TaskStartTime;
			// Low pass the fixed point value

			Task_Runtimes[task].Runtime += ((int32_t)(runtime << TASK_RUNTIME_FP_DIGITS) - (int32_t)Task_Runtimes[task].Runtime) >> TASK_RUNTIME_LPALPHA;

			if(runtime > Task_Runtimes[task].Maxtime)
			{
				Task_Runtimes[task].Maxtime = runtime;
				if(runtime > 5000)
				{
					formatlog("Main_Controller","Runtime_Task","WARNING:  Excessive Runtime Task %d\r\n", task);
				}
			}
		}
		else
		{
			// Initialize task runtime in fixed point format
			Task_Runtimes[task].Runtime = (pmdGetMsTicks() - TaskStartTime) << TASK_RUNTIME_FP_DIGITS;
		}

		++Task_Runtimes[task].Runcount;
	}

	if(task != Task_Mainloop)
	{
	}
	CurTask = Task_Mainloop;
}

void Print_Task_Runtimes(void)//VR NOTE: Come back later
{
	uint8_t i;

	aci_print("\r\n    Task Runtimes\r\n-------------------------\r\n");
	aci_print(" Task Name             Avg Runtime (ms*%d)   Runcount   Total Runtime (ms)  Max Runtime\r\n", 1 << TASK_RUNTIME_FP_DIGITS);
	aci_print("-----------------------------------------------------------------------------------------------\r\n");
	for(i=0; i<Task_Num_Tasks; ++i)
	{
		aci_print("%32s: %6ld  %10ld   %9ld    %8ld\r\n", Task_Strings[i],
				Task_Runtimes[i].Runtime, Task_Runtimes[i].Runcount,
				(Task_Runtimes[i].Runtime * Task_Runtimes[i].Runcount) >> TASK_RUNTIME_FP_DIGITS,
				Task_Runtimes[i].Maxtime);
	}
}

Tasks_Enum_t Get_Current_Task(void)
{
	return CurTask;
}

#include "Stats_Buffed.h"
extern void pmm_check_change_task(void);
void RunApplication(void)
{
	volatile uint32_t curtime = GetMsTicks();
	uint32_t mainloop_start;
	mainloop_start = curtime;

//	Low_Level_App_Update();

	if(Firmware_Install_Active_Flag)
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
	else
	{
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

		// 	Handle USB Net Host Tasks
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

		// 	Handle Ethernet Tasks
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

		// 	Handle WiFi Tasks
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

		// 	Handle 4G Tasks
		/////////////////////////////////////////////
		if(Networking.LTE_Interface_Up && TimesUp(LTE_Update_Time))
		{
			Monitor_Task(Task_LTE);
			LTE_Tasks();
			Update_Task_Runtime(Task_LTE);
			LTE_Update_Time = GetMsTicks() + LTE_UPDATE_TIME_MS;
		}

		//  Processes AC Stats Output
		//////////////////////////////////////////////
		if(Sending_AC_Stats_Flag && TimesUp(StatsUploadTaskTime))
		{
			Monitor_Task(Task_Send_AC_Stats);
			Update_AC_Stats_State();
			Update_Task_Runtime(Task_Send_AC_Stats);
			StatsUploadTaskTime = GetMsTicks() + STATS_UPLOAD_TASK_PERIOD_MS;
		}



		//  Processes Bluetooth messages
		//////////////////////////////////////////////
		if(Is_Bluetooth_Module_Installed() && TimesUp(BluetoothSampleUpdateTime) && LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			Monitor_Task(Task_Bluetooth);
			Bluetooth_Tasks();
			BluetoothSampleUpdateTime = GetMsTicks() + BLUETOOTH_SAMPLE_PERIOD_MS;
			Update_Task_Runtime(Task_Bluetooth);
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
		}



		//  Processes Ethernet module tasks
		//////////////////////////////////////////////
		if(Ethernet_Module_Installed() && TimesUp(EthernetSampleUpdateTime))
		{
			Monitor_Task(Task_Ethernet_Module);
			Ethernet_Module_Tasks();
			EthernetSampleUpdateTime = GetMsTicks() + Ethernet_SAMPLE_PERIOD_MS;
			Update_Task_Runtime(Task_Ethernet_Module);
		}

		//  Processes networking keep alive broadcast
		//////////////////////////////////////////////
		if(TimesUp(EthernetBroadcastUpdateTime))
		{
			Networking_Tasks();
			EthernetBroadcastUpdateTime = GetMsTicks() + 15000;
		}

		//  Processes 4G tasks
		//////////////////////////////////////////////
		if(Is_4G_Module_Installed() && TimesUp(LTE_4G_Update_Time))
		{
			Monitor_Task(Task_4G);
			Run_4G_Tasks();
			LTE_4G_Update_Time = GetMsTicks() + LTE_4G_SAMPLE_PERIOD_MS;
			Update_Task_Runtime(Task_4G);
		}

		//  Service Radar Sensor
		/////////////////////////////////////////////
		if(Radar_Message_Waiting && App_Data.Radar_Enable)
		{
			Monitor_Task(Task_Radar);
			Process_Radar_Messages();
			Update_Task_Runtime(Task_Radar);
		}

		stats_task_send_records();              //Steve Rui,send radar stats data to server

		//	Read MPPT Sensor Data
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


		//	Process GPIO Events
		/////////////////////////////////////////////
		if((GPIO_Active_Output_Ports || GPIO_Input_Active_Events) && TimesUp(GPIO_Update_Time))
		{
			Monitor_Task(Task_GPIO);
			Do_GPIO_Update();
			Update_Task_Runtime(Task_GPIO);
		}


		//	Process Sequencer Update
		/////////////////////////////////////////////
		if(TimesUp(Sequencer_Update_Time))
		{
			Monitor_Task(Task_Composite_Sequence);
			Update_Composite_Sequence();
			Update_Task_Runtime(Task_Composite_Sequence);
		}


static	int sta_no_work=0;
static	int sta_work=0;
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



		//  Service Ambient Light Sensor
		/////////////////////////////////////////////
		if( (ALS_Interrupt_Flag || TimesUp(ALSUpdateTime)) && LockSMutex(&Sensor_Bus_Master.Mutex, ALS_MUTEX_TAG))
		{
			static uint8_t retries = 0;
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



		//  Service 433 MHz Wireless Module
		/////////////////////////////////////////////
		if(_433_Key_Pressed_Flag && !IsSMutexLocked(&Sensor_Bus_Master.Mutex))
		{
			_433_Key_Pressed_Flag = 0;

			Monitor_Task(Task_433);
			_433_State_Machine();
			pmm_fob_check_change();
			Update_Task_Runtime(Task_433);
		}
		if(_433_Receiver.Menu_Active)
		{
			Monitor_Task(Task_433_Menu);
			_433_Menu_State_Machine();
			pmm_fob_check_change();
			Update_Task_Runtime(Task_433_Menu);
		}

		pmm_check_change_task();

		//  Temp Sensor Update
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



		//  WIFI Update
		/////////////////////////////////////////////
		//	if(TimesUp(WifiUpdateTime))
		//	{
		//		Monitor_Task();
		//		Do_Wifi_Update();
		//		Update_Task_Runtime(Task_Wifi);
		//		WifiUpdateTime = GetMsTicks() + WIFITESTPERIOD;
		//	}

		//  3G Update
		/////////////////////////////////////////////
		//	if(TimesUp(ThreeGUpdateTime))
		//	{
		//		Monitor_Task();
		//		Do_3G_Update();
		//		Update_Task_Runtime(Task_3G);
		//		ThreeGUpdateTime = GetMsTicks() + THREEGTESTPERIOD;
		//	}


		//  Update the Sign State
		/////////////////////////////////////////////
		if(!Demo_Mode_Active && TimesUp(SignStateTime) )
		{

			Monitor_Task(Task_SignState);
			UpdateSignState();
			Update_Task_Runtime(Task_SignState);
			SignStateTime = GetMsTicks() + SIGN_STATE_UPDATE_PERIOD_MS;
		}



		//  Demo Mode Update
		/////////////////////////////////////////////
		if(Demo_Mode_Active && TimesUp(DemoModeUpdateTime))
		{
			Monitor_Task(Task_Demo);
			Update_Demo_Mode();
			Update_Task_Runtime(Task_Demo);
			DemoModeUpdateTime = GetMsTicks() + DEMO_MODE_UPDATE_PERIOD_MS;
		}



		//  Update the RTC
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



		//  Update the Display
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


		//  Update the File System
		/////////////////////////////////////////////
		//	if(TimesUp(fatFsUpdateTime))
		//	{
		//		Monitor_Task();
		//	    fatFsTask();
		//		Update_Task_Runtime(Task_FS);
		//	    fatFsUpdateTime = GetMsTicks() + FATFS_UPDATE_PERIOD_MS;
		//	}


		// Parse external commands (unless Ymodem transfer in progress)
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


		// 	Firmware Update
		/////////////////////////////////////////////
		if(Firmware_Update_Flag && TimesUp(FirmwareUpdateTime) && LockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG))
		{
			Monitor_Task(Task_Prep_For_Firmware_Update);
			Firmware_Update_State();
			UnlockSMutex(&SPI2_LED2_Mutex, SPI_FLASH_MUTEX_TAG);
			FirmwareUpdateTime = pmdGetMsTicks() + FIRMWARE_UPDATE_PERIOD_MS;
			Update_Task_Runtime(Task_Prep_For_Firmware_Update);
		}



		// 	Log Data to Non-Volatile Memory
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



		//	 Send AC Messages
		///////////////////////////////////////////
		if(Serial_Output_Buffer_Msg_Pending())
		{
//			aci_print("<S>");
			Monitor_Task(Task_SendMsg);
			Process_Outgoing_Serial_Messages();
			Update_Task_Runtime(Task_SendMsg);
		}



		//	 Send MPPT Data
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




		//	 Multi-Packet Processing
		///////////////////////////////////////////
		if(Multi_Packet_Incoming_Msg_In_Flight && TimesUp(Multi_Packet_Timeout_Time))
		{
			Monitor_Task(Task_Multi_Packet_Timeout);
			Multi_Packet_Timeout();
			Update_Task_Runtime(Task_Multi_Packet_Timeout);
		}




		//	 AC Reset Msg Handler
		///////////////////////////////////////////
		if(Menu_Map_Update_Reset_Handler)
		{
			// Delay until ACK message has been sent.
			if(All_Messages_Sent())
			{
				Reset_Sign();
			}
		}


		//	 Maintain the External RS232 port
		///////////////////////////////////////////
		if(TimesUp(External_RS232_Update_Time))
		{
			Monitor_Task(Task_External_RS232);
			External_RS232_Tasks();
			Update_Task_Runtime(Task_External_RS232);
			External_RS232_Update_Time = GetMsTicks() + EXTERNAL_RS232_UPDATE_PERIOD_MS;
		}


		//	 Update Module RTCs
		///////////////////////////////////////////
		if(TimesUp(Module_RTC_Update_Time))
		{
			Monitor_Task(Task_Module_RTC_Update);
			Send_RTC_Update_To_All_Modules();
			Update_Task_Runtime(Task_Module_RTC_Update);
			Module_RTC_Update_Time = GetMsTicks() + MODULE_RTC_UPDATE_PERIOD_MS;
		}


#ifdef DO_RADAR_DISTANCE_UPDATE

		//	 Update Radar Distance
		///////////////////////////////////////////
		if(TimesUp(Radar_Distance_Update_Time) && LockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG))
		{
			Monitor_Task(Task_Radar_Distance_Update);
			Update_Radar_Sensitivity();
			UnlockSMutex(&Sensor_Bus_Master.Mutex, EEPROM_MUTEX_TAG);
			Update_Task_Runtime(Task_Radar_Distance_Update);
			Radar_Distance_Update_Time = GetMsTicks() + RADAR_DISTANCE_UPDATE_TIME_MS;
		}

#endif


		//	 Run I2C Watchdogs
		///////////////////////////////////////////
		if((Trigger_I2C_Bus_Function_Test || TimesUp(I2C_Bus_Watchdog_Time)))
		{
			TP_H;
			Monitor_Task(Task_I2C_Bus_Watchdog);

//			static uint8_t bus_trigger_flags = 0;

			//TODO:  Need to do a round-robin over the busses even if another bus is locked up.
//			static uint8_t bus_flag = 1;

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



		//	 Power Control Task
		///////////////////////////////////////////
		if(TimesUp(PowerControl_Update_Time))
		{
			Monitor_Task(Task_Power_Control);
			PowerControlTask();
			Update_Task_Runtime(Task_Power_Control);
			PowerControl_Update_Time = GetMsTicks() + POWER_CONTROL_PERIOD_MS;
		}

		// Module UART Passthru
		///////////////////////////////////////////
		if(IS_YDBG_FLAG_SET(Y_Debug_Enable_Module_UART_Log) && Is_Module_Uart_Passthru_Empty())
		{
			Monitor_Task(Task_Module_UART_Passthru);
			Process_Module_Uart_Log_Passthru();
			Update_Task_Runtime(Task_Module_UART_Passthru);
		}

		//	 Run Tests
		///////////////////////////////////////////
		if(IS_XDBG_FLAG_SET(X_Debug_AC_Message_Test_Parsing))
		{
			if(Do_Test_Message_Parsing && TimesUp(MessageTestTime))
			{
				Do_Test_Message_Parsing = Test_Msg_Parser();
				MessageTestTime = GetMsTicks() + 1000;
			}
		}

		//	 Run Alert Tests
		///////////////////////////////////////////
		if(TimesUp(AlertMessageTestTime))
		{
			void Do_Alert_Message_Testing(void);
			Do_Alert_Message_Testing();
			AlertMessageTestTime = GetMsTicks() + 3600000l;
		}

		if(first_pass)
		{
			if(App_Data.Auto_Start_Demo_Mode)
			{
				Set_Menu_Active(true);
				Start_Demo_Mode();
			}
			first_pass = 0;
		}

	}

	// Cheat the start time for the mainloop
	TaskStartTime = mainloop_start;
	Update_Task_Runtime(Task_Mainloop);
}




void Low_Level_App_Update(void)
{
	//  Process interrupt inputs from modules
	/////////////////////////////////////////////
	Monitor_Task(Task_LowLevel);

	// Slot 1 Interrupt Handler
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

	// Power Down Request Flag
	//////////////////////////////
	if(Power_Down_Request_Flag)
	{
		PowerControl_InitiateShutdown();
	}

	// MPPT Interrupt Handler
	//////////////////////////////
	if(MPPT_Int_Flag && Is_Sensor_Bus_Ready())
	{
		if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
		{
			formatlog("Main_Controller","Interrupt","IMPPT\r\n");
		}
		Sensor_Bus_INT_Callback();
		MPPT_Int_Flag = 0;
	}

	// LED 1 Interrupt Handler
	//////////////////////////////
	if(LED_1_Int_Flag && Is_Panel_Bus_1_Ready())
	{
		//if(IS_XDBG_FLAG_SET(X_Debug_Interrupts))
	//	{
			formatlog("Main_Controller","Interrupt","ILED1\r\n");
	//	}
		Panel_Bus_1_INT_Callback();
		LED_1_Int_Flag = 0;
	}

	// LED 2 Interrupt Handler
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

	//  Service Module Bus State Machine
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

	//  Service Panel Bus State Machine
	/////////////////////////////////////////////
	if(TimesUp(PanelBusUpdateTime) || Panel_Bus_1_Master.KickBusProcess)
	{
		Panel_Bus_1_Master.KickBusProcess = 0;
		Panel_Bus_1_Tasks();
		PanelBusUpdateTime = GetMsTicks() + Panel_Bus_1_Master.StateUpdatePeriod;
	}

	//  Service Panel Bus State Machine
	/////////////////////////////////////////////
	if(TimesUp(PanelBus2UpdateTime) || Panel_Bus_2_Master.KickBusProcess)
	{
		Panel_Bus_2_Master.KickBusProcess = 0;
		Panel_Bus_2_Tasks();
		PanelBus2UpdateTime = GetMsTicks() + Panel_Bus_2_Master.StateUpdatePeriod;
	}

	//  Sensor Bus State Machine
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

	//  Log Heartbeat Character
	/////////////////////////////////////////////
//	if(TimesUp(HeartbeatTime))
//	{
//		FCPutc('~');
//		HeartbeatTime = GetMsTicks() + LOG_HEARTBEAT_PERIOD_MS;
//	}

	//  Pet the Watchdog
	/////////////////////////////////////////////
	if(TimesUp(WDPetTime))
	{
		PetWatchdog();
		WDPetTime = GetMsTicks() + WDPETPERIOD;
	}

	//  Menu Idle Timeout
	/////////////////////////////////////////////
	if(!Demo_Mode_Active && Menu.Is_Active && TimesUp(MenuIdleTimeoutTime) && !Menu_Timeout_Override)
	{
		Exit_Menu();
	}

	Update_Task_Runtime(Task_LowLevel);
}

