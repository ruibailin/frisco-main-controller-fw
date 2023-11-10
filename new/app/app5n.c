/*
 * app5n.c
 *
 *  Created on: 09/11/2/23
 *      Author: Steve Rui
 */


#include "1imp.h"
/*================================================================*/
typedef enum
{
	APPN_INIT_STATE	= 0,
	APPN_ENUM_STATE,
	APPN_WORK_STATE
}App1_Machine_States;
extern uint8_t	Firmware_Install_Active_Flag;
/*------------------------------------*/
void pmg_app50_task(void *in);
void pmg_app50_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}


/*------------------------------------*/
void pmg_app51_task(void *in);
void pmg_app51_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}

/*------------------------------------*/
void pmg_app52_task(void *in);
void pmg_app52_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app53_task(void *in);
void pmg_app53_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app54_task(void *in);
void pmg_app54_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app55_task(void *in);
void pmg_app55_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app56_task(void *in);
void pmg_app56_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app57_task(void *in);
void pmg_app57_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app58_task(void *in);
void pmg_app58_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(10);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_ENUM_STATE:
		break;
	case APPN_WORK_STATE:
		eos_set_timer(10);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*------------------------------------*/
void pmg_app59_task(void *in);
void pmg_app59_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APPN_INIT_STATE:
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(APPN_ENUM_STATE);
		break;
	case APPN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		eos_set_state(APPN_WORK_STATE);
		break;
	case APPN_WORK_STATE:
		eos_set_timer(APPN_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of app5n.c */
