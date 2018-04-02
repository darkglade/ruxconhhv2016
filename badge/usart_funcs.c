#include "usart_funcs.h"
#include "printf.h"

void putChar(void* p, char ch)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {}

  USART_SendData(USART1, ch);
}

void USARTConfig()
{
  // Enable clock to GPIOB
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  // Enable clock to USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  // Setup the USART GPIO
  GPIO_InitTypeDef GPIO_InitStruct;

  // Configure TX GPIO
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Set Alternate Function mode on PA9 and PA10 to USART_TX and USART_RX
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_0); // USART1_RX
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_0); // USART1_TX

  // Configure the USART
  USART_InitTypeDef USART_InitStruct;

  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

  USART_Init(USART1, &USART_InitStruct);  

  USART_Cmd(USART1, ENABLE);
  
  init_printf(NULL, putChar);
}

