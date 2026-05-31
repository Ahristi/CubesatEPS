#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <stdint.h>
#include "powerpath.h"
#include "analog.h"

typedef void (*TaskFunction)();

typedef struct{
    TaskFunction function;
    uint32_t period_ms;
    uint32_t last_run_ms;
    uint32_t duration;
}Task;

void SCHEDULER_Init();
void SCHEDULER_Run();
void default_Task();

#endif