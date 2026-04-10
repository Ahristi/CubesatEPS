/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "powerDistribution.h"
#include "usart.h"
#include "telemetry.h"
#include "analog.h"
#include "gpio.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for distrHandle */
osThreadId_t distrHandleHandle;
const osThreadAttr_t distrHandle_attributes = {
  .name = "distrHandle",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for telemetryHandle */
osThreadId_t telemetryHandleHandle;
const osThreadAttr_t telemetryHandle_attributes = {
  .name = "telemetryHandle",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for heaterHandle */
osThreadId_t heaterHandleHandle;
const osThreadAttr_t heaterHandle_attributes = {
  .name = "heaterHandle",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for BMSHandle */
osThreadId_t BMSHandleHandle;
const osThreadAttr_t BMSHandle_attributes = {
  .name = "BMSHandle",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for commandHandle */
osThreadId_t commandHandleHandle;
const osThreadAttr_t commandHandle_attributes = {
  .name = "commandHandle",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};
/* Definitions for analogHandle */
osThreadId_t analogHandleHandle;
const osThreadAttr_t analogHandle_attributes = {
  .name = "analogHandle",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void distributionTask(void *argument);
void telemetryTask(void *argument);
void heaterTask(void *argument);
void BMSTask(void *argument);
void commandTask(void *argument);
void analogTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of distrHandle */
  distrHandleHandle = osThreadNew(distributionTask, NULL, &distrHandle_attributes);

  /* creation of telemetryHandle */
  telemetryHandleHandle = osThreadNew(telemetryTask, NULL, &telemetryHandle_attributes);

  /* creation of heaterHandle */
  heaterHandleHandle = osThreadNew(heaterTask, NULL, &heaterHandle_attributes);

  /* creation of BMSHandle */
  BMSHandleHandle = osThreadNew(BMSTask, NULL, &BMSHandle_attributes);

  /* creation of commandHandle */
  commandHandleHandle = osThreadNew(commandTask, NULL, &commandHandle_attributes);

  /* creation of analogHandle */
  analogHandleHandle = osThreadNew(analogTask, NULL, &analogHandle_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {

	//LED heartbeat
    osDelay(1000);
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
    osDelay(100);
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    osDelay(100);
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
    osDelay(100);
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_distributionTask */
/**
* @brief Function implementing the distrHandle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_distributionTask */
void distributionTask(void *argument)
{
  /* USER CODE BEGIN distributionTask */
	/* Infinite loop */
	EPS_CommandTypedef distributionCMD;
	for(;;)
	{
		if (hdist.state == DISTRIBUTION_STARTUP)
		{
			TELEMETRY_printf("Enabling all converters\r\n");
			//Enable the converters
			HAL_GPIO_WritePin(_5V_CONV_EN_GPIO_Port, _5V_CONV_EN_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(_6V_CONV_EN_GPIO_Port, _6V_CONV_EN_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(_12V_CONV_EN_GPIO_Port, _12V_CONV_EN_Pin, GPIO_PIN_SET);
			TELEMETRY_printf("Turning on OBC\r\n");
			//Turn on the OBC and move to normal operation mode
			HAL_GPIO_WritePin(_3V3_CH1_EN_GPIO_Port, _3V3_CH1_EN_Pin, GPIO_PIN_SET);
			hdist.convStates = DEFAULT_CONV_STATE;
			hdist.state = DISTRIBUTION_NORMAL;
			TELEMETRY_printf("Begin normal operation\r\n");
		}
		if (xQueueReceive(distributionQueue, &distributionCMD, pdMS_TO_TICKS(10)) == pdPASS)
		{
			DISTRIBUTION_UpdateState(distributionCMD);
			if (hdist.state == DISTRIBUTION_NORMAL)
			{
				if (distributionCMD.type == CMD_EFUSE)
				{
					DISTRIBUTION_ApplyeFuseCommands(distributionCMD.data);
				}

			}
			else if (hdist.state == DISTRIBUTION_POWER_SAVE)
			{
				DISTRIBUTION_DisableAlleFuses();
				DISTRIBUTION_DisableConverters();
			}
			else if (hdist.state == DISTRIBUTION_RESET)
			{
				TELEMETRY_printf("Beginning reset procedure\r\n...");
				//Turn off all converters and then turn off OBC
				TELEMETRY_printf("Turning off all eFuses\r\n...");
				DISTRIBUTION_DisableAlleFuses();
				DISTRIBUTION_DisableConverters();

				//Disable the OBC, wait 2 seconds then go through normal startup.
				TELEMETRY_printf("Power cycling OBC\r\n...");
				HAL_GPIO_WritePin(_3V3_CH1_EN_GPIO_Port, _3V3_CH1_EN_Pin, GPIO_PIN_RESET);
				osDelay(2000);
				HAL_GPIO_WritePin(_3V3_CH1_EN_GPIO_Port, _3V3_CH1_EN_Pin, GPIO_PIN_SET);
				TELEMETRY_printf("OBC powered up, begin startup procedure\r\n");
				hdist.state = DISTRIBUTION_STARTUP;
			}
		}
	  }
  /* USER CODE END distributionTask */
}

/* USER CODE BEGIN Header_telemetryTask */
/**
* @brief Function implementing the telemetryHandle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_telemetryTask */
void telemetryTask(void *argument)
{
  /* USER CODE BEGIN telemetryTask */
  /* Infinite loop */
	for(;;)
	{
		TELEMETRY_send3V3Telem();
		TELEMETRY_send5VTelem();
		TELEMETRY_send6VTelem();
		TELEMETRY_send12VTelem();
		TELEMETRY_sendSysTelem();
		TELEMETRY_printf("Sent CAN telemetry\r\n");
		osDelay(1000);
	}
  /* USER CODE END telemetryTask */
}

/* USER CODE BEGIN Header_heaterTask */
/**
* @brief Function implementing the heaterHandle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_heaterTask */
void heaterTask(void *argument)
{
  /* USER CODE BEGIN heaterTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END heaterTask */
}

/* USER CODE BEGIN Header_BMSTask */
/**
* @brief Function implementing the BMSHandle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BMSTask */
void BMSTask(void *argument)
{
  /* USER CODE BEGIN BMSTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END BMSTask */
}

/* USER CODE BEGIN Header_commandTask */
/**
* @brief Function implementing the commandHandle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_commandTask */
void commandTask(void *argument)
{
  /* USER CODE BEGIN commandTask */
  /* Infinite loop */
	//uint8_t local_rx_line[UART_BUFFER_LENGTH];
	//uint16_t local_len;
	CAN_CommandTypedef can_msg;
	for (;;)
	{
		if (xQueueReceive(canCommandQueue, &can_msg, pdMS_TO_TICKS(10)) == pdPASS)
		{
			TELEMETRY_CANCMDHandler(&can_msg);
		}
		/*
		if (rx_ready)
		{
			taskENTER_CRITICAL();
			memcpy(local_rx_line, rx_line, UART_BUFFER_LENGTH);
			local_len = rx_msg_len;
			rx_ready = 0;
			taskEXIT_CRITICAL();
			uartCMDHandler(local_rx_line, local_len);
		}
		*/
		osDelay(100);
    }
  /* USER CODE END commandTask */
}

/* USER CODE BEGIN Header_analogTask */
/**
* @brief Function implementing the TASK_ANALOG thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_analogTask */
void analogTask(void *argument)
{
  /* USER CODE BEGIN analogTask */
  /* Infinite loop */
  for(;;)
  {
	  ANALOG_startScan();
	  osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
	  ANALOG_process();
	  osDelay(10);
  }
  /* USER CODE END analogTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
    	osThreadFlagsSet(analogHandleHandle, 0x01);
    }
}

/* USER CODE END Application */

