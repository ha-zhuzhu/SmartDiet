#ifndef __SYS_H
#define __SYS_H
#include "stm32f4xx.h"

//先计算别名区对应的地址
#define BITBAND(addr, bit) (((addr & 0xf0000000) + 0x2000000) + ((addr & 0xfffff) * 8 + bit) * 4)

//操作别名区
#define MEM_ADDR(addr, bit) *(volatile unsigned int *)BITBAND(addr, bit)

//ODR
#define PAout(bit) MEM_ADDR((unsigned int)&GPIOA->ODR, bit)
#define PBout(bit) MEM_ADDR((unsigned int)&GPIOB->ODR, bit)
#define PCout(bit) MEM_ADDR((unsigned int)&GPIOC->ODR, bit)
#define PDout(bit) MEM_ADDR((unsigned int)&GPIOD->ODR, bit)
#define PEout(bit) MEM_ADDR((unsigned int)&GPIOE->ODR, bit)
#define PFout(bit) MEM_ADDR((unsigned int)&GPIOF->ODR, bit)
#define PGout(bit) MEM_ADDR((unsigned int)&GPIOG->ODR, bit)
#define PHout(bit) MEM_ADDR((unsigned int)&GPIOG->ODR, bit)

//IDR
#define PAin(bit) MEM_ADDR((unsigned int)&GPIOA->IDR, bit)
#define PBin(bit) MEM_ADDR((unsigned int)&GPIOB->IDR, bit)
#define PCin(bit) MEM_ADDR((unsigned int)&GPIOC->IDR, bit)
#define PDin(bit) MEM_ADDR((unsigned int)&GPIOD->IDR, bit)
#define PEin(bit) MEM_ADDR((unsigned int)&GPIOE->IDR, bit)
#define PFin(bit) MEM_ADDR((unsigned int)&GPIOF->IDR, bit)
#define PGin(bit) MEM_ADDR((unsigned int)&GPIOG->IDR, bit)
#define PHin(bit) MEM_ADDR((unsigned int)&GPIOG->IDR, bit)

void NVIC_Configuration(void);

#endif
