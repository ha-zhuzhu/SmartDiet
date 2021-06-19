#ifndef __MAIN_H
#define __MAIN_H

#include "stm32l0xx_hal.h"

#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_adc.h"

#include "string.h"
#include "stdio.h"

//#define USE_DEVELOPMENT_BOARD
//#define DEBUG_MODE
//#define FAST_PSM  //BC28每次上传结束都快速进入PSM，不好使

#define fastAbs(n) n > 0 ? n : -n

void SystemPower_Config(void);
void STOPMode_Enter(void);
void STOPMode_Recover(void);
void Error_Handler(void);

#endif /* __MAIN_H */
