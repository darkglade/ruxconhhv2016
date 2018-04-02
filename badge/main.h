#include <inttypes.h>
#include <stdio.h>
#include "usart_funcs.h"
#include "printf.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "ir_engine.h"
#include "ruxir.h"
#include "ruxir_proto.h"
#include "systick_utils.h"
#include "systick_config.h"
#include "chaser.h"

uint32_t g_badgeID;

#define FALSE 0
#define TRUE  1
