/*
 * scan.c
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */


#include "1imp.h"
/*================================================================*/
extern void Low_Level_App_Update(void);
extern void app_check_perf(void);
void pmg_module_bus_scan_task(void *in);
void pmg_sensor_bus_scan_task(void *in);
void pmg_panel1_bus_scan_task(void *in);
void pmg_panel2_bus_scan_task(void *in);
/*--------
 *
 * 	// Power Down Request Flag
	//////////////////////////////
	if(Power_Down_Request_Flag)
	{
		PowerControl_InitiateShutdown();
	}
 *
 * ----------------------------*/
void pmg_scan_task(void *in);
void pmg_scan_task(void *in)
{
//	Low_Level_App_Update();
	pmg_module_bus_scan_task(in);
	pmg_sensor_bus_scan_task(in);
	pmg_panel1_bus_scan_task(in);
	pmg_panel2_bus_scan_task(in);
	if(Power_Down_Request_Flag)
	{
		PowerControl_InitiateShutdown();
	}
	stats_task_send_records();
}
/*================================================================*/
/* end of scan.c */
