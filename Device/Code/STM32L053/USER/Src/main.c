#include "main.h"
#include "sysclock.h"
#include "usart.h"
#include "led.h"

int main()
{
    HAL_Init();
    SystemClock_Config();
    LED_Init();
    /* PB6   ------> USART1_TX
     * PB7   ------> USART1_RX */
    USART1_Init();
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
    while (1)
    {
        LL_mDelay(500);
        printf("blink/r/n");
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
        LL_mDelay(500);
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
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