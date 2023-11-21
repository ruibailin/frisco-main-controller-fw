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
	APPN_INIT_STATE	= 0,
	APPN_ENUM_STATE,
	APPN_WORK_STATE = APPN_ENUM_STATE+20
}Appn_Machine_States;
/*------------------------------------*/
extern void RunApplication(void);
void pmg_appn_task(void *in);
void pmg_appn_task(void *in)
{
	int ss;
	int state;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+0);
		break;
	case APPN_ENUM_STATE+0:
		state=sign_init_Init_Password(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+1);
		break;
	case APPN_ENUM_STATE+1:
		state=sign_init_CommChipInit(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+2);
		break;
	case APPN_ENUM_STATE+2:
		state=sign_init_InitCamera(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+3);
		break;
	case APPN_ENUM_STATE+3:
		state=sign_init_StartEnumerations(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+4);
		break;
	case APPN_ENUM_STATE+4:
		state=sign_init_StartCommChipEnum(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+5);
		break;
	case APPN_ENUM_STATE+5:
		state=sign_init_CommChipEnum(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+6);
		break;
	case APPN_ENUM_STATE+6:
		state=sign_init_EnumerationsComplete(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+7);
		break;
	case APPN_ENUM_STATE+7:
		state=sign_init_DoStartupFlagChecks(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+8);
		break;
	case APPN_ENUM_STATE+8:
		state=sign_init_NTCIP_Init(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+9);
		break;
	case APPN_ENUM_STATE+9:
		state=sign_init_Init_Sched_FW_Install(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+10);
		break;
	case APPN_ENUM_STATE+10:
		state=sign_init_SendStartupNotification(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+11);
		break;
	case APPN_ENUM_STATE+11:
		state=sign_init_InitComplete(sinit_state);
		if(state == sinit_state)
			break;
		sinit_state = state;
		sign_wait_init();
		eos_set_state(APPN_ENUM_STATE+12);
		break;
	case APPN_ENUM_STATE+12:
		sign_init_end();
		eos_set_timer(APPN_NORMAL_WORK_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
//		RunApplication();
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of appn.c */
