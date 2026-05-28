#ifndef __MPPT_H__
#define __MPPT_H__
#include <stdbool.h>
#include "tim.h"
//-------------Defines-------------

//-------------Typedef and Enums-------------
typedef struct{
    TIM_HandleTypeDef* htim;
    uint32_t channel;
    uint16_t duty;

    bool enable;
}MPPT_Handler_t;


extern MPPT_Handler_t hmppt0;
extern MPPT_Handler_t hmppt1;
//-------------Function Prototypes-------------
void MPPT_Init(void);
void MPPT_task(void);
bool MPPT_disable(MPPT_Handler_t* hmppt);
bool MPPT_enable(MPPT_Handler_t* hmppt);

#endif