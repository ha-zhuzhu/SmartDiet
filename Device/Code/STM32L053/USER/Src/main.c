/* CyberBalance - 可以联网的电子秤 */
#include "main.h"
#include "sys.h"
#include "usart.h"
#include "bc28.h"
#include "lcd.h"
#include "hx711.h"
#include "btn.h"
#include "adc.h"

extern uint8_t RxCounter;
extern _HX711_DATA HX711_Data;
uint8_t BC28_netstatus = 0;
uint16_t vdda;
#if defined(DEBUG_MODE)
char strbuff[40];
#endif

int main()
{
    // HAL_Init 中调用了 HAL_MspInit 打开了 Power Control clock
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
    LCD_Init();
    LCD_GLASS_Clear();
    /* HX711 */
    HX711_Init();
    /* BTN -- Wakeup */
    BTN_Init();
    /* BC28
     * PB10   ------> LPUART1_TX
     * PB11   ------> LPUART1_RX 
     * PB2    ------> BC28_RST*/
    LPUART1_Init();
    BC28_netstatus = BC28_Init();
    if (BC28_netstatus)
    {
        BC28_CreateInstance();
    }
    BC28_EnablePSM(); //BC28配置PSM参数
    ADC_Configure();  //ADC配置
    HX711_Tare();     //去皮复位
    while (1)
    {
        /* HX711 -- 读取重量 */
        HX711_Get_Weight(HX711_RefreshTimes);
#if defined(DEBUG_MODE)
        sprintf(strbuff, "%10.3f\r\n", HX711_Data.PreWeight);
        USART1_SendStr(strbuff);
        sprintf(strbuff, "%10.1f\r\n", HX711_Data.PreWeight);
        USART1_SendStr(strbuff);
        sprintf(strbuff, "%d\r\n", HX711_Data.PreValue);
        USART1_SendStr(strbuff);
#endif
        LCD_GLASS_Heartbeat(HX711_Data.PreWeight); //LCD显示

        /* BC28 -- 有可用重量数据，且未上传过，则上传 */
        if (HX711_Data.stableFlag && BC28_netstatus && !HX711_Data.sentFlag)
        {
            HX711_Data.sentFlag = 1;
            LCD_GLASS_BlinkConfig(); //LCD开启闪烁模式
            LCD_GLASS_Heartbeat(HX711_Data.PreWeight);
            //+0.5以四舍五入，保证上传的与LCD显示的一致
            BC28_NotifyResource((uint16_t)(HX711_Data.PreWeight * 10 + 0.5), ResTyp_Weight);
            LCD_GLASS_BlinkDeConfig(); //LCD开启闪烁模式
        }

        /* 静置休眠 */
        if (HX711_Data.idleFlag)
        {
            vdda = VDDA_Get();
            BC28_NotifyResource(vdda, ResTyp_VDDA); //获取VDDA电压并上传
            HX711_Data.idleFlag = 0;
            STOPMode_Enter();   //进入STOP Mode
            STOPMode_Recover(); //唤醒后系统恢复
#if defined(DEBUG_MODE)
            USART1_SendStr("wakeup!\r\n");
#endif
            vdda = VDDA_Get();
            BC28_NotifyResource(vdda, ResTyp_VDDA); //获取VDDA电压并上传
        }
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

    /* 调整 GPIO 状态 */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIOA 
       //PA5 -- HX711_SCK 拉高断电
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
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
    /* PA5 -- HX711_SCK 拉高断电
     * 这样反而给HX711喂了电压 让它电源引脚接近3V Ao3415完全没用了
     * 当然它也确实没有工作 传感器AV没有电压 系统总电流还是42u左右
     * 之前也是42u 但是HX711电源引脚总有0.1V 而且AV也在跳……*/
    /*
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
    */

    /* GPIOB 
       PB10 -- LPUART1_TX 不能悬空 上拉？
       PB11 -- LPUART1_RX 浮空输入
       PB2 -- BC28_RST 不变，保持拉低
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL ^ LL_GPIO_PIN_10 ^ LL_GPIO_PIN_11 ^ LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* PB10 -- LPUART1_TX 不能悬空 上拉？ */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    //GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;       //82u
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO; //79u
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* PB11 -- LPUART1_RX 浮空输入 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT; //82u
    //GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;   //100u
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

/* 进入STOP Mode */
void STOPMode_Enter(void)
{
    BC28_Sleep();
    LPUART1_DeInit(); //如果不deinit，唤醒后LPUART1的接收中断有问题
#if defined(DEBUG_MODE)
    USART1_DeInit();
#endif
    /* 关闭 LCD */
    LCD_Deinit();
    SystemPower_Config();
    /* PWR_CR_PDDS 置 0 */
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP);
    /* SLEEPDEEP 置 1 */
    LL_LPM_EnableDeepSleep();
    __wfi();
}

/* 唤醒后系统恢复 */
void STOPMode_Recover(void)
{
    // HAL_Init中调用了HAL_MspInit打开了 Power Control clock
    HAL_Init();
    SystemClock_Config();
#if defined(DEBUG_MODE)
    USART1_Init();
    USART1_SendStr("usart1\r\n");
#endif
    /* LCD */
    LCD_Init();
    LCD_GLASS_Clear();
    HX711_Init();
    BTN_Init();
    LPUART1_Init();
    HX711_Tare();
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