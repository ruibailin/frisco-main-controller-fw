/*
 * 1imp.h
 *
 *  Created on: Mar 28, 2024
 *      Author: stever
 */

#ifndef FRISCO_MAIN_CONTROLLER_FW_DRIVERS_ACI_MODULE_DRIVER_1IMP_H_
#define FRISCO_MAIN_CONTROLLER_FW_DRIVERS_ACI_MODULE_DRIVER_1IMP_H_
/*================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "main.h"
#include "stm32f7xx_hal.h"
#include "RingBuf8.h"
#include "MsgQ.h"
#include "Module_Bus.h"
#include "Module_Driver.h"
#include "Testpoints.h"
#include "I2C_Bus_Master.h"
#include "Debug_Config.h"
#include "IMsgQueue.h"
#include "Debug_Flags.h"
#include "I2C_Bus_Watchdog.h"
#include "Debug_Log_Msg.h"
/*------------------------------------*/
#if defined(LOW_LEVEL_I2C_DEBUG) || defined(PASSIVE_LOW_LEVEL_I2C_DEBUG)
#define PS(c)	RB8_PushOverwrite(&State_buf, c)
#else
#define PS(c)
#endif


extern MODULE_DRIVER Module_Driver;
extern MODULE_BUS Module_Bus;

extern RingBuf8 State_buf;

#define MAX_NUM_RX_MESSAGES		24
#define MAX_NUM_TX_MESSAGES		32

#define I2C_MSG_BASE_SIZE	4

/*------------------------------------*/
#define REG_I2C_CR1_INIT	(I2C_CR1_PE | I2C_CR1_RXIE | I2C_CR1_ADDRIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE | I2C_CR1_ERRIE)
#define REG_I2C_TIMEOUTR_INIT	(0)
#define REG_I2C_TIMINGR_INIT	0x007074AF // 0x00909FCE // ZGy_Note
#define STATE_TIMEOUT_MS		200 // 50 // Fix the time out issue
#define MESSAGE_HOLD_TIMEOUT_MS	100
#define MESSAGE_HOLD_TIME		7
#define MIN_IDLE_DELAY_MS		3
#define TASK_PASS_TIMEOUT		1000 // 20 // ZGy_Note

/*------------------------------------*/
extern void SetState(I2C_Bus_Master *bm, I2C_BUS_STATE state);
extern void Restore_I2C_Hal(I2C_Bus_Master *bm);
extern void Reset_I2C_Peripheral(I2C_Bus_Master *bm);
extern void Reset_I2C_Bus(I2C_Bus_Master *bm);
bool Is_I2C_Bus_Locked_Up(I2C_Bus_Master *bm);
bool Is_I2C_Bus_Idle(I2C_Bus_Master *bm);
/*------------------------------------*/
extern I2C_Bus_Master Module_Bus_Master;
extern I2C_Bus_Master Sensor_Bus_Master;
extern I2C_Bus_Master Panel_Bus_2_Master;
extern I2C_Bus_Master Panel_Bus_1_Master;
/*================================================================*/
#endif /* FRISCO_MAIN_CONTROLLER_FW_DRIVERS_ACI_MODULE_DRIVER_1IMP_H_ */
