
#include "systick_utils.h"

SysTick_Tasks _systick_tasks[SYSTICK_TASKS];
volatile uint32_t SysTick_ticks = 0;

uint32_t SysTick_Millis()
{
  return SysTick_ticks;
}

void SysTick_Delay(uint32_t ticks)
{
  uint32_t start = 0, end = 0;
  start = SysTick_Millis();
  end = start + ticks;
  if(start < end) {
    while((SysTick_Millis() >= start) && (SysTick_Millis() < end)) {
      __asm(""); // nop to avoid getting optimised out
    }
  } else {
    while((SysTick_Millis() >= start) || (SysTick_Millis() < end)) {
      __asm(""); // nop to avoid getting optimised out
    }  
  };
}

void SysTick_tick()
{
  SysTick_ticks++;
}

void SysTick_Init(uint32_t usTick)
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / usTick);
}

/// TASKING FUNCTIONS
void SysTick_ProcessTasks()
{
  uint8_t i = 0;
  for (; i < SYSTICK_TASKS; i++) {
    SysTick_Tasks *t;
    
    t = &_systick_tasks[i];
    if(t->state == ST_STATE_ARMED) {
      if(t->counts) {
        t->counts--;
      } else {
        if(t->func != NULL) {
          t->state = ST_STATE_DISARMED;
          (t->func)();
        } else {
          t->state = ST_STATE_PENDING;
        }
      }
    }
  }
}


void SysTick_AddTaskFn(uint8_t i, uint16_t c, void * f)
{
  SysTick_Tasks *t;

  t = &_systick_tasks[i];
  t->state = ST_STATE_ARMED;
  t->counts = c;
  t->func = f;
}

void SysTick_AddTask(uint8_t i, uint16_t c)
{
  SysTick_AddTaskFn(i, c, NULL);
}

uint8_t SysTick_GetTaskState(uint8_t i)
{
  SysTick_Tasks *t;

  t = &_systick_tasks[i];
  return t->state;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  SysTick_ticks++;
  SysTick_ProcessTasks();
}

