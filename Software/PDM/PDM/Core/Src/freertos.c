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
/* Definitions for distrTask */
osThreadId_t distrTaskHandle;
const osThreadAttr_t distrTask_attributes = {
  .name = "distrTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for telemetryTask */
osThreadId_t telemetryTaskHandle;
const osThreadAttr_t telemetryTask_attributes = {
  .name = "telemetryTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for heaterControlTa */
osThreadId_t heaterControlTaHandle;
const osThreadAttr_t heaterControlTa_attributes = {
  .name = "heaterControlTa",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for BMScontrolTask */
osThreadId_t BMScontrolTaskHandle;
const osThreadAttr_t BMScontrolTask_attributes = {
  .name = "BMScontrolTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for commandTask */
osThreadId_t commandTaskHandle;
const osThreadAttr_t commandTask_attributes = {
  .name = "commandTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh2,
};
/* Definitions for TASK_ANALOG */
osThreadId_t TASK_ANALOGHandle;
const osThreadAttr_t TASK_ANALOG_attributes = {
  .name = "TASK_ANALOG",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void DistributionTask(void *argument);
void telemetry(void *argument);
void heaterControl(void *argument);
void BMScontrol(void *argument);
void commandTaskHandler(void *argument);
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

  /* creation of distrTask */
  distrTaskHandle = osThreadNew(DistributionTask, NULL, &distrTask_attributes);

  /* creation of telemetryTask */
  telemetryTaskHandle = osThreadNew(telemetry, NULL, &telemetryTask_attributes);

  /* creation of heaterControlTa */
  heaterControlTaHandle = osThreadNew(heaterControl, NULL, &heaterControlTa_attributes);

  /* creation of BMScontrolTask */
  BMScontrolTaskHandle = osThreadNew(BMScontrol, NULL, &BMScontrolTask_attributes);

  /* creation of commandTask */
  commandTaskHandle = osThreadNew(commandTaskHandler, NULL, &commandTask_attributes);

  /* creation of TASK_ANALOG */
  TASK_ANALOGHandle = osThreadNew(analogTask, NULL, &TASK_ANALOG_attributes);

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

/* USER CODE BEGIN Header_DistributionTask */
/**
* @brief Function implementing the distrTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DistributionTask */
void DistributionTask(void *argument)
{
  /* USER CODE BEGIN DistributionTask */
  /* Infinite loop */
	EPSCommand_t distributionCMD;
	for(;;)
	{
		if (hdist.state == DISTRIBUTION_STARTUP)
		{
			//Enable the converters
			HAL_GPIO_WritePin(_5V_CONV_EN_GPIO_Port, _5V_CONV_EN_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(_6V_CONV_EN_GPIO_Port, _6V_CONV_EN_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(_12V_CONV_EN_GPIO_Port, _12V_CONV_EN_Pin, GPIO_PIN_SET);

			//Turn on the OBC and move to normal operation mode
			HAL_GPIO_WritePin(_3V3_CH1_EN_GPIO_Port, _3V3_CH1_EN_Pin, GPIO_PIN_SET);
			hdist.convStates = DEFAULT_CONV_STATE;
			hdist.state = DISTRIBUTION_NORMAL;
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
				//Turn off all converters and then turn off OBC
				DISTRIBUTION_DisableAlleFuses();
				DISTRIBUTION_DisableConverters();

				//Disable the OBC, weight 1 second then go through normal startup.
				HAL_GPIO_WritePin(_3V3_CH1_EN_GPIO_Port, _3V3_CH1_EN_Pin, GPIO_PIN_RESET);
				osDelay(1000);
				HAL_GPIO_WritePin(_3V3_CH1_EN_GPIO_Port, _3V3_CH1_EN_Pin, GPIO_PIN_SET);
				hdist.state = DISTRIBUTION_STARTUP;
			}
		}
  }

  /* USER CODE END DistributionTask */
}

/* USER CODE BEGIN Header_telemetry */
/**
* @brief Function implementing the telemetryTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_telemetry */
void telemetry(void *argument)
{
  /* USER CODE BEGIN telemetry */
  /* Infinite loop */
  for(;;)
  {



	  osDelay(1000);
  }
  /* USER CODE END telemetry */
}

/* USER CODE BEGIN Header_heaterControl */
/**
* @brief Function implementing the heaterControlTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_heaterControl */
void heaterControl(void *argument)
{
  /* USER CODE BEGIN heaterControl */
  /* Infinite loop */
  for(;;)
  {

	  //test
    osDelay(1);
  }
  /* USER CODE END heaterControl */
}

/* USER CODE BEGIN Header_BMScontrol */
/**
* @brief Function implementing the BMScontrolTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BMScontrol */
void BMScontrol(void *argument)
{
  /* USER CODE BEGIN BMScontrol */
  /* Infinite loop */
  for(;;)
  {

    osDelay(1);
  }
  /* USER CODE END BMScontrol */
}

/* USER CODE BEGIN Header_commandTaskHandler */
/**
* @brief Function implementing the commandTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_commandTaskHandler */
void commandTaskHandler(void *argument)
{
  /* USER CODE BEGIN commandTaskHandler */
  /* Infinite loop */
	uint8_t local_rx_line[UART_BUFFER_LENGTH];
    uint16_t local_len;
    for (;;)
    {
        if (rx_ready)
        {
            taskENTER_CRITICAL();
            memcpy(local_rx_line, rx_line, UART_BUFFER_LENGTH);
            local_len = rx_msg_len;
            rx_ready = 0;
            taskEXIT_CRITICAL();
            uartCMDHandler(local_rx_line, local_len);
        }

        osDelay(100);
    }
  /* USER CODE END commandTaskHandler */
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
    	osThreadFlagsSet(TASK_ANALOGHandle, 0x01);
    }
}

/* USER CODE END Application */

