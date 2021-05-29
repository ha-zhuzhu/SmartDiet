#include "main.h"
#include "sysclock.h"
#include "usart.h"
#include "led.h"
#include "bc28.h"
#include "lcd.h"

uint8_t timeout;
extern uint8_t RxCounter;

int main()
{
    HAL_Init();
    SystemClock_Config();

    /* LED test
    LED_Init();
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
    */

    /* USART1-PC
     * PB6   ------> USART1_TX
     * PB7   ------> USART1_RX */
    //USART1_Init();

    /* BC28 */
    /* LPUART1-BC28
     * PB10   ------> LPUART1_TX
     * PB11   ------> LPUART1_RX */
    //LPUART1_Init();
    //BC28_Init();
    //BC28_CreateInstance();

    /* LCD */
    LCD_Init();
    LCD_GLASS_Clear();

    while (1)
    {
        /* LED
        LL_mDelay(500);
        //LPUART1_SendStr("blink/r/n");
        LPUART1_SendStr(LPUART1_RX_BUF);
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
        LL_mDelay(500);
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
        */

        //BC28
        //ONENET_Readdata();//接收ONENET的数据下发指令请求
        /* 有可用重量数据则上传 */
        /*
        if (Save_Data.isUsefull)
        {
            Save_Data.isUsefull = 0;
            BC28_NotifyResource();
            timeout++;
            delay_ms(1000);
        }
        */

        /*
        BC28_NotifyResource();
        timeout++;
        LL_mDelay(2000);
        if (timeout >= 120) //2分钟刷新一次连接防掉线
        {
            LPUART1_SendStr("AT+MIPLUPDATE=0,600,0\r\n"); //10分钟在线时间
            RxCounter = 0;
            timeout = 0;
        }
        */

        /* LCD */
        LCD_GLASS_Heartbeat(233.3);
    }
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