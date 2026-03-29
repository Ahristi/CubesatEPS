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


CAN_TxHeaderTypeDef   TxHeader; /* Header containing the information of the transmitted frame */
TELEMETRY_handlerTypedef htelem;

void TELEMETRY_Init(CAN_HandleTypeDef*  hcan, UART_HandleTypeDef* huart)
{
	htelem.hcan = hcan;
	htelem.huart = huart;
	TELEMETRY_sendStartupMessage();

	/*
    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0x0000;
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = 0x0000;
    filter.FilterMaskIdLow = 0x0000;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14;


	HAL_CAN_ConfigFilter(hcan, &filter);
	HAL_CAN_Start(hcan);
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);*/

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






void TELEMETRY_sendCANMessage(uint32_t stdID, uint8_t *frame_data, uint8_t dlc)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    txHeader.StdId = stdID;
    txHeader.ExtId = 0;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 2;
    frame_data[0] = 50;
    frame_data[1] = 0xAA;
    txHeader.TransmitGlobalTime = DISABLE;
    HAL_StatusTypeDef retval = HAL_CAN_AddTxMessage(htelem.hcan, &txHeader, frame_data, &txMailbox);
    if (HAL_CAN_IsTxMessagePending(htelem.hcan, txMailbox) == 0)
    {
    	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
    }
    else
    {
    	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
    }
}

void TELEMETRY_testCANLoopback(void)
{
	  TxHeader.StdId = 0x123;
	  TxHeader.RTR = CAN_RTR_DATA;
	  TxHeader.IDE = CAN_ID_STD;
	  TxHeader.DLC = 8;
	  TxHeader.TransmitGlobalTime = DISABLE;
	  TxData[0] = 0;
	  TxData[7] = 0xFF;
	  TxData[0] ++; /* Increment the first byte */
	  TxData[7] --; /* Increment the last byte */

    /* It's mandatory to look for a free Tx mail box */
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0); /* Wait till a Tx mailbox is free. Using while loop instead of HAL_Delay() */
    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox) != HAL_OK)
    {
      /* Transmission request Error */
      Error_Handler();
    }
    /*
    if (HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO0) > 0)
    {

        if (HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        }
    }*/
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

