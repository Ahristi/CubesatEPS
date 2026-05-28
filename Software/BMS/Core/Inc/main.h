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
#define nACOK_Pin GPIO_PIN_0
#define nACOK_GPIO_Port GPIOC
#define nCHG_OK_Pin GPIO_PIN_1
#define nCHG_OK_GPIO_Port GPIOC
#define nCHG_EN_Pin GPIO_PIN_2
#define nCHG_EN_GPIO_Port GPIOC
#define BAT_SHDN_Pin GPIO_PIN_5
#define BAT_SHDN_GPIO_Port GPIOC
#define BAT_PG_Pin GPIO_PIN_10
#define BAT_PG_GPIO_Port GPIOB
#define UMB_SHDN_Pin GPIO_PIN_12
#define UMB_SHDN_GPIO_Port GPIOB
#define UMB_PG_Pin GPIO_PIN_13
#define UMB_PG_GPIO_Port GPIOB
#define BMS_nCE_Pin GPIO_PIN_9
#define BMS_nCE_GPIO_Port GPIOA
#define BMS_nSTAT_Pin GPIO_PIN_10
#define BMS_nSTAT_GPIO_Port GPIOA
#define BMS_nQON_Pin GPIO_PIN_11
#define BMS_nQON_GPIO_Port GPIOA
#define BMS_nINT_Pin GPIO_PIN_12
#define BMS_nINT_GPIO_Port GPIOA
#define CHG_PG_Pin GPIO_PIN_8
#define CHG_PG_GPIO_Port GPIOB
#define CHG_SHDN_Pin GPIO_PIN_9
#define CHG_SHDN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
