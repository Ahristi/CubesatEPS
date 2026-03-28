/*
 * telemetry.c
 *
 *  Created on: Mar 25, 2026
 *      Author: Tom
 */


#include "telemetry.h"
#include "powerDistribution.h"


uint8_t rx_byte;
uint8_t rx_line[64];
uint16_t rx_index = 0;
uint8_t rx_ready = 0;
uint8_t rx_msg_len = 0;



TELEMETRY_handlerTypedef htelem;

void TELEMETRY_Init(CAN_HandleTypeDef*  hcan, UART_HandleTypeDef* huart)
{
	return;
}



void uartCMDHandler(uint8_t *msg, uint16_t len)
{
    EPSCommand_t cmdMsg = {0};

    if (len < 3)
    {
        return;
    }

    if (msg[0] == CMD_EFUSE)
    {
        cmdMsg.type = CMD_EFUSE;
        cmdMsg.data = ((uint16_t)msg[1] << 8) | msg[2];
        xQueueSend(distributionQueue, &cmdMsg, 0);
    }
    else if (msg[0] == CMD_SYS)
    {
        cmdMsg.type = CMD_SYS;
        cmdMsg.data = ((uint16_t)msg[1] << 8) | msg[2];
        xQueueSend(distributionQueue, &cmdMsg, 0);
    }
}



void TELEMETRY_sendStartupMessage(void)
{
    char msg[64];

    int len = snprintf(msg, sizeof(msg),
                       "\r\n%s | %s\r\n",
                       DEVICE_NAME,
                       SOFTWARE_VERSION);


    HAL_UART_Transmit(&huart4, (uint8_t*)msg, len, HAL_MAX_DELAY);
}






void TELEMETRY_sendCANMessage(uint32_t stdID, const uint8_t *frame_data, uint8_t dlc)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    txHeader.StdId = stdID;
    txHeader.ExtId = 0;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = dlc;
    txHeader.TransmitGlobalTime = DISABLE;
    HAL_CAN_AddTxMessage(htelem.hcan, &txHeader, (uint8_t *)frame_data, &txMailbox);
}





void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4)
    {
        if (!rx_ready)
        {
            if (rx_byte == '\n' || rx_byte == '\r')
            {
            	rx_msg_len = rx_index;
                rx_line[rx_index] = '\0';
                rx_ready = 1;
                rx_index = 0;
            }
            else
            {
                if (rx_index < UART_BUFFER_LENGTH - 1)
                {
                    rx_line[rx_index] = rx_byte;
                    rx_index++;
                }
                else
                {
                    rx_index = 0;
                }
            }
        }

        // Re-arm interrupt for next byte
        HAL_UART_Receive_IT(&huart4, &rx_byte, 1);
    }
}
