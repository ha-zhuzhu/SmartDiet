#include "btn.h"

/* PA2  */
/*
void BTN_Init(void)
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};


    if (LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOA) == 0)
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE2);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);

    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_2;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    NVIC_SetPriority(EXTI2_3_IRQn , 0);
    NVIC_EnableIRQ(EXTI2_3_IRQn );
}

void EXTI2_3_IRQHandler(void)
{
    LL_mDelay(10);
    if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_2) == RESET) //按键消抖
    {
        LCD_GLASS_Clear();
        HX711_Tare(10);
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2) != RESET) //检验EXIT0的标志位
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2); //清除EXIT0的标志位
    }
}
*/

/* 用于开发板 PC13 需要内部拉高*/
void BTN_Init(void)
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

    if (LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOC) == 0)
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_13, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_13, LL_GPIO_MODE_INPUT);

    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    NVIC_SetPriority(EXTI4_15_IRQn, 0);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void)
{
    LL_mDelay(10);
    if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13) == RESET) //按键消抖
    {
        LCD_GLASS_Clear();
        HX711_Tare(10);
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) != RESET) //检验EXIT0的标志位
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13); //清除EXIT0的标志位
    }
}