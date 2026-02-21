/*
 * computer_interface.c
 *
 *  Created on: Feb 21, 2026
 *      Author: Tom
 */

#include "computer_interface.h"
#include "usart.h"
HAL_StatusTypeDef waitCMD(BQ25792_HandleTypeDef* hbms)
{

	if (uart_line_ready) {
		if (uart_rx_buffer[0] == WRITE_SINGLE) {
			uint8_t reg  = uart_rx_buffer[1];
			uint16_t val = uart_rx_buffer[2] << 8 | uart_rx_buffer[2];
			return writeSingle(hbms, reg, val);
		}
		else if (uart_rx_buffer[0] == READ_SINGLE)
		{
			uint8_t reg  = uart_rx_buffer[1];
			return readSingle(hbms, reg);
		}
	}
	return HAL_OK;
}


HAL_StatusTypeDef readSingle(BQ25792_HandleTypeDef* hbms, uint8_t reg)
{
	uint16_t val;
	HAL_StatusTypeDef stat;
	if (bq25792_reg_is_16bit(reg))
	{
		stat = bq25792_read_u16(hbms, reg, &val);
	}
	else
	{
		stat = bq25792_read_u8(hbms, reg, (uint8_t*) &val);
	}

	if (stat != HAL_OK)
	{
		uart_tx_buffer[0] = CMD_ERROR;
		HAL_UART_Transmit(&huart3, uart_tx_buffer, ERROR_RETURN_LENGTH, HAL_MAX_DELAY);
	}
	else
	{
		uart_tx_buffer[0] = READ_SINGLE;
		uart_tx_buffer[1] = val >> 8;
		uart_tx_buffer[2] = val & 0xFF;
		HAL_UART_Transmit(&huart3, uart_tx_buffer, READ_SINGLE_RETURN_LENGTH, HAL_MAX_DELAY);
	}
	return stat;
}
HAL_StatusTypeDef writeSingle(BQ25792_HandleTypeDef* hbms, uint8_t reg, uint16_t val)
{
	HAL_StatusTypeDef stat;
	if (bq25792_reg_is_16bit(reg))
	{
		stat = bq25792_write_u16(hbms, reg, val);
	}
	else
	{
		uint8_t val8bit = (uint8_t)val;
		stat = bq25792_write_u8(hbms, reg, val8bit);
	}

	if (stat != HAL_OK)
	{
		uart_tx_buffer[0] = CMD_ERROR;
		HAL_UART_Transmit(&huart3, uart_tx_buffer, ERROR_RETURN_LENGTH, HAL_MAX_DELAY);
	}
	else
	{
		uart_tx_buffer[0] = WRITE_SINGLE;
		HAL_UART_Transmit(&huart3, uart_tx_buffer, WRITE_SINGLE_RETURN_LENGTH, HAL_MAX_DELAY);
	}

	return stat;
}

