#ifndef __ADC_H
#define __ADC_H

#include "main.h"

#define USE_TIMEOUT 0   //adc 超时
#define VDDA_ADC_Times 5
/* Raw data acquired at temperature of 25 °C VDDA = 3 V */
#define VREFINT_ADDR 0x1FF80078

/* Delay between ADC end of calibration and ADC enable.                     */
/* Delay estimation in CPU cycles: Case of ADC enable done                  */
/* immediately after ADC calibration, ADC clock setting slow                */
/* (LL_ADC_CLOCK_ASYNC_DIV32). Use a higher delay if ratio                  */
/* (CPU clock / ADC clock) is above 32.  这里是div4                          */
#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 4)

void ADC_Configure(void);
void ADC_Activate(void);
void ADC_DeActivate(void);
void ConversionStartPoll_ADC_GrpRegular(void);
/* 读取VDDA值 */
uint16_t VDDA_Get(void);    

#endif
