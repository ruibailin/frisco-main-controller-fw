/*
 * main.c
 *
 *  Created on: 2023年11月5日
 *      Author: code
 */


#include "1imp.h"
/*================================================================*/
#include "FW_Version.h"
#include "util.h"
#include "App.h"
#include "Digit_Display.h"
#include "Testpoints.h"
#include "Parameter_Map.h"
#include "Registration_Data.h"
#include "UART_Driver.h"
#include "Debug_Config.h"
#include "EEPROM/CAT24M01.h"
#include "Module_UART.h"
#include "Debug_Log_Msg.h"
static void main_log_register(void);
static void main_log_register()
{
	  {
		  formatlog("Main_Controller","Initialization","Init Reg Data\r\n");
		  uint8_t Registrataion_Data_Init_Retries = 5;
		  while(!Init_Registration_Data())
		  {
			  if(Registrataion_Data_Init_Retries-- == 0)
			  {
				  formatlog("Main_Controller","Initialization","Giving up on initializing registration data.  Resetting\r\n");

				  _Error_Handler(__FILE__, __LINE__);
			  }
		  }

		  // Force str to be destroyed after logging.
		  char str[16];
		  formatlog("Main_Controller", "Mod_Info","Frisco Controller\r\n");
		  sprintf(str, "%d.%d.%d.%d", MAJOR_FW_VERSION, MINOR_FW_VERSION, SUB_FW_VERSION, TEST_FW_VERSION);
		  formatlog("Main_Controller", "Mod_Info","Firmware Revision %s\r\n",str);
		  formatlog("Main_Controller", "Mod_Info","Serial Number %8s\r\n",Regn_Data_Image.SerNum);
	  }
#ifdef DISPLAY_PANEL_15_INCH
  formatlog("Main_Controller", "Mod_Info","15 INCH\r\n");
#elif defined(DISPLAY_PANEL_18_INCH)
  formatlog("Main_Controller", "Mod_Info","18 INCH\r\n");
#else
  formatlog("Main_Controller", "Mod_Info","12 INCH\r\n");
#endif
  formatlog("Main_Controller", "Mod_Info","Parameter Map Major Version: 0x%08X\r\n", PARAM_MAP_VERSION_MAJOR);
  formatlog("Main_Controller", "Mod_Info","Parameter Map Incremental Version:  0x%08X\r\n", PARAM_MAP_VERSION_INCR);
  formatlog("Main_Controller", "Initialization","Init Complete\r\n");
}
extern uint8_t RunHardwareTests(uint8_t TestLevel);
static void main_hw_test(void);
static void main_hw_test()
{
	// If Hardware tests faile, loop
	for(uint8_t i=0; RunHardwareTests(1) == 0; ++i)
	{
		  if(i < 10)
		  {
			  HAL_Delay(1000);
			  formatlog("Main_Controller","Initialization","Retrying Hardware Tests\r\n");
		  }
		  else
		  {
			  formatlog("Main_Controller","Initialization","Critical System Failure--Shutting Down!\r\n");
			  exit(1);
		  }
	}
}
/*------------------------------------*/
#include <stdint.h>
extern void Init_Debug_Configs(void);
extern void Init_Frisco_Comm_UART(void);
extern void Init_Frisco_Module_UART(void);
extern void Read_Back_Messages(void);
extern uint32_t reg_rcc_csr_copy;
extern void LogResetReason(uint32_t rcc_csr);

extern void Log_Watchdog_Count(void);
extern void Set_Port_Resets(uint8_t portmask);
extern void InitApplication(void);
extern void Init_Task_Monitor(void);
/*------------------------------------*/
typedef enum
{
	MAIN_INIT_STATE	= 0,
	MAIN_ENUM_STATE,
	MAIN_WORK_STATE
}Main_Machine_States;
/*------------------------------------*/
#include "main.h"
void pmg_main_task(void *in);
void pmg_main_task(void *in)
{
	int ss;
	ss=eos_get_state();
	switch(ss)
	{
	case MAIN_INIT_STATE:
		Init_Debug_Configs();
		Init_Frisco_Comm_UART();
		Init_Frisco_Module_UART();
		HAL_GPIO_WritePin(STM_F767_nRST_L082_GPIO_Port, STM_F767_nRST_L082_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(FIBER_ON_GPIO_Port, FIBER_ON_Pin, GPIO_PIN_SET);
		Read_Back_Messages();
		main_log_register();
		HAL_Delay(20);
		LogResetReason(reg_rcc_csr_copy);
		Log_Watchdog_Count();

		main_hw_test();
		Set_Port_Resets(0xff);
//		InitApplication();
		Init_Task_Monitor();
		eos_set_timer(APPN_WAIT_ENUM_MS);
		eos_set_state(MAIN_ENUM_STATE);
		break;
	case MAIN_ENUM_STATE:
		eos_set_timer(APPN_CHECK_ENUM_MS);
		int ret;
		ret=sign_init_is_end(sinit_state);
		if(!ret)
			break;
		eos_set_state(MAIN_WORK_STATE);
		break;
	case MAIN_WORK_STATE:
		//eos_set_timer(APPN_NORMAL_WORK_MS*100);
		break;
	default:
		break;
	}
}
/*================================================================*/
/* end of main.c */
