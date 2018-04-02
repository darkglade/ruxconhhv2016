#include "main.h"

#define STM32_UUID ((uint16_t *)0x1FFFF7AC) // Refer to Cortex-M0 programming manual

void GPIO_SetBit(GPIO_TypeDef*, uint32_t);
void GPIOConfig();
void ComputeID();
void SayHello();
void DisplayIR(IREngineDecoded *);
void StepChaser();

uint32_t g_badgeID = 0;
uint8_t g_chaserStep = 0;

int main()
{
  SysTick_Init(1000);
  GPIOConfig();
  USARTConfig();
  ComputeID();
  SayHello();

  IREngine_DecodeInit();
  IREngine_EncodeInit(40);

  SysTick_AddTask(SYSTICK_CHIRP, 1000);
  Chaser_SetPattern(Chaser_StepBounce, 150);


  while (1) {
    IREngineDecoded results;

    SysTick_Delay(800);
 
    if(IREngine_Decode(&results)) {
      DisplayIR(&results);
      IREngine_Resume();  
    }

    if(SysTick_GetTaskState(SYSTICK_CHIRP) == ST_STATE_PENDING)
    {
      RuxIRProto_SendChrip();
      SysTick_AddTask(SYSTICK_CHIRP, 1000);
    }
  }

  return 0;
}

void DisplayIR(IREngineDecoded * ruxir)
{
  if(ruxir->overflow) {
    tfp_printf("Overflow\r\n");
  } else {
    tfp_printf("Address: %06X Data: %08X\r\n", ruxir->address, ruxir->value);
  }
}

void SayHello()
{
  tfp_printf("Hello, World!\r\n Badge ID=0x%06X\r\n", g_badgeID);
}


void ComputeID()
{
  uint8_t * buf = (uint8_t *)STM32_UUID;
  uint8_t tmp[3] = { 0x00, 0x00, 0x00 };

  // XOR each 24-bit chunk of the UUID with the rest
  int i = 0;
  for(i = 0; i < 4; i++) {
    uint8_t idx = i * 3;
    tmp[0] ^= buf[idx];
    tmp[1] ^= buf[idx + 1];  
    tmp[2] ^= buf[idx + 2];
  }

  // Convert badge ID to uint32_t
  for(i = 0; i < 3; i++) {
    g_badgeID ^= tmp[2-i] << 8*i;
  }
  
  g_badgeID &= 0x007FFFFF; // mask top bit of ID to accommodate beacon addresses
}

void GPIOConfig()
{
  GPIO_InitTypeDef GPIO_InitStruct;

  // Enable clock routing to GPIOA
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  // Setup our LED GPIOs
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // Call to phony method which has our static analysis flag embedded
  GPIO_SetBit(GPIOF, GPIO_Pin_7);
}

void GPIO_SetBit(GPIO_TypeDef * GPIOx, uint32_t GPIO_Pin)
{
  __asm volatile (
    "b F7A6           \n"
    ".word 0x9321d192 \n"
    ".word 0x89d9ac81 \n"
    ".word 0xac855879 \n"
    ".word 0x11d17bf3 \n"
    "F7A6:            \n"
  );
}
