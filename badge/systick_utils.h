#ifndef __SYSTICK_UTILS_H__
#define __SYSTICK_UTILS_H__

#include "main.h"

#define SYSTICK_TASKS 2

#define ST_STATE_DISARMED 1
#define ST_STATE_ARMED    2
#define ST_STATE_PENDING  3

void SysTick_Init(uint32_t);
uint32_t SysTick_Millis();
void SysTick_Delay(uint32_t);

void SysTick_AddTask(uint8_t, uint16_t);
void SysTick_AddTaskFn(uint8_t, uint16_t, void *);
uint8_t SysTick_GetTaskState(uint8_t);
void SysTick_ProcessTasks();

void SysTick_Handler(void);

typedef struct
{
  uint8_t state;
  uint16_t counts;
  void (*func)();
} SysTick_Tasks;

#endif // __SYSTICK_UTIL_H__
