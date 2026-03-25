/*
 * powerDistribution.c
 *
 *  Created on: Mar 25, 2026
 *      Author: Tom Holland
 */
#include "powerDistribution.h"


DISTRIBUTION_HandleTypeDef hdist;
QueueHandle_t distributionQueue;



static const DISTRIBUTION_HandleTypeDef DISTRIBUTION_DEFAULT = {
    .eFuseCommands = 0x0,
    .state         = DISTRIBUTION_STARTUP,
    .eFuseFaults   = 0x0,
	.sysCommands   = 0x0,
	.sysFaults     = 0x0
};



void DISTRIBUTION_Init()
{
	hdist = DISTRIBUTION_DEFAULT;
	distributionQueue = xQueueCreate(4, sizeof(EPSCommand_t));
}

void DISTRIBUTION_DisableAlleFuses(void)
{
    HAL_GPIO_WritePin(_3V3_CH2_EN_GPIO_Port, _3V3_CH2_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_5V_CH1_EN_GPIO_Port, _5V_CH1_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_5V_CH2_EN_GPIO_Port, _5V_CH2_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_6V_CH1_EN_GPIO_Port, _6V_CH1_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_12V_CH1_EN_GPIO_Port, _12V_CH1_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_12V_CH2_EN_GPIO_Port, _12V_CH2_EN_Pin, GPIO_PIN_RESET);
}

void DISTRIBUTION_DisableConverters(void)
{
    HAL_GPIO_WritePin(_5V_CONV_EN_GPIO_Port, _5V_CONV_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_6V_CONV_EN_GPIO_Port, _6V_CONV_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_12V_CONV_EN_GPIO_Port, _12V_CONV_EN_Pin, GPIO_PIN_RESET);
}

void DISTRIBUTION_UpdateState(EPSCommand_t cmd)
{
 if (cmd.data & RESET_MASK)
 {
	 hdist.state = DISTRIBUTION_RESET;
 }
 else if (cmd.data & POWER_SAVE_MASK)
 {
	 hdist.state = DISTRIBUTION_POWER_SAVE;
 }
 else
 {
	 hdist.state = DISTRIBUTION_NORMAL;
 }
}

void DISTRIBUTION_ApplyeFuseCommands(uint16_t eFuseCMD)
{
	HAL_GPIO_WritePin(_3V3_CH2_EN_GPIO_Port, _3V3_CH2_EN_Pin,
		((eFuseCMD & _3V3_CH2_EN_MASK) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(_5V_CH1_EN_GPIO_Port, _5V_CH1_EN_Pin,
		((eFuseCMD & _5V_CH1_EN_MASK) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(_5V_CH2_EN_GPIO_Port, _5V_CH2_EN_Pin,
		((eFuseCMD & _5V_CH2_EN_MASK) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(_6V_CH1_EN_GPIO_Port, _6V_CH1_EN_Pin,
		((eFuseCMD & _6V_CH1_EN_MASK) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(_12V_CH1_EN_GPIO_Port, _12V_CH1_EN_Pin,
		((eFuseCMD & _12V_CH1_EN_MASK) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(_12V_CH2_EN_GPIO_Port, _12V_CH2_EN_Pin,
		((eFuseCMD & _12V_CH2_EN_MASK) != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}



