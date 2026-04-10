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
	distributionQueue = xQueueCreate(4, sizeof(EPS_CommandTypedef));
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

void DISTRIBUTION_UpdateState(EPS_CommandTypedef cmd)
{
	if (cmd.type == CMD_SYS)
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
}

void DISTRIBUTION_ApplyeFuseCommands(uint16_t eFuseCMD)
{
	TELEMETRY_printf("Processing eFuse command");
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
	hdist.eFuseStates = (uint8_t) eFuseCMD & 0xFF;
}

void DISTRIBUTION_checkEfuseFaults(void)
{
	uint8_t CH1_3V3_STATE = HAL_GPIO_ReadPin(_3V3_CH1_nFLT_GPIO_Port, _3V3_CH1_nFLT_Pin);
	uint8_t CH2_3V3_STATE = HAL_GPIO_ReadPin(_3V3_CH2_nFLT_GPIO_Port, _3V3_CH2_nFLT_Pin);
	uint8_t CH1_5V_STATE = HAL_GPIO_ReadPin(_5V_CH1_nFLT_GPIO_Port, _5V_CH1_nFLT_Pin);
	uint8_t CH2_5V_STATE = HAL_GPIO_ReadPin(_5V_CH2_nFLT_GPIO_Port, _5V_CH2_nFLT_Pin);
	uint8_t CH1_6V_STATE = HAL_GPIO_ReadPin(_6V_CH1_PG_GPIO_Port, _6V_CH1_PG_Pin);
	uint8_t CH2_6V_STATE = 0;
	uint8_t CH1_12V_STATE = HAL_GPIO_ReadPin(_12V_CH1_nFLT_GPIO_Port, _12V_CH1_nFLT_Pin);
	uint8_t CH2_12V_STATE = HAL_GPIO_ReadPin(_12V_CH2_nFLT_GPIO_Port, _12V_CH2_nFLT_Pin);

	hdist.eFuseFaults = CH1_3V3_STATE << 7
					  | CH2_3V3_STATE << 6
					  | CH1_5V_STATE  << 5
					  | CH2_5V_STATE  << 4
					  | CH1_6V_STATE  << 3
					  | CH2_6V_STATE  << 2
					  | CH1_12V_STATE << 1
					  | CH2_12V_STATE;

}



