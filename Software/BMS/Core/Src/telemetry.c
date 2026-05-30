#include "telemetry.h"

uint16_t adc1_dma_buffer[ADC1_NUM_CHANNELS];
TELEMETRY_Handler_t htelemetry;


void TELEMETRY_Init(void)
{
    return;
    //HAL_TIM_Base_Start(&htim2);
    //HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_dma_buffer, ADC1_NUM_CHANNELS); 
}


void TELEMETRY_Task(void)
{
    uint16_t adc_snapshot[ADC1_NUM_CHANNELS];

    __disable_irq();
    memcpy(adc_snapshot, adc1_dma_buffer, sizeof(adc_snapshot));
    __enable_irq();

    // Raw ADC values
    htelemetry.raw_panel0_vmon = adc_snapshot[0];
    htelemetry.raw_panel0_imon = adc_snapshot[1];
    htelemetry.raw_panel1_vmon = adc_snapshot[2];
    htelemetry.raw_panel1_imon = adc_snapshot[3];
    htelemetry.raw_sys_vmon    = adc_snapshot[4];
    htelemetry.raw_sys_imon    = adc_snapshot[5];
    htelemetry.raw_batt_tmon   = adc_snapshot[6];
    htelemetry.raw_umb_vmon    = adc_snapshot[7];
    htelemetry.raw_batt_vmon   = adc_snapshot[8];
    htelemetry.raw_batt_imon   = adc_snapshot[9];

    uint16_t raw_vdda          = adc_snapshot[10];
    float vdda                 = TELEMETRY_CalibrateVDDA(raw_vdda);

    // ADC pin voltages
    htelemetry.voltage_panel0_vmon = TELEMETRY_RawToAdcVoltage(htelemetry.raw_panel0_vmon, vdda);
    htelemetry.voltage_panel0_imon = TELEMETRY_RawToAdcVoltage(htelemetry.raw_panel0_imon, vdda);
    htelemetry.voltage_panel1_vmon = TELEMETRY_RawToAdcVoltage(htelemetry.raw_panel1_vmon, vdda);
    htelemetry.voltage_panel1_imon = TELEMETRY_RawToAdcVoltage(htelemetry.raw_panel1_imon, vdda);
    htelemetry.voltage_sys_vmon    = TELEMETRY_RawToAdcVoltage(htelemetry.raw_sys_vmon, vdda);
    htelemetry.voltage_sys_imon    = TELEMETRY_RawToAdcVoltage(htelemetry.raw_sys_imon, vdda);
    htelemetry.voltage_batt_tmon    = TELEMETRY_RawToAdcVoltage(htelemetry.raw_batt_tmon, vdda);
    htelemetry.voltage_umb_vmon    = TELEMETRY_RawToAdcVoltage(htelemetry.raw_umb_vmon, vdda);
    htelemetry.voltage_batt_vmon   = TELEMETRY_RawToAdcVoltage(htelemetry.raw_batt_vmon, vdda);
    htelemetry.voltage_batt_imon   = TELEMETRY_RawToAdcVoltage(htelemetry.raw_batt_imon, vdda);

    // True engineering values
    htelemetry.panel0_vmon = TELEMETRY_ConvertPanelVmon(htelemetry.voltage_panel0_vmon);
    htelemetry.panel0_imon = TELEMETRY_ConvertPanelImon(htelemetry.voltage_panel0_imon);
    htelemetry.panel1_vmon = TELEMETRY_ConvertPanelVmon(htelemetry.voltage_panel1_vmon);
    htelemetry.panel1_imon = TELEMETRY_ConvertPanelImon(htelemetry.voltage_panel1_imon);

    htelemetry.sys_vmon    = TELEMETRY_ConvertSysVmon(htelemetry.voltage_sys_vmon);
    htelemetry.sys_imon    = TELEMETRY_ConvertSysImon(htelemetry.voltage_sys_imon);
    htelemetry.batt_tmon    = TELEMETRY_ConvertBatTmon(htelemetry.voltage_batt_tmon);
    htelemetry.umb_vmon    = TELEMETRY_ConvertUmbVmon(htelemetry.voltage_umb_vmon);
    htelemetry.batt_vmon   = TELEMETRY_ConvertBattVmon(htelemetry.voltage_batt_vmon);
    htelemetry.batt_imon   = TELEMETRY_ConvertBattImon(htelemetry.voltage_batt_imon);
}
float TELEMETRY_CalibrateVDDA(uint16_t raw_vrefint)
{
    uint16_t vrefint_cal = *VREFINT_CAL_ADDR;
    return VREFINT_CAL_VOLTAGE *
           ((float)vrefint_cal / (float)raw_vrefint);
}

float TELEMETRY_RawToAdcVoltage(uint16_t raw, float vdda)
{
    return ((float)raw * vdda) / ADC_MAX_COUNTS;
}


float TELEMETRY_ConvertPanelVmon(float adc_voltage)
{
    return adc_voltage / PANEL_VOLTAGE_GAIN_VV;
}
float TELEMETRY_ConvertPanelImon(float adc_voltage)
{
    return adc_voltage / PANEL_CURRENT_GAIN_VA;
}
float TELEMETRY_ConvertSysVmon(float adc_voltage)
{
    return adc_voltage /SYS_VOLTAGE_GAIN_VV;
}
float TELEMETRY_ConvertSysImon(float adc_voltage)
{
    return adc_voltage / SYS_CURRENT_GAIN_VA;
}

float TELEMETRY_ConvertUmbVmon(float adc_voltage)
{
    return adc_voltage /SYS_VOLTAGE_GAIN_VV; 
}

float TELEMETRY_ConvertBattVmon(float adc_voltage)
{
    return adc_voltage /BATT_VOLTAGE_GAIN_VV; 
}
float TELEMETRY_ConvertBattImon(float adc_voltage)
{
    return adc_voltage /BATT_CURRENT_GAIN_VA; 
}
float TELEMETRY_ConvertBatTmon(float adc_voltage)
{
    if (adc_voltage <= 0.0f) {
        return 999.0f;   // short to GND / very hot / invalid
    }

    if (adc_voltage >= VCC) {
        return -999.0f;  // open circuit / very cold / invalid
    }
    float r_ntc = R_TOP * adc_voltage / (VCC - adc_voltage);

    float temp_k = 1.0f / 
        ((1.0f / T0) + (1.0f / BETA) * logf(r_ntc / R0));

    return temp_k - 273.15f;
}

