/*
 * main.c
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */


#include "1imp.h"
/*================================================================*/
typedef enum
{
	MAIN_INIT_STATE	= 0,
	MAIN_ENUM_STATE,
	MAIN_WORK_STATE
}Main_Machine_States;
/*------------------------------------*/
void pmg_main_task(void *in);
void pmg_main_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case MAIN_INIT_STATE:
		eos_set_timer(1000);
		eos_set_state(MAIN_WORK_STATE);
		break;
	case MAIN_ENUM_STATE:
		break;
	case MAIN_WORK_STATE:
		eos_set_timer(1000);
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of main.c */
