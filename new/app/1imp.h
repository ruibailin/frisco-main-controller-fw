/*
 * 1imp.h
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */

#ifndef APP_1IMP_H_
#define APP_1IMP_H_
/*================================================================*/
#include "2exp.h"
/*------------------------------------*/
#include <stdio.h>
#define app_print(x...)  	printf(x);
#define app_debug(x...)  	printf(x);
#define app_alarm(x...)   	printf(x);
#define app_panic(x...)		printf(x);
/*------------------------------------*/
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

//volatile
extern volatile uint8_t _433_Key_Pressed_Flag;

extern volatile uint8_t Slot_1_Int_Flag;
extern volatile uint8_t Slot_2_Int_Flag;
extern volatile uint8_t Slot_3_Int_Flag;
extern volatile uint8_t Slot_4_Int_Flag;
extern volatile uint8_t Slot_5_Int_Flag;
extern volatile uint8_t Slot_6_Int_Flag;
extern volatile uint8_t Slot_7_Int_Flag;
extern uint8_t Slot_X_Enum_Flag[MAX_NUM_MODULE_PORTS]; // = {1, 1, 0, 0, 1, 0, 0}; // ZGy_Note

extern volatile uint8_t LED_1_Int_Flag;
extern volatile uint8_t LED_2_Int_Flag;
extern volatile uint8_t MPPT_Int_Flag;
extern volatile uint8_t Power_Down_Request_Flag;

extern volatile uint8_t ALS_Interrupt_Flag;
extern volatile uint8_t TempSensorConvComplete_Flag;
extern volatile uint8_t GPIO_Input_Active_Events;
extern volatile uint8_t Trigger_I2C_Bus_Function_Test;
extern uint8_t pass;

extern const char *Task_Strings[];

extern uint8_t first_pass;// = 1;

extern Task_Runtimes_t Task_Runtimes[Task_Num_Tasks];
extern uint32_t TaskStartTime;// = 0;
extern Tasks_Enum_t CurTask;// = 0;

//uint

extern uint8_t 				Data_Log_Update_Flag;
extern uint32_t 				reg_rcc_csr_copy;
extern uint32_t 				BusStateUpdatePeriod;
extern uint32_t 				SignStateTime;
extern uint32_t 				ModuleBusUpdateTime;
extern uint32_t 				PanelBusUpdateTime;
extern uint32_t 				PanelBus2UpdateTime;
extern uint32_t 				SensorBusUpdateTime;
extern uint32_t				 DisplayTime;
extern uint32_t 				HeartbeatTime;
extern uint32_t 				WDPetTime;
extern uint32_t 				WifiUpdateTime;     //= 6000;
extern uint32_t 				ThreeGUpdateTime;     //= 6000;
extern uint32_t 				ALSUpdateTime;
extern uint32_t 				fatFsUpdateTime;
extern uint32_t 				rtcUpdateTime;     //= RTC_UPDATE_PERIOD_MS;
extern uint32_t				 DemoModeUpdateTime;
#ifdef HAMMERED_SENSOR_BUS_TESTING
extern uint32_t 				DataLoggerUpdateTime;     //= 10000;	// Allow 45 seconds to start up and capture initial samples
#else
extern uint32_t 				DataLoggerUpdateTime;     //= 45000;	// Allow 45 seconds to start up and capture initial samples
#endif
extern uint32_t 				MPPTSampleUpdateTime;
extern uint32_t 				MenuIdleTimeoutTime;     //= MAX_APP_TIME;
extern uint32_t 				StatsUpdateTime;
extern uint32_t 				MessageTestTime;
extern uint32_t 				TempSensorUpdateTime;
extern uint32_t 				MPPT_Update_Time;
extern uint32_t 				External_RS232_Update_Time;
extern uint32_t 				Menu_Map_Update_Reset_Handler;
extern uint32_t 				FirmwareUpdateTime;
extern uint32_t 				BluetoothSampleUpdateTime;
extern uint32_t 				EthernetSampleUpdateTime;
extern uint32_t				NTCIP_Update_Time;     //= 10000;
extern uint32_t				Module_RTC_Update_Time;     //= 8000;
extern uint32_t				Ethernet_Update_Time;
extern uint32_t				LTE_Update_Time;
extern uint32_t				WiFi_Update_Time;
extern uint32_t				USB_Net_Host_Update_Time;
extern uint32_t				USB_Thumbdrive_Host_Update_Time;
extern uint32_t				Radar_Distance_Update_Time;     //= RADAR_DISTANCE_UPDATE_TIME_MS;
extern uint32_t				I2C_Bus_Watchdog_Time;     //= I2C_BUS_WATCHDOG_TIME_MS;
extern uint32_t 				LTE_4G_Update_Time;
extern uint32_t				PowerControl_Update_Time;     //= MAX_APP_TIME;
extern uint32_t				StatsUploadTaskTime;
extern uint32_t				AlertMessageTestTime;     //= 240000; //3600000l;
extern uint32_t				EthernetBroadcastUpdateTime;

extern void  Low_Level_App_Update(void);
extern void  Update_Demo_Mode(void);
extern void  Exit_Menu(void);
extern uint8_t Test_Msg_Parser(void);
extern void  Check_For_RTC_Alarms(void);
extern void  Process_Power_Manager_Data_Messages(void);
extern void  Start_Demo_Mode(void);
extern void  Update_AC_Stats_State(void);
extern void  Process_Incoming_Multi_Packet_Timeout(void);
extern void  Multi_Packet_Timeout(void);
extern void  Sensor_Bus_Function_Test(void);
extern uint8_t Update_Data_Logger(void);

extern uint32_t Reset_Reason;
//function defined in App.c
extern void LogResetReason(uint32_t rcc_csr);
extern uint32_t Get_Reset_Reason(void);
extern void Init_Task_Monitor(void);
extern void Update_Task_Runtime(Tasks_Enum_t task);
extern void Print_Task_Runtimes(void);
extern Tasks_Enum_t Get_Current_Task(void);

#include "Stats_Buffed.h"
extern void pmm_check_change_task(void);

#define APPN_WAIT_ENUM_MS		(1000*3)
#define APPN_CHECK_ENUM_MS		1000
#define APPN_NORMAL_WORK_MS		10

#include "eos/eos.h"
/*================================================================*/
#endif /* APP_1IMP_H_ */
