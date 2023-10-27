/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define USE_LEGACY_I2C_IMPLEMENTATION

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void _Error_Handler(char *file, int line);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BRIGHTNESS_PWM_COUNT 8399
#define TIMER_7_RELOAD_VALUE 65535
#define TIMER_14_RELOAD_VALUE 37500
#define ENUM_SLAVE_ADDR 0x7F
#define PWM_FREQ_DIVIDER_120HZ 74
#define ETH_TXD3_Pin GPIO_PIN_2
#define ETH_TXD3_GPIO_Port GPIOE
#define RADAR_nDISABLE_ANALOG_Pin GPIO_PIN_3
#define RADAR_nDISABLE_ANALOG_GPIO_Port GPIOE
#define ERROR_Pin GPIO_PIN_4
#define ERROR_GPIO_Port GPIOE
#define SLOT6_nRESET_Pin GPIO_PIN_5
#define SLOT6_nRESET_GPIO_Port GPIOE
#define SLOT6_nOE_Pin GPIO_PIN_6
#define SLOT6_nOE_GPIO_Port GPIOE
#define SLOT6_INT_Pin GPIO_PIN_8
#define SLOT6_INT_GPIO_Port GPIOI
#define SLOT6_INT_EXTI_IRQn EXTI9_5_IRQn
#define ALS_nINT_Pin GPIO_PIN_13
#define ALS_nINT_GPIO_Port GPIOC
#define ALS_nINT_EXTI_IRQn EXTI15_10_IRQn
#define UART4_RX_Pin GPIO_PIN_9
#define UART4_RX_GPIO_Port GPIOI
#define ETH_RX_ER_Pin GPIO_PIN_10
#define ETH_RX_ER_GPIO_Port GPIOI
#define STM_F767_nRST_L082_Pin GPIO_PIN_11
#define STM_F767_nRST_L082_GPIO_Port GPIOI
#define I2C2_STM_SDA_Pin GPIO_PIN_0
#define I2C2_STM_SDA_GPIO_Port GPIOF
#define I2C2_STM_SCL_Pin GPIO_PIN_1
#define I2C2_STM_SCL_GPIO_Port GPIOF
#define SLOT4_nOE_Pin GPIO_PIN_2
#define SLOT4_nOE_GPIO_Port GPIOF
#define SLOT4_INT_Pin GPIO_PIN_3
#define SLOT4_INT_GPIO_Port GPIOF
#define SLOT4_INT_EXTI_IRQn EXTI3_IRQn
#define SLOT4_nRESET_Pin GPIO_PIN_4
#define SLOT4_nRESET_GPIO_Port GPIOF
#define SYS_TEMP_Pin GPIO_PIN_5
#define SYS_TEMP_GPIO_Port GPIOF
#define UART7_STM_EXT_RX_Pin GPIO_PIN_6
#define UART7_STM_EXT_RX_GPIO_Port GPIOF
#define UART7_STM_EXT_TX_Pin GPIO_PIN_7
#define UART7_STM_EXT_TX_GPIO_Port GPIOF
#define BOOT0_PRGM_Pin GPIO_PIN_10
#define BOOT0_PRGM_GPIO_Port GPIOF
#define nIRQ_STM_RTC_Pin GPIO_PIN_0
#define nIRQ_STM_RTC_GPIO_Port GPIOC
#define nIRQ_STM_RTC_EXTI_IRQn EXTI0_IRQn
#define ETH_MDC_Pin GPIO_PIN_1
#define ETH_MDC_GPIO_Port GPIOC
#define ETH_TXD2_Pin GPIO_PIN_2
#define ETH_TXD2_GPIO_Port GPIOC
#define ETH_TX_CLK_Pin GPIO_PIN_3
#define ETH_TX_CLK_GPIO_Port GPIOC
#define ETH_RX_CLK_Pin GPIO_PIN_1
#define ETH_RX_CLK_GPIO_Port GPIOA
#define ETH_MDIO_Pin GPIO_PIN_2
#define ETH_MDIO_GPIO_Port GPIOA
#define ETH_CRS_Pin GPIO_PIN_2
#define ETH_CRS_GPIO_Port GPIOH
#define ETH_COL_Pin GPIO_PIN_3
#define ETH_COL_GPIO_Port GPIOH
#define LP_TRIGGER_Pin GPIO_PIN_4
#define LP_TRIGGER_GPIO_Port GPIOH
#define LP_TRIGGER_EXTI_IRQn EXTI4_IRQn
#define TRIGGER_Pin GPIO_PIN_5
#define TRIGGER_GPIO_Port GPIOH
#define USART2_CAM_RX_Pin GPIO_PIN_3
#define USART2_CAM_RX_GPIO_Port GPIOA
#define nKEY_SW2_Pin GPIO_PIN_4
#define nKEY_SW2_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_5
#define VBAT_GPIO_Port GPIOA
#define INPUT_VOLTAGE_Pin GPIO_PIN_6
#define INPUT_VOLTAGE_GPIO_Port GPIOA
#define ETH_RX_DV_Pin GPIO_PIN_7
#define ETH_RX_DV_GPIO_Port GPIOA
#define ETH_RXD0_Pin GPIO_PIN_4
#define ETH_RXD0_GPIO_Port GPIOC
#define ETH_RXD1_Pin GPIO_PIN_5
#define ETH_RXD1_GPIO_Port GPIOC
#define ETH_RXD2_Pin GPIO_PIN_0
#define ETH_RXD2_GPIO_Port GPIOB
#define ETH_RXD3_Pin GPIO_PIN_1
#define ETH_RXD3_GPIO_Port GPIOB
#define SPI3_LED1_MOSI_Pin GPIO_PIN_2
#define SPI3_LED1_MOSI_GPIO_Port GPIOB
#define SPI3_LED1_nSS_Pin GPIO_PIN_11
#define SPI3_LED1_nSS_GPIO_Port GPIOF
#define SLOT7_INT_Pin GPIO_PIN_12
#define SLOT7_INT_GPIO_Port GPIOF
#define SLOT7_INT_EXTI_IRQn EXTI15_10_IRQn
#define USB_COMM_PRSNT_Pin GPIO_PIN_13
#define USB_COMM_PRSNT_GPIO_Port GPIOF
#define I2C4_LED_SCL_Pin GPIO_PIN_14
#define I2C4_LED_SCL_GPIO_Port GPIOF
#define I2C4_LED_SDA_Pin GPIO_PIN_15
#define I2C4_LED_SDA_GPIO_Port GPIOF
#define SLOT7_nRESET_Pin GPIO_PIN_0
#define SLOT7_nRESET_GPIO_Port GPIOG
#define SLOT7_nOE_Pin GPIO_PIN_1
#define SLOT7_nOE_GPIO_Port GPIOG
#define SSL_WDI_Pin GPIO_PIN_7
#define SSL_WDI_GPIO_Port GPIOE
#define SLOT3_nOE_Pin GPIO_PIN_8
#define SLOT3_nOE_GPIO_Port GPIOE
#define RADAR_nDISABLE_ALL_Pin GPIO_PIN_9
#define RADAR_nDISABLE_ALL_GPIO_Port GPIOE
#define SLOT3_INT_Pin GPIO_PIN_10
#define SLOT3_INT_GPIO_Port GPIOE
#define SLOT3_INT_EXTI_IRQn EXTI15_10_IRQn
#define FIBER_ON_Pin GPIO_PIN_13
#define FIBER_ON_GPIO_Port GPIOE
#define PSET_nOFF_Pin GPIO_PIN_15
#define PSET_nOFF_GPIO_Port GPIOE
#define ETH_TX_EN_Pin GPIO_PIN_11
#define ETH_TX_EN_GPIO_Port GPIOB
#define nPDWN_RQST_Pin GPIO_PIN_6
#define nPDWN_RQST_GPIO_Port GPIOH
#define nPDWN_RQST_EXTI_IRQn EXTI9_5_IRQn
#define I2C3_LED_SCL_Pin GPIO_PIN_7
#define I2C3_LED_SCL_GPIO_Port GPIOH
#define I2C3_LED_SDA_Pin GPIO_PIN_8
#define I2C3_LED_SDA_GPIO_Port GPIOH
#define LED1_nOE_Pin GPIO_PIN_9
#define LED1_nOE_GPIO_Port GPIOH
#define LED2_nOE_Pin GPIO_PIN_11
#define LED2_nOE_GPIO_Port GPIOH
#define ETH_TXD0_Pin GPIO_PIN_12
#define ETH_TXD0_GPIO_Port GPIOB
#define USB_EXT_VBUS_Pin GPIO_PIN_13
#define USB_EXT_VBUS_GPIO_Port GPIOB
#define USB_HS_EXT_DM_Pin GPIO_PIN_14
#define USB_HS_EXT_DM_GPIO_Port GPIOB
#define USB_HS_EXT_DP_Pin GPIO_PIN_15
#define USB_HS_EXT_DP_GPIO_Port GPIOB
#define MPPT_nRST_Pin GPIO_PIN_8
#define MPPT_nRST_GPIO_Port GPIOD
#define MPPT_INT_OUT_Pin GPIO_PIN_9
#define MPPT_INT_OUT_GPIO_Port GPIOD
#define MPPT_INT_OUT_EXTI_IRQn EXTI9_5_IRQn
#define LED1_MOD_nRST_Pin GPIO_PIN_10
#define LED1_MOD_nRST_GPIO_Port GPIOD
#define LED1_INT_OUT_Pin GPIO_PIN_11
#define LED1_INT_OUT_GPIO_Port GPIOD
#define LED1_INT_OUT_EXTI_IRQn EXTI15_10_IRQn
#define LED_BLANK_Pin GPIO_PIN_12
#define LED_BLANK_GPIO_Port GPIOD
#define LED2_MOD_nRST_Pin GPIO_PIN_13
#define LED2_MOD_nRST_GPIO_Port GPIOD
#define LED2_INT_OUT_Pin GPIO_PIN_14
#define LED2_INT_OUT_GPIO_Port GPIOD
#define LED2_INT_OUT_EXTI_IRQn EXTI15_10_IRQn
#define SSL_nTAMPER_Pin GPIO_PIN_15
#define SSL_nTAMPER_GPIO_Port GPIOD
#define SLOT1_INT_Pin GPIO_PIN_2
#define SLOT1_INT_GPIO_Port GPIOG
#define SLOT1_INT_EXTI_IRQn EXTI2_IRQn
#define SLOT1_nRESET_Pin GPIO_PIN_3
#define SLOT1_nRESET_GPIO_Port GPIOG
#define SLOT1_nOE_Pin GPIO_PIN_4
#define SLOT1_nOE_GPIO_Port GPIOG
#define SSL_nWAKE_Pin GPIO_PIN_5
#define SSL_nWAKE_GPIO_Port GPIOG
#define SLOT2_nRESET_Pin GPIO_PIN_6
#define SLOT2_nRESET_GPIO_Port GPIOG
#define SLOT2_INT_Pin GPIO_PIN_7
#define SLOT2_INT_GPIO_Port GPIOG
#define SLOT2_INT_EXTI_IRQn EXTI9_5_IRQn
#define SLOT2_nOE_Pin GPIO_PIN_8
#define SLOT2_nOE_GPIO_Port GPIOG
#define USART6_TX_Pin GPIO_PIN_6
#define USART6_TX_GPIO_Port GPIOC
#define USART6_RX_Pin GPIO_PIN_7
#define USART6_RX_GPIO_Port GPIOC
#define SLOT3_nRESET_Pin GPIO_PIN_8
#define SLOT3_nRESET_GPIO_Port GPIOC
#define STM767_ANT_nEN_Pin GPIO_PIN_9
#define STM767_ANT_nEN_GPIO_Port GPIOC
#define USART1_CK_Pin GPIO_PIN_8
#define USART1_CK_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define USB_FS_DM_Pin GPIO_PIN_11
#define USB_FS_DM_GPIO_Port GPIOA
#define USB_FS_DP_Pin GPIO_PIN_12
#define USB_FS_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SSL_nRESET_Pin GPIO_PIN_13
#define SSL_nRESET_GPIO_Port GPIOH
#define STM_STATS_nSS_Pin GPIO_PIN_14
#define STM_STATS_nSS_GPIO_Port GPIOH
#define SPI2_RTC_nSS_Pin GPIO_PIN_15
#define SPI2_RTC_nSS_GPIO_Port GPIOH
#define SPI2_LED2_nSS_Pin GPIO_PIN_0
#define SPI2_LED2_nSS_GPIO_Port GPIOI
#define SPI2_LED2_SCLK_Pin GPIO_PIN_1
#define SPI2_LED2_SCLK_GPIO_Port GPIOI
#define SPI2_LED2_MISO_Pin GPIO_PIN_2
#define SPI2_LED2_MISO_GPIO_Port GPIOI
#define SPI2_LED2_MOSI_Pin GPIO_PIN_3
#define SPI2_LED2_MOSI_GPIO_Port GPIOI
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SPI2_SSL_nSS_Pin GPIO_PIN_15
#define SPI2_SSL_nSS_GPIO_Port GPIOA
#define USART3_STM_TX_Pin GPIO_PIN_10
#define USART3_STM_TX_GPIO_Port GPIOC
#define USART3_STM_RX_Pin GPIO_PIN_11
#define USART3_STM_RX_GPIO_Port GPIOC
#define SDMMC2_nCARD_DETECT_Pin GPIO_PIN_12
#define SDMMC2_nCARD_DETECT_GPIO_Port GPIOC
#define CAN1_RX_Pin GPIO_PIN_0
#define CAN1_RX_GPIO_Port GPIOD
#define CAN1_TX_Pin GPIO_PIN_1
#define CAN1_TX_GPIO_Port GPIOD
#define ALS_PWR_Pin GPIO_PIN_2
#define ALS_PWR_GPIO_Port GPIOD
#define CAN_BUS_RS_Pin GPIO_PIN_3
#define CAN_BUS_RS_GPIO_Port GPIOD
#define ANT_EN1_Pin GPIO_PIN_4
#define ANT_EN1_GPIO_Port GPIOD
#define USART2_CAM_TX_Pin GPIO_PIN_5
#define USART2_CAM_TX_GPIO_Port GPIOD
#define SDMMC2_CK_Pin GPIO_PIN_6
#define SDMMC2_CK_GPIO_Port GPIOD
#define SDMMC2_CMD_Pin GPIO_PIN_7
#define SDMMC2_CMD_GPIO_Port GPIOD
#define SDMMC2_D0_Pin GPIO_PIN_9
#define SDMMC2_D0_GPIO_Port GPIOG
#define SDMMC2_D1_Pin GPIO_PIN_10
#define SDMMC2_D1_GPIO_Port GPIOG
#define SDMMC2_D2_Pin GPIO_PIN_11
#define SDMMC2_D2_GPIO_Port GPIOG
#define SDMMC2_D3_Pin GPIO_PIN_12
#define SDMMC2_D3_GPIO_Port GPIOG
#define IM_STOLEN_Pin GPIO_PIN_13
#define IM_STOLEN_GPIO_Port GPIOG
#define ETH_TXD1_Pin GPIO_PIN_14
#define ETH_TXD1_GPIO_Port GPIOG
#define FIBER_INT_Pin GPIO_PIN_15
#define FIBER_INT_GPIO_Port GPIOG
#define FIBER_INT_EXTI_IRQn EXTI15_10_IRQn
#define SPI3_LED1_SCLK_Pin GPIO_PIN_3
#define SPI3_LED1_SCLK_GPIO_Port GPIOB
#define SPI3_LED1_MISO_Pin GPIO_PIN_4
#define SPI3_LED1_MISO_GPIO_Port GPIOB
#define RADAR_SPEED_RX_Pin GPIO_PIN_5
#define RADAR_SPEED_RX_GPIO_Port GPIOB
#define I2C1_SLOT_SCL_Pin GPIO_PIN_6
#define I2C1_SLOT_SCL_GPIO_Port GPIOB
#define I2C1_SLOT_SDA_Pin GPIO_PIN_7
#define I2C1_SLOT_SDA_GPIO_Port GPIOB
#define PWER_INT_Pin GPIO_PIN_8
#define PWER_INT_GPIO_Port GPIOB
#define RADAR_SPEED_TX_Pin GPIO_PIN_9
#define RADAR_SPEED_TX_GPIO_Port GPIOB
#define RADAR_STATS_RX_Pin GPIO_PIN_0
#define RADAR_STATS_RX_GPIO_Port GPIOE
#define RADAR_STATS_TX_Pin GPIO_PIN_1
#define RADAR_STATS_TX_GPIO_Port GPIOE
#define AES_nTLS_MODE_Pin GPIO_PIN_4
#define AES_nTLS_MODE_GPIO_Port GPIOI
#define SLOT5_INT_Pin GPIO_PIN_5
#define SLOT5_INT_GPIO_Port GPIOI
#define SLOT5_INT_EXTI_IRQn EXTI9_5_IRQn
#define SLOT5_nRESET_Pin GPIO_PIN_6
#define SLOT5_nRESET_GPIO_Port GPIOI
#define SLOT5_nOE_Pin GPIO_PIN_7
#define SLOT5_nOE_GPIO_Port GPIOI
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
