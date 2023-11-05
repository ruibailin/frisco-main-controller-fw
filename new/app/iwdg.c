/*
 * iwdg.c
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */


#include "1imp.h"
/*================================================================*/
typedef enum
{
	IWDG_INIT_STATE	= 0,
	IWDG_ENUM_STATE,
	IWDG_WORK_STATE
}Main_Machine_States;
/*------------------------------------*/
extern void MX_IWDG_Init(void);
extern void MX_IWDG_Refresh(void);
void pmg_iwdg_task(void *in);
void pmg_iwdg_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case IWDG_INIT_STATE:
		MX_IWDG_Init();
		eos_set_timer(1000);
		eos_set_state(IWDG_WORK_STATE);
		break;
	case IWDG_ENUM_STATE:
		break;
	case IWDG_WORK_STATE:
		MX_IWDG_Refresh();
		eos_set_timer(1000);
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of iwdg.c */
