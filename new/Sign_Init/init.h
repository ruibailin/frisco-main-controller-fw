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
extern int sign_init_Start(int sinit_state);
extern int sign_init_Init_Password(int sinit_state);
extern int sign_init_CommChipInit(int sinit_state);
extern int sign_init_RS232Init(int sinit_state);
extern int sign_init_InitSensorBus(int sinit_state);
extern int sign_init_InitAppData(int sinit_state);
extern int sign_init_InituSD(int sinit_state);
extern int sign_init_InitFrisco(int sinit_state);
extern int sign_init_MPPTInit(int sinit_state);
extern int sign_init_InitFriscoCommBus(int sinit_state);
extern int sign_init_InitCamera(int sinit_state);
extern int sign_init_InitDisplay(int sinit_state);
extern int sign_init_InitLightSensorManager(int sinit_state);
extern int sign_init_InitSignState(int sinit_state);
extern int sign_init_TurnOnRadarPower(int sinit_state);
extern int sign_init_RtcInit(int sinit_state);
extern int sign_init_InitTempSensor(int sinit_state);
extern int sign_init_Ymodem_Init(int sinit_state);
extern int sign_init_StartEnumerations(int sinit_state);
extern int sign_init_StartModuleBusEnumeration(int sinit_state);
extern int sign_init_ModuleBusEnum(int sinit_state);
extern int sign_init_StartPanelEnumeration(int sinit_state);
extern int sign_init_PanelBusEnum(int sinit_state);
extern int sign_init_WaitForPanelBusEnumComplete(int sinit_state);
extern int sign_init_PanelEnumTimedOut(int sinit_state);
extern int sign_init_StartCommChipEnum(int sinit_state);
extern int sign_init_CommChipEnum(int sinit_state);
extern int sign_init_StartMPPTEnumeration(int sinit_state);
extern int sign_init_TestForMPPTPresence(int sinit_state);
extern int sign_init_MPPTEnum(int sinit_state);
extern int sign_init_MPPT_Enumeration_Complete(int sinit_state);
extern int sign_init_EnumerationsComplete(int sinit_state);
extern int sign_init_ClearDisplays(int sinit_state);
extern int sign_init_InitData_Log(int sinit_state);
extern int sign_init_DoStartupFlagChecks(int sinit_state);
extern int sign_init_Module_Post_Enum_Configuration(int sinit_state);
extern int sign_init_NTCIP_Init(int sinit_state);
extern int sign_init_Init_Sched_FW_Install(int sinit_state);
extern int sign_init_SendStartupNotification(int sinit_state);
extern int sign_init_InitComplete(int sinit_state);
extern int sign_init_RunControllerApp(int sinit_state);
extern int sign_init_is_end(int sinit_state);
/*------------------------------------*/
extern void sign_init_end(void);
/*================================================================*/
/* end of init.c */



/*================================================================*/
#endif /* SIGN_INIT_INIT_H_ */
