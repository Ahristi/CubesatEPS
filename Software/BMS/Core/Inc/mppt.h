#ifndef __MPPT_H__
#define __MPPT_H__
#include <stdbool.h>
#include "tim.h"
#include "analog.h"
//-------------Defines-------------
#define MPPT_MAX_OUTPUT_VOLTAGE 16.0f //Before taper charge to be safe
#define MPPT_STEP 1
#define MPPT_PWM_PERIOD 100
#define MPPT_MAX_DUTY_CYCLE 80

//-------------Typedef and Enums-------------
typedef struct{
    TIM_HandleTypeDef* htim;
    uint32_t channel;
    uint16_t duty;
    
    float power_now;
    float power_previous;
    uint8_t direction;
    float output_voltage;

    bool enable;
}MPPT_Handler_t;


extern MPPT_Handler_t hmppt0;
extern MPPT_Handler_t hmppt1;
//-------------Function Prototypes-------------
void MPPT_Init(void);
void MPPT_Task(void);
bool MPPT_disable(MPPT_Handler_t* hmppt);
bool MPPT_enable(MPPT_Handler_t* hmppt);
bool MPPT_setDuty(MPPT_Handler_t* hmppt, uint32_t duty);
void MPPT_update(MPPT_Handler_t *hmppt);
#endif