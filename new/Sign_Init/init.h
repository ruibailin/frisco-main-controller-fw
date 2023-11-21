/*
 * init.h
 *
 *  Created on: Nov 10, 2023
 *      Author: stever
 */

#ifndef SIGN_INIT_INIT_H_
#define SIGN_INIT_INIT_H_
#include "1imp.h"
/*================================================================*/
extern void sign_wait_init(void);

extern int sign_init_Start(int sinit_state);		//44 Module_Bus_Tasks();
extern int sign_init_Init_Password(int sinit_state);	//56 pmg_appn_task	NTCIP
extern int sign_init_CommChipInit(int sinit_state);		//56 pmg_appn_task
extern int sign_init_RS232Init(int sinit_state);		//36 External_RS232_Tasks();
extern int sign_init_InitSensorBus(int sinit_state);	//47 Sensor_Bus_Tasks();
extern int sign_init_InitAppData(int sinit_state);		//35 Reset_Sign();
extern int sign_init_InituSD(int sinit_state);			//17 Stats_Port_Tasks();
extern int sign_init_InitFrisco(int sinit_state);		//12 Process_Radar_Messages();
extern int sign_init_MPPTInit(int sinit_state);			//14 Power_Manager_Update();
extern int sign_init_InitFriscoCommBus(int sinit_state);	//12 Process_Radar_Messages();
extern int sign_init_InitCamera(int sinit_state);		//56 pmg_appn_task
extern int sign_init_InitDisplay(int sinit_state);		//28 ret=Display_Update_Thread();
extern int sign_init_InitLightSensorManager(int sinit_state);	//18 Update_Light_Sensor_Manager();
extern int sign_init_InitSignState(int sinit_state);	//25 UpdateSignState();
extern int sign_init_TurnOnRadarPower(int sinit_state); //12 Process_Radar_Messages();
extern int sign_init_RtcInit(int sinit_state);			//27 RtcTask();
extern int sign_init_InitTempSensor(int sinit_state);	//23 ADC_Sensor_Tasks();
extern int sign_init_Ymodem_Init(int sinit_state);		//29 YmodemRun();
extern int sign_init_StartEnumerations(int sinit_state);	//56 pmg_appn_task
extern int sign_init_StartModuleBusEnumeration(int sinit_state);	//44 Module_Bus_Tasks();
extern int sign_init_ModuleBusEnum(int sinit_state);				//44 Module_Bus_Tasks();
extern int sign_init_StartPanelEnumeration(int sinit_state);		//45 Panel_Bus_1_Tasks();
extern int sign_init_PanelBusEnum(int sinit_state);					//45 Panel_Bus_1_Tasks();
extern int sign_init_WaitForPanelBusEnumComplete(int sinit_state);	//45 Panel_Bus_1_Tasks();
extern int sign_init_PanelEnumTimedOut(int sinit_state);			//45 Panel_Bus_1_Tasks();
extern int sign_init_StartCommChipEnum(int sinit_state);			//56 pmg_appn_task
extern int sign_init_CommChipEnum(int sinit_state);					//56 pmg_appn_task
extern int sign_init_StartMPPTEnumeration(int sinit_state);			//47 Sensor_Bus_Tasks();
extern int sign_init_TestForMPPTPresence(int sinit_state);			//47 Sensor_Bus_Tasks();
extern int sign_init_MPPTEnum(int sinit_state);						//47 Sensor_Bus_Tasks();
extern int sign_init_MPPT_Enumeration_Complete(int sinit_state);	//47 Sensor_Bus_Tasks();
extern int sign_init_EnumerationsComplete(int sinit_state);			//56 pmg_appn_task
extern int sign_init_ClearDisplays(int sinit_state);				//28 ret=Display_Update_Thread();
extern int sign_init_InitData_Log(int sinit_state);					//31 Update_Data_Logger();
extern int sign_init_DoStartupFlagChecks(int sinit_state);			//56 pmg_appn_task
extern int sign_init_Module_Post_Enum_Configuration(int sinit_state);	//15 Do_GPIO_Update();
extern int sign_init_NTCIP_Init(int sinit_state);					//56 pmg_appn_task
extern int sign_init_Init_Sched_FW_Install(int sinit_state);		//56 pmg_appn_task
extern int sign_init_SendStartupNotification(int sinit_state);		//56 pmg_appn_task
extern int sign_init_InitComplete(int sinit_state);					//56 pmg_appn_task
extern int sign_init_RunControllerApp(int sinit_state);				//56 pmg_appn_task
extern int sign_init_is_end(int sinit_state);
/*------------------------------------*/
extern void sign_init_end(void);									//59 pmg_main_task
/*================================================================*/
#endif /* SIGN_INIT_INIT_H_ */
