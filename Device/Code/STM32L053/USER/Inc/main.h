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

#include "string.h"
#include "stdio.h"

void Error_Handler(void);

#define fastAbs(n) n > 0 ? n : -n

#endif /* __MAIN_H */