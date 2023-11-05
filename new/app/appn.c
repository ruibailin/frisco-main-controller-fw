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
	APPN_WORK_STATE
}Main_Machine_States;
/*------------------------------------*/
extern void RunApplication(void);
void pmg_appn_task(void *in);
void pmg_appn_task(void *in)
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
		RunApplication();
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of appn.c */
