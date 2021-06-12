#include "btn.h"
#include "usart.h"
#include "hx711.h"
#include "lcd.h"

/* PC13 */
void BTN_Init(void)
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

    if (LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOC) == 0)
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);
    //LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_13, LL_GPIO_PULL_UP);//79.3
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_13, LL_GPIO_PULL_NO);//79.4
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_13, LL_GPIO_MODE_INPUT);

    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    NVIC_SetPriority(EXTI4_15_IRQn, 1);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void)
{
    LL_mDelay(10);
    if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13) == RESET) //按键消抖
    {
        if (LL_PWR_IsActiveFlag_WU()) //刚被唤醒
            LL_PWR_ClearFlag_WU();
        else
        {
            USART1_SendStr("btn\r\n");
            // 去皮
            LCD_GLASS_Clear();
            HX711_Tare();
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) != RESET) //检验EXIT13的标志位
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13); //清除EXIT13的标志位
    }
}
