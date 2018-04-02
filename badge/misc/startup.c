
#include "main.h"

// external refs to linker script 
extern unsigned long _sidata;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _estack;

// reset handler, entry point to user code
void Reset_Handler(void)
{
  unsigned long *src;
  unsigned long *dest;
  
  // Copy data from flash to RAM
  src = &_sidata;
  for(dest = &_sdata; dest < &_edata;) {
    *(dest++) = *(src++);
  }

 	// Initialise bss section with zeros
  for(dest = &_sbss; dest < &_ebss;) {
    *(dest++) = 0;
  }

  // Call SystemInit from system_st32f0xx.h to setup clocks and such
  SystemInit();
  // Call main
  main();

  while(1) {} // just in case we crash through main
}

/* Blank interrupt service routine symbols

  These symbols are not defined in the default stm32f0xx_it.h
  so we define them as weak symbols here to allow them to be
  implemented without having to yutz with this script.

*/
void __attribute__((weak)) NMI_Handler(void) {}
void __attribute__((weak)) HardFault_Handler(void) {}
void __attribute__((weak)) SVC_Handler(void) {}
void __attribute__((weak)) PendSV_Handler(void) {}
void __attribute__((weak)) SysTick_Handler(void) {}
///////////////
void __attribute__((weak)) WWDG_IRQHandler(void) {}
void __attribute__((weak)) PVD_IRQHandler(void) {}
void __attribute__((weak)) RTC_IRQHandler(void) {}
void __attribute__((weak)) FLASH_IRQHandler(void) {}
void __attribute__((weak)) RCC_IRQHandler(void) {}
void __attribute__((weak)) EXTI0_1_IRQHandler(void) {}
void __attribute__((weak)) EXTI2_3_IRQHandler(void) {}
void __attribute__((weak)) EXTI4_15_IRQHandler(void) {}
void __attribute__((weak)) DMA1_Channel1_IRQHandler(void) {}
void __attribute__((weak)) DMA1_Channel2_3_IRQHandler(void) {}
void __attribute__((weak)) DMA1_Channel4_5_IRQHandler(void) {}
void __attribute__((weak)) ADC1_IRQHandler(void) {}
void __attribute__((weak)) TIM1_BRK_UP_TRG_COM_IRQHandler(void) {}
void __attribute__((weak)) TIM1_CC_IRQHandler(void) {}
void __attribute__((weak)) TIM2_IRQHandler(void) {}
void __attribute__((weak)) TIM3_IRQHandler(void) {}
void __attribute__((weak)) TIM14_IRQHandler(void) {}
void __attribute__((weak)) TIM16_IRQHandler(void) {}
void __attribute__((weak)) TIM17_IRQHandler(void) {}
void __attribute__((weak)) I2C1_IRQHandler(void) {}
void __attribute__((weak)) SPI1_IRQHandler(void) {}
void __attribute__((weak)) USART1_IRQHandler(void) {}
void __attribute__((weak)) BootRAM(void) {}


/* Interrupt Vectors

	Details of the interrupt and exception vectors can be found in
	Section 12.1.3 of RM0091 - STM32F0x1/STM32F0x2/STM32F0x8
	advanced ARM based 32-bit MCUs

  Names in use are CMSIS names for compatibility
*/
__attribute__ ((section(".isr_vector")))
void (* const interrupt_vector_table[])(void) =
{
  (void *) &_estack,	// Index 0: Reserved (Stack End)
  Reset_Handler,		// Index 1: Reset
  NMI_Handler,		// Index 2: NMI
  HardFault_Handler,	// Index 3: HardFault
  0,			// ZERO Fill to 0x2c
  0,
  0,
  0,
  0,
  0,
  0,
  SVC_Handler,		// Index 4: SVCall
  0,			// ZERO Fill to 0x38
  0,
  PendSV_Handler,		// Index 5: PendSV
  SysTick_Handler,	// Index 6: SysTick
  /////////////////
  WWDG_IRQHandler,
  PVD_IRQHandler,
  RTC_IRQHandler,
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_1_IRQHandler,
  EXTI2_3_IRQHandler,
  EXTI4_15_IRQHandler,
  0,
  DMA1_Channel1_IRQHandler,
  DMA1_Channel2_3_IRQHandler,
  DMA1_Channel4_5_IRQHandler,
  ADC1_IRQHandler,
  TIM1_BRK_UP_TRG_COM_IRQHandler,
  TIM1_CC_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  0,
  0,
  TIM14_IRQHandler,
  0,
  TIM16_IRQHandler,
  TIM17_IRQHandler,
  I2C1_IRQHandler,
  0,
  SPI1_IRQHandler,
  0,
  USART1_IRQHandler,
  0,
  0,
  0,
  0,
  (void*)0xF108F85F 
};

