#ifndef __RUXIR_H__
#define __RUXIR_H__

#include "main.h"
#include "ir_common.h"

#define RUXIR_BITS          56
#define RUXIR_HDR_MARK     900
#define RUXIR_HDR_SPACE   1800
#define RUXIR_BIT_MARK     250
#define RUXIR_ONE_SPACE    700
#define RUXIR_ZERO_SPACE   300

void RuxIR_SendRuxBadge(uint32_t, uint32_t);
uint8_t RuxIR_Decode(IREngineDecoded *);

#endif //__RUXIR_H__
