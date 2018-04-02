#ifndef __USART_FUNCS_H__
#define __USART_FUNCS_H__

#include <stdio.h>
#include <inttypes.h>
#include "printf.h"
#include "stm32f0xx_conf.h"

void putChar(void *, char);
void USARTConfig();

#endif // __USART_FUNCS_H__
