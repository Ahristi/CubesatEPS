#include "analog.h"

uint16_t adc_dma_buffer[ADC_DMA_BUFFER_LEN];

uint16_t adc_measurements[ADC_NUM_CHANNELS];         //Oversampled analog buffer
ANALOG_Measurement_t measurements[ADC_NUM_CHANNELS] = {
    [ADC_PANEL0_VMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertPanelVmon,
        .pack_measurement    = ANALOG_PackmVbit
    },

    [ADC_PANEL0_IMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertPanelImon,
        .pack_measurement    = ANALOG_PackmAbit
    },

    [ADC_PANEL1_VMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertPanelVmon,
        .pack_measurement    = ANALOG_PackmVbit
    },

    [ADC_PANEL1_IMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertPanelImon,
        .pack_measurement    = ANALOG_PackmAbit
    },

    [ADC_SYS_VMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertSysVmon,
        .pack_measurement    = ANALOG_PackmVbit
    },

    [ADC_SYS_IMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertSysImon,
        .pack_measurement    = ANALOG_PackmAbit
    },

    [ADC_BAT_TMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertBatTmon,
        .pack_measurement    = ANALOG_PackCelsiusbit
    },

    [ADC_UMB_VMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertUmbVmon,
        .pack_measurement    = ANALOG_PackmVbit
    },

    [ADC_BATT_VMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertBattVmon,
        .pack_measurement    = ANALOG_PackmVbit
    },

    [ADC_BATT_IMON] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertBattImon,
        .pack_measurement    = ANALOG_PackmAbit 
    },

    [ADC_VREFINT] = {
        .raw = 0,
        .voltage = 0.0f,
        .value = 0.0f,
        .packed_value = 0,
        .convert_measurement = ANALOG_ConvertVREF,
        .pack_measurement    = ANALOG_PackmVbit //Note that this function isn't actually used bc we don't transmit VREF
    }
};

void ANALOG_Init(void)
{
    HAL_TIM_Base_Start(&htim2);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer,ADC_DMA_BUFFER_LEN); 
}


void ANALOG_Task(void)
{
    uint16_t adc_snapshot[ADC_NUM_CHANNELS];
    __disable_irq();
    memcpy(adc_snapshot, adc_measurements, sizeof(adc_snapshot));
    __enable_irq();
    uint16_t raw_vref          = adc_snapshot[ADC_VREFINT];
    float vref                 = ANALOG_CalibrateVDDA(raw_vref);

    for (uint16_t i = 0; i < ADC_NUM_CHANNELS; i++)
    {
        measurements[i].raw            = adc_snapshot[i];
        measurements[i].voltage        = ANALOG_RawToAdcVoltage(measurements[i].raw, vref);
        //Convert ADC voltage to the actual measurement (ie ADC voltage -> temperature)
        measurements[i].value          = measurements[i].convert_measurement(measurements[i].voltage);
        //Pack measurement into telemetry format  
        measurements[i].packed_value   = measurements[i].pack_measurement(measurements[i].value);
    }
}

void ANALOG_overSampleADC(const uint16_t *buffer)
{
    //Note: GPIO toggle pin is in here to measure the time taken to perform the oversample
    HAL_GPIO_WritePin(BMS_nINT_GPIO_Port, BMS_nINT_Pin, GPIO_PIN_SET);
    for (uint16_t ch = 0; ch < ADC_NUM_CHANNELS; ch++)
    {
        uint32_t sum = 0;

        for (uint16_t sample = 0; sample < ADC_NUM_SAMPLES; sample++)
        {
            sum += buffer[sample * ADC_NUM_CHANNELS + ch];
        }
        adc_measurements[ch] = (uint16_t)(sum / ADC_NUM_SAMPLES);
    }
    HAL_GPIO_WritePin(BMS_nINT_GPIO_Port, BMS_nINT_Pin, GPIO_PIN_RESET);
}


float ANALOG_CalibrateVDDA(uint16_t raw_vrefint)
{
    uint16_t vrefint_cal = *VREFINT_CAL_ADDR;
    return VREFINT_CAL_VOLTAGE *
           ((float)vrefint_cal / (float)raw_vrefint);
}

float ANALOG_RawToAdcVoltage(uint16_t raw, float vdda)
{
    return ((float)raw * vdda) / ADC_MAX_COUNTS;
}
float ANALOG_ConvertPanelVmon(float adc_voltage)
{
    return adc_voltage / PANEL_VOLTAGE_GAIN_VV;
}
float ANALOG_ConvertPanelImon(float adc_voltage)
{
    return adc_voltage / PANEL_CURRENT_GAIN_VA;
}
float ANALOG_ConvertSysVmon(float adc_voltage)
{
    return adc_voltage /SYS_VOLTAGE_GAIN_VV;
}
float ANALOG_ConvertSysImon(float adc_voltage)
{
    return adc_voltage / SYS_CURRENT_GAIN_VA;
}

float ANALOG_ConvertUmbVmon(float adc_voltage)
{
    return adc_voltage /SYS_VOLTAGE_GAIN_VV; 
}

float ANALOG_ConvertBattVmon(float adc_voltage)
{
    return adc_voltage /BATT_VOLTAGE_GAIN_VV; 
}
float ANALOG_ConvertBattImon(float adc_voltage)
{
    return (adc_voltage-BATT_CURRENT_OFFSET_V)/BATT_CURRENT_GAIN_VA; 
}
float ANALOG_ConvertBatTmon(float adc_voltage)
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
float ANALOG_ConvertVREF(float raw_voltage)
{
    return raw_voltage;
}



//------------------Telemetry Packing---------------------
uint16_t ANALOG_PackmVbit(float value_V)
{
    int32_t scaled = (int32_t)lroundf(value_V * 1000.0f);

    if (scaled < 0) scaled = 0;
    if (scaled > UINT16_MAX) scaled = UINT16_MAX;

    return (uint16_t)scaled;
}

uint16_t ANALOG_PackmAbit(float value_A)
{
    int32_t scaled = (int32_t)lroundf(value_A * 1000.0f);

    if (scaled < 0) scaled = 0;
    if (scaled > UINT16_MAX) scaled = UINT16_MAX;

    return (uint16_t)scaled;
}

uint16_t ANALOG_PackSignedmAbit(float value_A)
{
    int32_t scaled = (int32_t)lroundf(value_A * 1000.0f);

    if (scaled > INT16_MAX) scaled = INT16_MAX;
    if (scaled < INT16_MIN) scaled = INT16_MIN;

    return (uint16_t)((int16_t)scaled);
}

uint16_t ANALOG_PackCelsiusbit(float value_C)
{
    int32_t scaled = (int32_t)lroundf(value_C);

    if (scaled > INT16_MAX) scaled = INT16_MAX;
    if (scaled < INT16_MIN) scaled = INT16_MIN;

    return (uint16_t)((int16_t)scaled);
}






//------------------DMA INTERRUPTS---------------------
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        ANALOG_overSampleADC(&adc_dma_buffer[0]);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        ANALOG_overSampleADC(&adc_dma_buffer[ADC_NUM_CHANNELS * ADC_NUM_SAMPLES]);
    }
}