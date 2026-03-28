/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PC6   ------> S_TIM3_CH1
     PC7   ------> S_TIM3_CH2
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, BMS_nCE_Pin|BMS_nQON_Pin|_12V_CH2_EN_Pin|_6V_CH1_EN_Pin
                          |_5V_CH1_EN_Pin|_12V_CH1_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, _5V_CH2_EN_Pin|PANEL_EN3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, _6V_CONV_EN_Pin|LED1_Pin|_12V_CONV_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, _3V3_CH2_EN_Pin|_3V3_CH1_EN_Pin|CAN_SHDN_Pin|CAN_STB_Pin
                          |_5V_CONV_EN_Pin|PANEL_EN2_Pin|PANEL_EN1_Pin|PANEL_EN0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED0_Pin */
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BMS_nCE_Pin BMS_nQON_Pin _12V_CH2_EN_Pin _6V_CH1_EN_Pin
                           _5V_CH1_EN_Pin _12V_CH1_EN_Pin */
  GPIO_InitStruct.Pin = BMS_nCE_Pin|BMS_nQON_Pin|_12V_CH2_EN_Pin|_6V_CH1_EN_Pin
                          |_5V_CH1_EN_Pin|_12V_CH1_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BMS_nSTAT_Pin BMS_nINT_Pin */
  GPIO_InitStruct.Pin = BMS_nSTAT_Pin|BMS_nINT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : _5V_CH2_nFLT_Pin */
  GPIO_InitStruct.Pin = _5V_CH2_nFLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(_5V_CH2_nFLT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : _5V_CH2_EN_Pin PANEL_EN3_Pin */
  GPIO_InitStruct.Pin = _5V_CH2_EN_Pin|PANEL_EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : _6V_CONV_EN_Pin _12V_CONV_EN_Pin */
  GPIO_InitStruct.Pin = _6V_CONV_EN_Pin|_12V_CONV_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : _12V_CH2_nFLT_Pin _6V_CH1_PG_Pin _5V_CH1_nFLT_Pin _12V_CH1_nFLT_Pin */
  GPIO_InitStruct.Pin = _12V_CH2_nFLT_Pin|_6V_CH1_PG_Pin|_5V_CH1_nFLT_Pin|_12V_CH1_nFLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : _3V3_CH2_nFLT_Pin _3V3_CH1_nFLT_Pin */
  GPIO_InitStruct.Pin = _3V3_CH2_nFLT_Pin|_3V3_CH1_nFLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : _3V3_CH2_EN_Pin _3V3_CH1_EN_Pin CAN_SHDN_Pin CAN_STB_Pin
                           _5V_CONV_EN_Pin PANEL_EN2_Pin PANEL_EN1_Pin PANEL_EN0_Pin */
  GPIO_InitStruct.Pin = _3V3_CH2_EN_Pin|_3V3_CH1_EN_Pin|CAN_SHDN_Pin|CAN_STB_Pin
                          |_5V_CONV_EN_Pin|PANEL_EN2_Pin|PANEL_EN1_Pin|PANEL_EN0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : HEATER2_PWM_Pin HEATER1_PWM_Pin */
  GPIO_InitStruct.Pin = HEATER2_PWM_Pin|HEATER1_PWM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PANEL_PG3_Pin */
  GPIO_InitStruct.Pin = PANEL_PG3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PANEL_PG3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PANEL_PG2_Pin PANEL_PG1_Pin PANEL_PG0_Pin */
  GPIO_InitStruct.Pin = PANEL_PG2_Pin|PANEL_PG1_Pin|PANEL_PG0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
