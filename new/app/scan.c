/*
 * scan.c
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */


#include "1imp.h"
/*================================================================*/
extern void Low_Level_App_Update(void);
/*------------------------------------*/
void pmg_scan_task(void *in);
void pmg_scan_task(void *in)
{
	Low_Level_App_Update();
}
/*================================================================*/
/* end of scan.c */
