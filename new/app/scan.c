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
extern void sta_main_init(void);
extern void sta_main_loop(void);
void pmg_scan_task(void *in);
void pmg_scan_task(void *in)
{
//	Low_Level_App_Update();
	int ss;
	ss = eos_get_state();
	if(ss==0)
	{
		sta_main_init();
		eos_set_state(1);
		return;
	}
	pmg_module_bus_scan_task(in);
	pmg_sensor_bus_scan_task(in);
	pmg_panel1_bus_scan_task(in);
	pmg_panel2_bus_scan_task(in);
	if(Power_Down_Request_Flag)
	{
		PowerControl_InitiateShutdown();
	}
	stats_task_send_records();
	sta_main_loop();
}
/*================================================================*/
/* end of scan.c */
