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
QueueHandle_t canCommandQueue;
void TELEMETRY_Init(CAN_HandleTypeDef*  hcan, UART_HandleTypeDef* huart)
{
	CAN_FilterTypeDef  sFilterConfig;
	TELEMETRY_sendStartupMessage();
	htelem.hcan = hcan;
	htelem.huart = huart;
	sFilterConfig.FilterBank = 14;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_CAN_Start(&hcan2) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		Error_Handler();
	}
	canCommandQueue = xQueueCreate(8, sizeof(CAN_CommandTypedef));
}



void uartCMDHandler(uint8_t *msg, uint16_t len)
{
	EPS_CommandTypedef cmdMsg = {0};

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


void TELEMETRY_CANCMDHandler(CAN_CommandTypedef* can_msg)
{
	/*
	 *  This is a little retarded because we wait for the CAN queue handler to queue another almost identical message.
	 * 	Could probably do away with this entirely and queue commands directly from the interrupt, but I feel like this will make it easier to handle
	 *  commands from multiple peripherals.
	 */
	EPS_CommandTypedef cmd_msg = {0};
    uint32_t StdId = can_msg->header.StdId;
	if (can_msg->header.IDE != CAN_ID_STD)
	{
		return;
	}
	if (StdId == CMD_EFUSE)
	{
		cmd_msg.type = CMD_EFUSE;
		cmd_msg.data = (uint16_t)can_msg->data[0];
		xQueueSend(distributionQueue, &cmd_msg, 0);
	}
	if (StdId == CMD_SYS)
	{
		cmd_msg.type = CMD_SYS;
		cmd_msg.data = (uint16_t)can_msg->data[0];
		xQueueSend(distributionQueue, &cmd_msg, 0);
	}
	if (StdId == CMD_WATCHDOG)
	{
		htelem.OBC_watchdog = 0;
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
    txHeader.StdId = stdID;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = dlc;
    txHeader.TransmitGlobalTime = DISABLE;
	while(HAL_CAN_GetTxMailboxesFreeLevel(htelem.hcan) == 0);
    if (HAL_CAN_AddTxMessage(htelem.hcan, &txHeader, frame_data, &htelem.CAN_txMailbox) != HAL_OK)
    {
      Error_Handler();
    }
}

void TELEMETRY_testCANLoopback(void)
{
	  CAN_TxHeaderTypeDef   TxHeader;
	  TxHeader.StdId = 0x123;
	  TxHeader.RTR = CAN_RTR_DATA;
	  TxHeader.IDE = CAN_ID_STD;
	  TxHeader.DLC = 8;
	  TxHeader.TransmitGlobalTime = DISABLE;
	  TxData[0] = 0;
	  TxData[7] = 0xFF;
	  TxData[0] ++;
	  TxData[7] --;


	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0);
    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox) != HAL_OK)
    {
      Error_Handler();
    }
}




void TELEMETRY_send3V3Telem(void)
{
	uint8_t frame_data[8] = {0};
	frame_data[0] = hanalog.raw[INDEX_3V3_VMON] >> 8;
	frame_data[1] = hanalog.raw[INDEX_3V3_VMON] & 0xFF;
	frame_data[2] = hanalog.raw[INDEX_3V3_CH1_IMON] >> 8;
	frame_data[3] = hanalog.raw[INDEX_3V3_CH1_IMON] & 0xFF;
	frame_data[4] = hanalog.raw[INDEX_3V3_CH2_IMON] >> 8;
	frame_data[5] = hanalog.raw[INDEX_3V3_CH2_IMON] & 0xFF;
	TELEMETRY_sendCANMessage(ID_3V3_MEASUREMENTS, frame_data, CAN_FRAME_LENGTH);
}
void TELEMETRY_send5VTelem(void)
{
	uint8_t frame_data[8] = {0};
	frame_data[0] = hanalog.raw[INDEX_5V_VMON] >> 8;
	frame_data[1] = hanalog.raw[INDEX_5V_VMON] & 0xFF;
	frame_data[2] = hanalog.raw[INDEX_5V_CH1_IMON] >> 8;
	frame_data[3] = hanalog.raw[INDEX_5V_CH1_IMON] & 0xFF;
	frame_data[4] = hanalog.raw[INDEX_5V_CH2_IMON] >> 8;
	frame_data[5] = hanalog.raw[INDEX_5V_CH2_IMON] & 0xFF;
	TELEMETRY_sendCANMessage(ID_5V_MEASUREMENTS, frame_data, CAN_FRAME_LENGTH);
}


void TELEMETRY_send6VTelem(void)
{
	uint8_t frame_data[8] = {0};
	frame_data[0] = hanalog.raw[INDEX_6V_VMON] >> 8;
	frame_data[1] = hanalog.raw[INDEX_6V_VMON] & 0xFF;
	frame_data[2] = hanalog.raw[INDEX_6V_CH1_IMON] >> 8;
	frame_data[3] = hanalog.raw[INDEX_6V_CH1_IMON] & 0xFF;
	frame_data[4] = 0x00;
	frame_data[5] = 0x00;
	TELEMETRY_sendCANMessage(ID_6V_MEASUREMENTS, frame_data, CAN_FRAME_LENGTH);
}

void TELEMETRY_send12VTelem(void)
{
	uint8_t frame_data[8] = {0};
	frame_data[0] = hanalog.raw[INDEX_12V_VMON] >> 8;
	frame_data[1] = hanalog.raw[INDEX_12V_VMON] & 0xFF;
	frame_data[2] = hanalog.raw[INDEX_12V_CH1_IMON] >> 8;
	frame_data[3] = hanalog.raw[INDEX_12V_CH1_IMON] & 0xFF;
	frame_data[4] = hanalog.raw[INDEX_12V_CH2_IMON] >> 8;
	frame_data[5] = hanalog.raw[INDEX_12V_CH2_IMON] & 0xFF;
	TELEMETRY_sendCANMessage(ID_12V_MEASUREMENTS, frame_data, CAN_FRAME_LENGTH);
}
void TELEMETRY_sendBattTelem(void)
{
	uint8_t frame_data[8] = {0};
	
	frame_data[0] = bms_measurements[BMS_BATT_VMON].raw >> 8;
	frame_data[1] = bms_measurements[BMS_BATT_VMON].raw & 0xFF;

	frame_data[2] = bms_measurements[BMS_BATT_IMON].raw >> 8;
	frame_data[3] = bms_measurements[BMS_BATT_IMON].raw & 0xFF;

	frame_data[4] = bms_measurements[BMS_SYS_VMON].raw>> 8;
	frame_data[5] = bms_measurements[BMS_SYS_VMON].raw & 0xFF;

	frame_data[6] = (uint8_t)bms_measurements[BMS_BAT_TMON].raw;
	frame_data[7] = (uint8_t)(0);//TODO: DIE TEMP;

	TELEMETRY_sendCANMessage(ID_BMS_MEASUREMENTS, frame_data, CAN_FRAME_LENGTH);
}





void TELEMETRY_sendSysTelem(void)
{
	uint8_t frame_data[8] = {0};
	frame_data[0] = hdist.eFuseStates;
	frame_data[1] = hdist.eFuseFaults;
	frame_data[2] = hanalog.internal_temp;
	TELEMETRY_sendCANMessage(ID_SYS_MEASUREMENTS, frame_data, CAN_FRAME_LENGTH);
}

//-------------INTERRUPTS-------------
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



void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_CommandTypedef msg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &msg.header, msg.data) != HAL_OK)
    {
        return;
    }
    xQueueSendFromISR(canCommandQueue, &msg, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  /*
  if (HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {

    Error_Handler();
  }
  */
}
void TELEMETRY_printf(const char *fmt, ...)
{
    char msg[128];  // increase if needed

    va_list args;
    va_start(args, fmt);

    int len = vsnprintf(msg, sizeof(msg), fmt, args);

    va_end(args);

    if (len > 0)
    {
        // Clamp length to buffer size (vsnprintf can return > sizeof)
        if (len > sizeof(msg))
            len = sizeof(msg);

        HAL_UART_Transmit(&huart4, (uint8_t*)msg, len, HAL_MAX_DELAY);
    }
}

void TELEMETRY_updateWatchdog()
{
	EPS_CommandTypedef cmd_msg = {0};
	htelem.OBC_watchdog++;
	if (htelem.OBC_watchdog >= OBC_WATCHDOG_COMPARE)
	{
		htelem.OBC_watchdog = 0;
		cmd_msg.type = CMD_SYS;
		cmd_msg.data = RESET_MASK;
		xQueueSend(distributionQueue, &cmd_msg, 0);
	}
}
