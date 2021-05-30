#include "sys.h"

void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

  /* Wait till HSE is ready */
  while (LL_RCC_HSE_IsReady() != 1)
  {
  }
  /*
  LL_PWR_EnableBkUpAccess();
  LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
  LL_RCC_LSE_Enable();
  //这里出了问题 LSE不起振
   /* Wait till LSE is ready */
  /*
  while(LL_RCC_LSE_IsReady() != 1)
  {

  }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
  LL_RCC_EnableRTC();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_8, LL_RCC_PLL_DIV_3);
  LL_RCC_PLL_Enable();
  */

  LL_RCC_LSI_Enable();

  /* Wait till LSI is ready */
  while (LL_RCC_LSI_IsReady() != 1)
  {
  }

  /* 配置 RTC 时钟，因为 LCD 和它共用 */
  LL_PWR_EnableBkUpAccess(); //PWR_CR->DBP=1
  /*
     * 访问 RTC 寄存器的先决条件：
     * RCC_APB1ENR->PWREN=1 在 HAL_Init()->HAL_MspInit() 中
     * PWR_CR->DBP=1 即上一条
     */
  LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
  LL_RCC_EnableRTC();

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_8, LL_RCC_PLL_DIV_3);
  LL_RCC_PLL_Enable();

  /* Wait till PLL is ready */
  while (LL_RCC_PLL_IsReady() != 1)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  /* Wait till System clock is ready */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }
  LL_SetSystemCoreClock(32000000);

  /* Update the time base */
  if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);
}

static void SystemPower_Config(void)
{
  LL_PWR_EnableUltraLowPower();

  LL_PWR_EnableFastWakeUp();

  LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_MSI);

  // 把除了 HX711_EN WAKEUP 还有BC28串口 等GPIO口设为模拟输入
}