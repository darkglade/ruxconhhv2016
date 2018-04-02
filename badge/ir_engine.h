#ifndef __IR_ENGINE_H__
#define __IR_ENGINE_H__

#include "main.h"
#include "ir_common.h"

volatile IREngineEncoded irencoded;
volatile IREngineDecoder irdecoder;

void IREngine_DecodeInit();
void IREngine_EncodeInit(uint8_t);

uint8_t IREngine_Decode(IREngineDecoded *);
void IREngine_Resume();

uint8_t MATCH(uint16_t, uint16_t);
uint8_t MATCH_MARK(uint16_t, uint16_t);
uint8_t MATCH_SPACE(uint16_t, uint16_t);

void TIM3_IRQHandler();

#endif // __IR_ENGINE_H__
