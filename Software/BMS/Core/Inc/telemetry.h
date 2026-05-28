#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__
#include "tim.h"
#include "dma.h"
#include "adc.h"
#include <math.h>
#include <string.h>
//-------------Defines-------------
#define ADC1_NUM_CHANNELS 11
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

//Thermistor Parameters
#define VCC     3.3f
#define R_TOP   10000.0f  
#define R0      10000.0f    
#define T0      298.15f       
#define BETA    3435.0f     


//-------------Typedef and Enums-------------
typedef struct {
    uint16_t raw_panel0_vmon;
    uint16_t raw_panel1_imon;
    uint16_t raw_panel2_vmon;
    uint16_t raw_panel2_imon;
    uint16_t raw_sys_vmon;
    uint16_t raw_sys_imon;
    uint16_t raw_batt_tmon;
    uint16_t raw_umb_vmon;
    uint16_t raw_batt_vmon;
    uint16_t raw_batt_imon;

    float    voltage_panel0_vmon;
    float    voltage_panel1_imon;
    float    voltage_panel2_vmon;
    float    voltage_panel2_imon;
    float    voltage_sys_vmon;
    float    voltage_sys_imon;
    float    voltage_batt_tmon;
    float    voltage_umb_vmon;
    float    voltage_batt_vmon;
    float    voltage_batt_imon;

    float    panel0_vmon;
    float    panel1_imon;
    float    panel2_vmon;
    float    panel2_imon;
    float    sys_vmon;
    float    sys_imon;
    float    batt_tmon;
    float    umb_vmon;
    float    batt_vmon;
    float    batt_imon;
}TELEMETRY_Handler_t;

extern TELEMETRY_Handler_t htelemetry;

void TELEMETRY_Init(void);
void TELEMETRY_Task(void);
float TELEMETRY_RawToAdcVoltage(uint16_t raw, float vdda);
float TELEMETRY_CalibrateVDDA(uint16_t raw_vrefint);
float TELEMETRY_ConvertPanelVmon(float raw_voltage);
float TELEMETRY_ConvertPanelImon(float raw_voltage);
float TELEMETRY_ConvertSysVmon(float raw_voltage);
float TELEMETRY_ConvertSysImon(float raw_voltage);
float TELEMETRY_ConvertBatTmon(float raw_voltage);
float TELEMETRY_ConvertUmbVmon(float raw_voltage);
float TELEMETRY_ConvertBattVmon(float raw_voltage);
float TELEMETRY_ConvertBattImon(float raw_voltage);

#endif