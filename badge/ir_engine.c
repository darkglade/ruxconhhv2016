#include "ir_engine.h"
#include "ruxir.h"

void IREngine_DecodeInit()
{
  TIM_TimeBaseInitTypeDef TIM_TBInitStruct;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // Setup a 50uS timer
  TIM_TBInitStruct.TIM_Prescaler = 0;
  TIM_TBInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TBInitStruct.TIM_Period = 399;
  TIM_TBInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TBInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TBInitStruct);
  
  // Enable Update Interrupt
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  
  // Enable Timer
  TIM_Cmd(TIM3, ENABLE);

  // Enable IRQ Routing
  NVIC_EnableIRQ(TIM3_IRQn);

  // Initialise irdecoder
  irdecoder.rcvstate = STATE_IDLE;
  irdecoder.rawlen = 0;
}

void IREngine_EncodeInit(uint8_t KHz)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  TIM_TimeBaseInitTypeDef TIM_TBInit;
  TIM_OCInitTypeDef TIM_OCInit;

  // Enable clock to GPIOB
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  // PB1 is our output
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  // AF0 on PB1 is TIM14_CH1
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_0);
  
  // Enable clock to TIM14
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

  // Configure TIM14 timebase
  TIM_TBInit.TIM_Prescaler = 0;
  TIM_TBInit.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TBInit.TIM_Period = (SystemCoreClock / 2 / (KHz*1000)) - 1; 
  TIM_TBInit.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TBInit.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM14, &TIM_TBInit);

  // Configure TIM14 output compare mode to push our signal out PB1
  TIM_OCInit.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInit.TIM_OutputState = TIM_OutputState_Disable; // We start disabled
  TIM_OCInit.TIM_Pulse = 0;
  TIM_OC1Init(TIM14, &TIM_OCInit);
  
  // Disable preload
  TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Disable);

  // Enable TIM14
  TIM_Cmd(TIM14, ENABLE);

  irencoded.state = TXSTATE_IDLE;
}

uint8_t IREngine_Decode(IREngineDecoded * results)
{
  results->rawbuf   = irdecoder.rawbuf;
  results->rawlen   = irdecoder.rawlen;
  results->overflow = irdecoder.overflow;

  if(irdecoder.rcvstate != STATE_STOP) return FALSE;

  if(RuxIR_Decode(results)) return TRUE;

  IREngine_Resume();

  return FALSE;
}

void IREngine_Resume()
{
  irdecoder.rcvstate = STATE_IDLE;
  irdecoder.rawlen = 0;
}

uint8_t MATCH(uint16_t measured, uint16_t desired)
{
  uint8_t passed = ((measured >= TICKS_LOW(desired)) && (measured <= TICKS_HIGH(desired)));
  return passed;
}

uint8_t MATCH_MARK(uint16_t measured_ticks, uint16_t desired_us)
{
#ifdef DEBUG
  tfp_printf("TESTING MARK  %d-%d  %d\r\n", TICKS_LOW(desired_us + MARK_EXCESS), TICKS_HIGH(desired_us + MARK_EXCESS), measured_ticks);
#endif
  uint8_t passed = ((measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS))
                  && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS)));
  return passed;
}

uint8_t MATCH_SPACE(uint16_t measured_ticks, uint16_t desired_us)
{
#ifdef DEBUG
  tfp_printf("TESTING SPACE %d-%d  %d\r\n", TICKS_LOW(desired_us - MARK_EXCESS), TICKS_HIGH(desired_us - MARK_EXCESS), measured_ticks);
#endif
  uint8_t passed = ((measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS))
                  && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS)));
  return passed;
}

void TIM3_IRQHandler()
{
  if(TIM3->SR & TIM_SR_UIF) {

    ////////////////////////////////////////////////////////////////////////
    /// RECEIVE CODE
    ////////////////////////////////////////////////////////////////////////
    uint8_t irdata = GPIO_ReadInputDataBit(IRIN_PORT, IRIN_PIN); // Read pin

    irdecoder.timer++; // Tick
    if(irdecoder.rawlen >= RAWLEN) irdecoder.rcvstate = STATE_OVERFLOW; // Buffer overflowed

    switch(irdecoder.rcvstate) {
      case STATE_IDLE:  // In a gap
        if(irdata == MARK) {
          if(irdecoder.timer < GAP_TICKS) { // Too short to be a gap
            irdecoder.timer = 0;
          } else {  // Gap just ended 
            irdecoder.overflow                   = FALSE;
            irdecoder.rawlen                     = 0;
            irdecoder.rawbuf[irdecoder.rawlen++]  = irdecoder.timer;
            irdecoder.timer                      = 0;
            irdecoder.rcvstate                   = STATE_MARK;
          }
        }
        break;
      case STATE_MARK: // Measuring mark
        if(irdata == SPACE) { // Just ended mark
          irdecoder.rawbuf[irdecoder.rawlen++]  = irdecoder.timer;
          irdecoder.timer                      = 0;
          irdecoder.rcvstate                   = STATE_SPACE;
        }
        break;
      case STATE_SPACE: // Measuring space
        if(irdata == MARK) { // Just ended space
          irdecoder.rawbuf[irdecoder.rawlen++]  = irdecoder.timer;
          irdecoder.timer                      = 0;
          irdecoder.rcvstate                   = STATE_MARK;
        } else if(irdecoder.timer > GAP_TICKS) { // Are we in a gap?
          irdecoder.rcvstate = STATE_STOP;
        }
        break;
      case STATE_STOP: // Waiting
        if(irdata == MARK) irdecoder.timer = 0;  // Reset gap timer
        break;
      case STATE_OVERFLOW:  // Overflowed bail out
        irdecoder.overflow = TRUE;
        irdecoder.rcvstate = STATE_STOP;
        break;
    }

    /////////////////////////////////////////////////////////////////////
    /// TRANSMIT CODE
    /////////////////////////////////////////////////////////////////////
    if(irencoded.state == TXSTATE_READY)
    {
      if(irencoded.timer == 0 && irencoded.offset < irencoded.length) {
        irencoded.timer = irencoded.data[irencoded.offset++]; // Load the next tick count into timer
        TIM14->CCER ^= 0x0001; // Toggle timer output
      } else if(irencoded.timer > 0) {
        irencoded.timer--;
      } else if(irencoded.offset >= irencoded.length) {
        TIM14->CCER &= 0xFFFE; // Make sure our output is turned off when we're done
        irencoded.offset = 0;
        irencoded.length = 0;
        irencoded.state  = TXSTATE_IDLE;
      }   
    }

    TIM3->SR &= ~TIM_SR_UIF; // Clear Update Interrupt
  }
}
