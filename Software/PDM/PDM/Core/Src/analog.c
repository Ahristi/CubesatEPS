/*
 * analog.c
 *
 *  Created on: Mar 28, 2026
 *      Author: Tom
 */

#include "analog.h"
#include "freertos.h"
#include "cmsis_os2.h"
ANALOG_handlerTypedef hanalog;


void ANALOG_Init(TIM_HandleTypeDef* htim, ADC_HandleTypeDef* hadc)
{
	hanalog = (ANALOG_handlerTypedef){
		.hadc = hadc,
		.htim = htim,
		.internal_temp = 0,
		.dma_buf = {0},
		.busy = 0,
		.data_ready = 0
	};
	HAL_TIM_Base_Start(htim);
}

void ANALOG_startScan()
{
	hanalog.busy = 1;
	hanalog.data_ready = 0;
	HAL_ADC_Start_DMA(hanalog.hadc, (uint32_t*)hanalog.dma_buf, NUM_ANALOG_MEASUREMENTS);
}

void ANALOG_process()
{
    for (uint8_t i = 0; i < NUM_ANALOG_MEASUREMENTS; i++)
    {
        hanalog.raw[i] = hanalog.dma_buf[i];
        hanalog.voltage[i] = ((float)hanalog.raw[i] * ADC_VREF) / ADC_MAX;
    }
    hanalog.internal_temp = ANALOG_convertTemp(hanalog.raw[INDEX_MCU_TEMP]);
	hanalog.busy = 0;
	hanalog.data_ready = 1;
}

float ANALOG_convertTemp(uint16_t adc_raw)
{
    const uint16_t *TS_CAL1 = (uint16_t*)0x1FFF7A2C;
    const uint16_t *TS_CAL2 = (uint16_t*)0x1FFF7A2E;

    float cal1 = (float)(*TS_CAL1);
    float cal2 = (float)(*TS_CAL2);

    return ((adc_raw - cal1) * 80.0f) / (cal2 - cal1) + 30.0f;
}
