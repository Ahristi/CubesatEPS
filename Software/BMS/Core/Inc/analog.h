#ifndef __FILTER_H__
#define __FILTER_H__
#include "adc.h"
#include "tim.h"
#include "dma.h"
#include <stdint.h>
#include <math.h>
#include <string.h>
//-------------Defines-------------
#define ADC_NUM_SAMPLES 10
#define ADC_MAX_COUNTS  4095.0f
#define VREFINT_CAL_VOLTAGE 3.3f


#define PANEL_VOLTAGE_GAIN_VV 0.2702702703f
#define PANEL_CURRENT_GAIN_VA 1.0f

#define SYS_VOLTAGE_GAIN_VV 0.1449275362f
#define SYS_CURRENT_GAIN_VA 1.0f

#define UMB_VOLTAGE_GAIN_VV 0.1449275362f
#define UMB_CURRENT_GAIN_VA 1.0f

#define BATT_VOLTAGE_GAIN_VV 0.1449275362f
#define BATT_CURRENT_GAIN_VA 1.0f
#define BATT_CURRENT_OFFSET_V 2.45


#define ADC_DMA_BUFFER_LEN   (ADC_NUM_CHANNELS * ADC_NUM_SAMPLES * 2)
//Thermistor Parameters
#define VCC     3.3f
#define R_TOP   10000.0f  
#define R0      10000.0f    
#define T0      298.15f       
#define BETA    3435.0f     


//-------------Typedef and Enums-------------
typedef float (*ANALOG_convertFunction)(float);

typedef struct{
    uint16_t raw;
    float voltage;
    float value;
    ANALOG_convertFunction convert;
}ANALOG_Measurement_t;

typedef enum {
    ADC_PANEL0_VMON = 0,
    ADC_PANEL1_IMON,
    ADC_PANEL2_VMON,
    ADC_PANEL2_IMON,
    ADC_SYS_VMON,
    ADC_SYS_IMON,
    ADC_BAT_TMON,
    ADC_UMB_VMON,
    ADC_BATT_IMON,
    ADC_BATT_VMON,
    ADC_VREFINT,
    ADC_NUM_CHANNELS
} ADC_ChannelIndex_t;
//-------------Variables-------------




void  ANALOG_Init(void);
void  ANALOG_Task(void);
void  ANALOG_overSampleADC(const uint16_t *buffer);
float ANALOG_RawToAdcVoltage(uint16_t raw, float vdda);
float ANALOG_CalibrateVDDA(uint16_t raw_vrefint);
float ANALOG_ConvertPanelVmon(float raw_voltage);
float ANALOG_ConvertPanelImon(float raw_voltage);
float ANALOG_ConvertSysVmon(float raw_voltage);
float ANALOG_ConvertSysImon(float raw_voltage);
float ANALOG_ConvertBatTmon(float raw_voltage);
float ANALOG_ConvertUmbVmon(float raw_voltage);
float ANALOG_ConvertBattVmon(float raw_voltage);
float ANALOG_ConvertBattImon(float raw_voltage);
float ANALOG_ConvertVREF(float raw_voltage);






#endif