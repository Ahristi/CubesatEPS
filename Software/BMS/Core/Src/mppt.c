#include "mppt.h"


MPPT_Handler_t hmppt0;
MPPT_Handler_t hmppt1;



void MPPT_Init(void)
{
    hmppt0.htim    = &htim3;
    hmppt0.channel = TIM_CHANNEL_2;
    hmppt0.duty    = (uint16_t) MPPT_PWM_PERIOD/2;
    hmppt0.enable  = false;
    hmppt0.direction = 1;

    hmppt1.htim    = &htim3;
    hmppt1.channel = TIM_CHANNEL_1;
    hmppt1.duty    = (uint16_t) MPPT_PWM_PERIOD/2;
    hmppt1.enable  = false;
    hmppt1.direction = 1;

    MPPT_disable(&hmppt0);
    MPPT_disable(&hmppt1);
}


void MPPT_Task(void)
{
    
    float panel0_voltage = measurements[ADC_PANEL0_VMON].value;
    float panel0_current = measurements[ADC_PANEL0_IMON].value;
    hmppt0.power_now = panel0_voltage*panel0_current;
    hmppt0.output_voltage = measurements[ADC_BATT_VMON].value;

    float panel1_voltage = measurements[ADC_PANEL1_VMON].value;
    float panel1_current = measurements[ADC_PANEL1_IMON].value;
    hmppt1.power_now = panel1_voltage*panel1_current;
    hmppt1.output_voltage = measurements[ADC_BATT_VMON].value;

    MPPT_update(&hmppt0);
    MPPT_update(&hmppt1);
}

bool MPPT_enable(MPPT_Handler_t* hmppt)
{
    if (hmppt == NULL || hmppt->htim == NULL)
    {
        return false;
    }

    __HAL_TIM_SET_COMPARE(hmppt->htim, hmppt->channel, hmppt->duty);

    if (HAL_TIM_PWM_Start(hmppt->htim, hmppt->channel) != HAL_OK)
    {
        hmppt->enable = false;
        return false;
    }

    hmppt->enable = true;
    return true;
}

bool MPPT_disable(MPPT_Handler_t* hmppt)
{
    if (hmppt == NULL || hmppt->htim == NULL)
    {
        return false;
    }

    __HAL_TIM_SET_COMPARE(hmppt->htim, hmppt->channel, 0);

    if (HAL_TIM_PWM_Stop(hmppt->htim, hmppt->channel) != HAL_OK)
    {
        return false;
    }

    hmppt->enable = false;
    return true;
}

bool MPPT_setDuty(MPPT_Handler_t* hmppt, uint32_t duty)
{
    if (hmppt == NULL || hmppt->htim == NULL)
    {
        return false;
    }

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(hmppt->htim);

    if (duty > arr)
    {
        duty = arr;
    }

    hmppt->duty = duty;

    if (hmppt->enable)
    {
        __HAL_TIM_SET_COMPARE(hmppt->htim, hmppt->channel, hmppt->duty);
    }

    return true;
}



void MPPT_update(MPPT_Handler_t *hmppt)
{
    if (!hmppt->enable)
    {
        return;
    }    
    if (hmppt->output_voltage > MPPT_MAX_OUTPUT_VOLTAGE)
    {
        //If we are at the max MPPT output voltage. Do nothing.
        //Otherwise if there is no load the boost converter can go super high.
        hmppt->duty    = hmppt->duty;
    }
    else if (hmppt->power_now > hmppt->power_previous)
    {
        //If the power has increased, then the last perturbation worked, so keep going in that direction
        hmppt->duty += hmppt->direction * MPPT_STEP;
    }
    else
    {
        //If power decreases we have overshot the maximum power point, so go backwards.
        hmppt->direction = -hmppt->direction;
        hmppt->duty += hmppt->direction * MPPT_STEP;
    }
    if (hmppt->duty > MPPT_MAX_DUTY_CYCLE)
    {
        hmppt->duty = MPPT_MAX_DUTY_CYCLE;
    }
    hmppt->power_previous = hmppt->power_now;

    MPPT_setDuty(hmppt, hmppt->duty);
}