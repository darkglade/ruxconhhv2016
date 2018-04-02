/*
  RuxIR_common.h

  This is basically a cut down port of the Arduino IRRemote library

*/

#ifndef __IR_COMMON_H__
#define __IR_COMMON_H__

#include "main.h"

#define RAWLEN 121

#define IRIN_PORT GPIOA
#define IRIN_PIN  GPIO_Pin_9

typedef struct {
  uint8_t   rcvstate;
  uint8_t   rawlen;
  uint16_t  timer;
  uint16_t  rawbuf[RAWLEN];
  uint8_t   overflow;
} IREngineDecoder;

typedef struct {
  uint8_t   state;
  uint8_t   offset;
  uint8_t   length;
  uint8_t   data[RAWLEN];
  uint16_t  timer;
} IREngineEncoded;

typedef struct {
  uint8_t             decoded;
  uint32_t            address;
  uint32_t            value;
  uint8_t             bits;
  volatile uint16_t * rawbuf;
  uint8_t             rawlen;
  uint8_t             overflow;
} IREngineDecoded;

// Transmit states
#define TXSTATE_IDLE    2
#define TXSTATE_PEND    3
#define TXSTATE_READY   4

// Receive state machine status
#define STATE_IDLE      2
#define STATE_MARK      3
#define STATE_SPACE     4
#define STATE_STOP      5
#define STATE_OVERFLOW  6

#define MARK_EXCESS 50
 
#define USECPERTICK 50

#define TOLERANCE   25
#define LTOL        (1.0 - (TOLERANCE/100.))
#define UTOL        (1.0 + (TOLERANCE/100.))

#define _GAP        5000
#define GAP_TICKS   (_GAP/USECPERTICK)

#define TICKS_LOW(us)  ((int)(((us)*LTOL/USECPERTICK)))
#define TICKS_HIGH(us) ((int)(((us)*UTOL/USECPERTICK + 1)))

#define MARK        0
#define SPACE       1

#endif // __IR_COMMON_H__
