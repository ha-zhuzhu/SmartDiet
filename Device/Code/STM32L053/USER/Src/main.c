#include "main.h"
#include "sys.h"
#include "usart.h"
#include "led.h"
#include "bc28.h"
#include "lcd.h"
#include "hx711.h"
#include "btn.h"
#include "adc.h"

uint8_t timeout;
extern uint8_t RxCounter;
extern _HX711_DATA HX711_Data;
uint8_t BC28_netstatus = 0;

int main()
{
    uint16_t vdda;
    char buf[30];
    // HAL_Init中调用了HAL_MspInit打开了 Power Control clock
    HAL_Init();
    SystemClock_Config();

    /* 
     * USART1-PC
     * PB6   ------> USART1_TX
     * PB7   ------> USART1_RX 
     */
#if defined(DEBUG_MODE)
    USART1_Init();
    USART1_SendStr("usart1\r\n");
#endif

    /* LCD */
    /*
    LCD_Init();
    LCD_GLASS_Clear();
    */
    /* HX711 */
    /*
    HX711_Init();
    int buff;
    char strbuff[40];
    */
    BTN_Init();

    /* BC28 */
    /* LPUART1-BC28
     * PB10   ------> LPUART1_TX
     * PB11   ------> LPUART1_RX */

    LPUART1_Init();
    BC28_netstatus = BC28_Init();
    if (BC28_netstatus)
    {
        BC28_CreateInstance();
    }
    BC28_EnablePSM();

    //HX711_Tare(40);
    ADC_Configure();

    while (1)
    {
        vdda = VDDA_Get();
        sprintf(buf, "vdda:%d\r\n", vdda);
        USART1_SendStr(buf);
        BC28_NotifyResource(vdda, ResTyp_VDDA);
        LL_mDelay(2000);
        /*
        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2))
            USART1_SendStr("GPIOA2 HIGH\r\n");
        else
            USART1_SendStr("GPIOA2 OFF\r\n");
            */
        /* HX711 */
        /*
        HX711_Get_Weight(40);
#if defined(DEBUG_MODE)
        sprintf(strbuff, "%10.3f", HX711_Data.PreWeight);
        USART1_SendStr(strbuff);
#endif
*/
        // LCD_GLASS_Heartbeat(HX711_Data.PreWeight);

        /* BC28 */
        /* 有可用重量数据则上传 */
        /*
        if (HX711_Data.stableFlag && BC28_netstatus)
        //if (1)
        {
            HX711_Data.stableFlag = 0;
            LCD_GLASS_BlinkConfig();
            LCD_GLASS_Heartbeat(HX711_Data.PreWeight);
            //BC28_NotifyResource((uint16_t)1314.520);
            BC28_NotifyResource((uint16_t)HX711_Data.PreWeight);
            LCD_GLASS_BlinkDeConfig();
        }
        */
        /* 静置休眠 */
        /*
        if(HX711_Data.idleFlag)
        
            STOPMode_Enter();
            STOPMode_Recover();
            USART1_SendStr("wakeup!\r\n");
        }
        */
        /*
        if (timeout >= 120) //2分钟刷新一次连接防掉线
        {
            LPUART1_SendStr("AT+MIPLUPDATE=0,600,0\r\n"); //10分钟在线时间
            RxCounter = 0;
            timeout = 0;
        }
        */
    }
}

void SystemPower_Config(void)
{
    LL_PWR_EnableUltraLowPower();

    LL_PWR_EnableFastWakeUp();

    LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_MSI);

    /* Enable GPIOs clock */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);

    /* 关闭 LCD */
    LCD_Deinit();

    /* 调整 GPIO 状态 */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIOA 
       PA7 -- HX_EN 启用为拉高输出
     */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL ^ LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* PA7 -- HX_EN 拉高输出 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* GPIOB
       PB10 -- LPUART1_TX 不能悬空 上拉？
       PB11 -- LPUART1_RX 浮空输入
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL ^ LL_GPIO_PIN_10 ^ LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* PB10 -- LPUART1_TX 不能悬空 上拉？ */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* PB11 -- LPUART1_RX 浮空输入 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* GPIO D H */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    LL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* GPIOC */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL ^ LL_GPIO_PIN_13;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* WAKEUP 对应引脚的时钟不能关 */
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOD);
    LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOH);
}

void STOPMode_Enter(void)
{
    //BC28_Sleep();
    SystemPower_Config();
    /* PWR_CR_PDDS 置 0 */
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
    /* SLEEPDEEP 置 1 */
    LL_LPM_EnableDeepSleep();
    __wfi();
}

void STOPMode_Recover(void)
{
    SystemClock_Config();
#if defined(DEBUG_MODE)
    USART1_Init();
    USART1_SendStr("usart1\r\n");
#endif
    /* LCD */
    /*
    LCD_Init();
    LCD_GLASS_Clear();

    HX711_Init();
    */
    BTN_Init();
    /*
    LPUART1_Init();
    HX711_Tare(10);*/
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */