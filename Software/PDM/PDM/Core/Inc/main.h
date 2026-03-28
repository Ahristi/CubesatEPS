/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_2
#define LED0_GPIO_Port GPIOE
#define BMS_nCE_Pin GPIO_PIN_3
#define BMS_nCE_GPIO_Port GPIOE
#define BMS_nQON_Pin GPIO_PIN_4
#define BMS_nQON_GPIO_Port GPIOE
#define BMS_nSTAT_Pin GPIO_PIN_5
#define BMS_nSTAT_GPIO_Port GPIOE
#define BMS_nINT_Pin GPIO_PIN_6
#define BMS_nINT_GPIO_Port GPIOE
#define _12V_CH2_IMON_Pin GPIO_PIN_0
#define _12V_CH2_IMON_GPIO_Port GPIOC
#define HEATER1_IMON_Pin GPIO_PIN_1
#define HEATER1_IMON_GPIO_Port GPIOC
#define HEATER2_IMON_Pin GPIO_PIN_2
#define HEATER2_IMON_GPIO_Port GPIOC
#define _5V5_VMON_Pin GPIO_PIN_0
#define _5V5_VMON_GPIO_Port GPIOA
#define _12V_VMON_Pin GPIO_PIN_1
#define _12V_VMON_GPIO_Port GPIOA
#define _12V_CH1_IMON_Pin GPIO_PIN_2
#define _12V_CH1_IMON_GPIO_Port GPIOA
#define _5V_CH1_IMON_Pin GPIO_PIN_3
#define _5V_CH1_IMON_GPIO_Port GPIOA
#define _6V_CH1_IMON_Pin GPIO_PIN_4
#define _6V_CH1_IMON_GPIO_Port GPIOA
#define _5V_CH2_IMON_Pin GPIO_PIN_5
#define _5V_CH2_IMON_GPIO_Port GPIOA
#define _3V3_CH2_IMON_Pin GPIO_PIN_6
#define _3V3_CH2_IMON_GPIO_Port GPIOA
#define _3V3_CH1_IMON_Pin GPIO_PIN_7
#define _3V3_CH1_IMON_GPIO_Port GPIOA
#define _5V_CH2_nFLT_Pin GPIO_PIN_4
#define _5V_CH2_nFLT_GPIO_Port GPIOC
#define _5V_CH2_EN_Pin GPIO_PIN_5
#define _5V_CH2_EN_GPIO_Port GPIOC
#define _3V3_VMON_Pin GPIO_PIN_0
#define _3V3_VMON_GPIO_Port GPIOB
#define _5V_VMON_Pin GPIO_PIN_1
#define _5V_VMON_GPIO_Port GPIOB
#define _6V_CONV_EN_Pin GPIO_PIN_2
#define _6V_CONV_EN_GPIO_Port GPIOB
#define _12V_CH2_nFLT_Pin GPIO_PIN_9
#define _12V_CH2_nFLT_GPIO_Port GPIOE
#define _12V_CH2_EN_Pin GPIO_PIN_10
#define _12V_CH2_EN_GPIO_Port GPIOE
#define _6V_CH1_EN_Pin GPIO_PIN_11
#define _6V_CH1_EN_GPIO_Port GPIOE
#define _6V_CH1_PG_Pin GPIO_PIN_12
#define _6V_CH1_PG_GPIO_Port GPIOE
#define _5V_CH1_nFLT_Pin GPIO_PIN_13
#define _5V_CH1_nFLT_GPIO_Port GPIOE
#define _5V_CH1_EN_Pin GPIO_PIN_14
#define _5V_CH1_EN_GPIO_Port GPIOE
#define _3V3_CH2_nFLT_Pin GPIO_PIN_8
#define _3V3_CH2_nFLT_GPIO_Port GPIOD
#define _3V3_CH2_EN_Pin GPIO_PIN_9
#define _3V3_CH2_EN_GPIO_Port GPIOD
#define _3V3_CH1_nFLT_Pin GPIO_PIN_10
#define _3V3_CH1_nFLT_GPIO_Port GPIOD
#define _3V3_CH1_EN_Pin GPIO_PIN_11
#define _3V3_CH1_EN_GPIO_Port GPIOD
#define CAN_SHDN_Pin GPIO_PIN_12
#define CAN_SHDN_GPIO_Port GPIOD
#define CAN_STB_Pin GPIO_PIN_13
#define CAN_STB_GPIO_Port GPIOD
#define _5V_CONV_EN_Pin GPIO_PIN_14
#define _5V_CONV_EN_GPIO_Port GPIOD
#define HEATER2_PWM_Pin GPIO_PIN_6
#define HEATER2_PWM_GPIO_Port GPIOC
#define HEATER1_PWM_Pin GPIO_PIN_7
#define HEATER1_PWM_GPIO_Port GPIOC
#define PANEL_PG3_Pin GPIO_PIN_15
#define PANEL_PG3_GPIO_Port GPIOA
#define PANEL_EN3_Pin GPIO_PIN_12
#define PANEL_EN3_GPIO_Port GPIOC
#define PANEL_PG2_Pin GPIO_PIN_0
#define PANEL_PG2_GPIO_Port GPIOD
#define PANEL_EN2_Pin GPIO_PIN_1
#define PANEL_EN2_GPIO_Port GPIOD
#define PANEL_PG1_Pin GPIO_PIN_2
#define PANEL_PG1_GPIO_Port GPIOD
#define PANEL_EN1_Pin GPIO_PIN_3
#define PANEL_EN1_GPIO_Port GPIOD
#define PANEL_EN0_Pin GPIO_PIN_4
#define PANEL_EN0_GPIO_Port GPIOD
#define PANEL_PG0_Pin GPIO_PIN_5
#define PANEL_PG0_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOB
#define BMS_SCL_Pin GPIO_PIN_6
#define BMS_SCL_GPIO_Port GPIOB
#define BMS_SDA_Pin GPIO_PIN_7
#define BMS_SDA_GPIO_Port GPIOB
#define _12V_CONV_EN_Pin GPIO_PIN_8
#define _12V_CONV_EN_GPIO_Port GPIOB
#define _12V_CH1_EN_Pin GPIO_PIN_0
#define _12V_CH1_EN_GPIO_Port GPIOE
#define _12V_CH1_nFLT_Pin GPIO_PIN_1
#define _12V_CH1_nFLT_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
