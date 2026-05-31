#include "scheduler.h"



Task tasks[] = {
        {default_Task,    1000, 0, 0},
        {ANALOG_Task,      100, 0, 0},
        {MPPT_Task,        100, 0, 0},
        {POWERPATH_Task,   100, 0, 0},
    };


#define NUM_TASKS ((uint8_t)(sizeof(tasks) / sizeof(tasks[0])))

void SCHEDULER_Init()
{
    uint32_t now = HAL_GetTick();

    for (uint8_t i = 0; i < NUM_TASKS; i++)
    {
        tasks[i].last_run_ms = now;
    }
}
void SCHEDULER_Run(void)
{
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0; i < NUM_TASKS; i++)
    {
        if ((now - tasks[i].last_run_ms) >= tasks[i].period_ms)
        {
            tasks[i].last_run_ms = now;
            tasks[i].function();
            tasks[i].duration = HAL_GetTick() - tasks[i].last_run_ms;
        }
    }
}
void default_Task()
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
}