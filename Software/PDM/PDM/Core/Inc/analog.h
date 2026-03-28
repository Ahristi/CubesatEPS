/*
 * analog.h
 *
 *  Created on: Mar 28, 2026
 *      Author: Tom Holland
 */

#ifndef SRC_ANALOG_H_
#define SRC_ANALOG_H_

#include "tim.h"
#include "adc.h"
#include "dma.h"
#include <stdint.h>

#define ADC_VREF 3.3f
#define ADC_MAX  4095.0f



#define NUM_ANALOG_MEASUREMENTS 14
#define DMA_BUFFER_SIZE 2*NUM_ANALOG_MEASUREMENTS




typedef struct
{
    ADC_HandleTypeDef* hadc;
    TIM_HandleTypeDef* htim;
    uint16_t dma_buf[NUM_ANALOG_MEASUREMENTS];
    uint16_t raw[NUM_ANALOG_MEASUREMENTS];
    float voltage[NUM_ANALOG_MEASUREMENTS];
    float internal_temp;
    volatile uint8_t busy;
    volatile uint8_t data_ready;
} ANALOG_handlerTypedef;

typedef enum
{
    INDEX_6V_VMON      = 0,
	INDEX_12V_VMON     = 1,
	INDEX_12V_CH1_IMON = 2,
	INDEX_5V_CH1_IMON  = 3,
	INDEX_6V_CH1_IMON  = 4,
	INDEX_5V_CH2_IMON  = 5,
	INDEX_3V3_CH2_IMON = 6,
	INDEX_3V3_CH1_IMON = 7,
	INDEX_3V3_VMON	   = 8,
	INDEX_5V_VMON      = 9,
	INDEX_12V_CH2_IMON = 10,
	INDEX_HEATER1_IMON = 11,
	INDEX_HEATER2_IMON = 12,
	INDEX_MCU_TEMP     = 13
} ANALOG_adcIndexTypedef;


extern ANALOG_handlerTypedef hanalog;


void ANALOG_Init(TIM_HandleTypeDef* htim, ADC_HandleTypeDef* hadc);
void ANALOG_startScan();
void ANALOG_process();
float ANALOG_convertTemp(uint16_t raw);


#endif /* SRC_ANALOG_H_ */
