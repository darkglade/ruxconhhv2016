#include "main.h"
#include "ir_common.h"

uint8_t RuxIR_Decode(IREngineDecoded * results)
{
  uint64_t  data = 0;
  uint8_t   offset = 1; 

  if(!MATCH_MARK(results->rawbuf[offset++], RUXIR_HDR_MARK))  {
#ifdef DEBUG
    tfp_printf("%02d ", offset + 1);
    tfp_printf("HDR_MARK UNMATCHED\r\n");
#endif
    return FALSE; // No Header SPACE, bail out
  }
  if(!MATCH_MARK(results->rawbuf[offset++], RUXIR_HDR_SPACE)) {
#ifdef DEBUG
    tfp_printf("%02d ", offset + 1);
    tfp_printf("HDR_SPACE UNMATCHED\r\n");
#endif
    return FALSE; // No Header MARK, bail out
  }

  uint8_t i = 0;
  for(i = 0; i < RUXIR_BITS; i++) {
    if(!MATCH_MARK(results->rawbuf[offset++], RUXIR_BIT_MARK)) {
#ifdef DEBUG
      tfp_printf("%02d ", offset + 1);
      tfp_printf("BIT_MARK UNMATCHED\r\n");
#endif
      return FALSE; // No Bit MARK, bail out
    }
 
    if(MATCH_SPACE(results->rawbuf[offset], RUXIR_ONE_SPACE)) {
#ifdef DEBUG
      tfp_printf("%02d ", offset);
#endif
      data = (data << 1) | 1;
    } else if(MATCH_SPACE(results->rawbuf[offset], RUXIR_ZERO_SPACE)) {
#ifdef DEBUG
      tfp_printf("%02d ", offset);
#endif
      data = (data << 1) | 0;
    } else {
#ifdef DEBUG
      tfp_printf("%02d ONE_SPACE/ZERO_SPACE UNMATCHED val=%d\r\n", offset, results->rawbuf[offset]);
#endif
      return FALSE; // Time doesn't match ONE_SPACE or ZERO_SPACE, bail out
    }
    offset++;
  }

  results->value = (uint32_t)data;
  results->address = (uint32_t)(data >> 32);
  results->bits = RUXIR_BITS;
  
  return TRUE;
}

void RuxIR_SendRuxBadge(uint32_t addr, uint32_t data)
{
  if(irencoded.state == TXSTATE_IDLE) {
    irencoded.state  = TXSTATE_PEND;
    irencoded.offset = 0;
    irencoded.length = 0;
    irencoded.timer  = 0;
    
    int idx = 0;
    irencoded.data[idx++] = RUXIR_HDR_MARK  / USECPERTICK;
    irencoded.data[idx++] = RUXIR_HDR_SPACE / USECPERTICK;
    
    uint32_t mask;

    // Send address
    for (mask = 1UL << (24 - 1); mask; mask >>= 1) {
      irencoded.data[idx++] = RUXIR_BIT_MARK / USECPERTICK;
      if(addr & mask)
        irencoded.data[idx++] = RUXIR_ONE_SPACE  / USECPERTICK;
      else
        irencoded.data[idx++] = RUXIR_ZERO_SPACE / USECPERTICK;
    }   

    // Send data
    for (mask = 1UL << (32 - 1); mask; mask >>= 1) {
      irencoded.data[idx++] = RUXIR_BIT_MARK / USECPERTICK;
      if(data & mask)
        irencoded.data[idx++] = RUXIR_ONE_SPACE  / USECPERTICK;
      else
        irencoded.data[idx++] = RUXIR_ZERO_SPACE / USECPERTICK;
    }

    irencoded.data[idx++] = RUXIR_BIT_MARK / USECPERTICK;

    irencoded.length = idx;
    irencoded.state = TXSTATE_READY;
  }
}
