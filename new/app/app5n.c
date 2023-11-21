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
	APP5_INIT_STATE	= 0,
	APP5_ENUM_STATE,
	APP5_WORK_STATE
}App5_Machine_States;
#define APP5_WAIT_ENUM_MS		(1000*3)
#define APP5_CHECK_ENUM_MS		1000
#define APP5_NORMAL_WORK_MS		1000
extern uint8_t	Firmware_Install_Active_Flag;
/*------------------------------------*/
void pmg_app50_task(void *in);
void pmg_app50_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
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
	case APP5_INIT_STATE:
		eos_set_timer(APP5_WAIT_ENUM_MS);
		eos_set_state(APP5_ENUM_STATE);
		break;
	case APP5_ENUM_STATE:
		eos_set_timer(APP5_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(APP5_WORK_STATE);
		break;
	case APP5_WORK_STATE:
		eos_set_timer(APP5_NORMAL_WORK_MS);
		if(Firmware_Install_Active_Flag)
			break;
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of app5n.c */
