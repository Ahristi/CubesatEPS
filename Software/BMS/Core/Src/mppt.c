#include "mppt.h"


MPPT_Handler_t hmppt0;
MPPT_Handler_t hmppt1;



void MPPT_Init(void)
{
    hmppt0.htim    = &htim3;
    hmppt0.channel = TIM_CHANNEL_1;
    hmppt0.duty    = 500;
    hmppt0.enable  = false;

    hmppt0.htim    = &htim3;
    hmppt0.channel = TIM_CHANNEL_2;
    hmppt0.duty    = 500;
    hmppt0.enable  = false;
}


void MPPT_task(void)
{

}

bool MPPT_enable(MPPT_Handler_t* hmppt0)
{
    return true;
}

bool MPPT_disable(MPPT_Handler_t* hmppt0)
{
    return true;
}