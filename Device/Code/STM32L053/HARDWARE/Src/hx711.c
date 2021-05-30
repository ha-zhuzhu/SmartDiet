#include "hx711.h"

_HX711_DATA HX711_Data={0,0,0,HX711_defaultRatio,0,0,0};

void HX711_Init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LCD_Init() 也会对 GPIOA 时钟初始化
    if (LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOA) == 0)
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);

    /* HX711_SCK - GPIOA5 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* HX711_DOUT - GPIOA6 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 重启 HX711 */
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
    LL_mDelay(50);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);

    /* 通过一次读数设定增益 */
    HX711_Value();
    /* 初始化去皮 */
    HX711_Tare(10);
}

/* 限幅中位均值滤波 */
int HX711_Average_Value(uint8_t times)
{
    int sum, buf, minData, maxData;
    sum = buf = maxData = 0;
    minData = 0x800000;
    for (uint8_t i = 0; i < times; i++)
    {
        buf = HX711_Value();
        maxData = buf > maxData ? buf : maxData;
        minData = buf < minData ? buf : minData;
        sum += buf;
    }
    return (sum - minData - maxData) / (times - 2);
}

int HX711_Value()
{
    int buffer;
    buffer = 0;

    while (LL_GPIO_IsInputPinSet(DOUT_GPIO, DOUT_PIN) == 1)
        ;

    for (uint8_t i = 0; i < 24; i++)
    {
        LL_GPIO_SetOutputPin(SCK_GPIO, SCK_PIN);

        buffer = buffer << 1;

        if (LL_GPIO_IsInputPinSet(DOUT_GPIO, DOUT_PIN))
        {
            buffer++;
        }

        LL_GPIO_ResetOutputPin(SCK_GPIO, SCK_PIN);
    }

    for (int i = 0; i < HX711_gain; i++)
    {
        LL_GPIO_SetOutputPin(SCK_GPIO, SCK_PIN);
        LL_GPIO_ResetOutputPin(SCK_GPIO, SCK_PIN);
    }

    buffer = buffer ^ 0x800000;

    return buffer;
}

/* 去皮 */
void HX711_Tare(uint8_t times)
{
    int sum = HX711_Average_Value(times);
    HX711_Data.offset = sum;
}

void HX711_Get_Weight(uint8_t times)
{
    int value=0;
    float weight=0;
    value=HX711_Average_Value(times);
    HX711_Data.PreWeight=weight=(float)(value-HX711_Data.offset)/HX711_Data.ratio;
    /* 稳定 */
    if (fastAbs(value-HX711_Data.PreValue)<HX711_stableThr)
    {
        if (weight>5)
        {
            if (HX711_Data.stableCounter==HX711_stableTime-1 && weight>5)
            {
                HX711_Data.stableCounter=0;
                HX711_Data.stableFlag=1;
            }
            else
                HX711_Data.stableCounter++;
        }
        else
            HX711_Data.idleCounter++;
    }
    else
    {
        HX711_Data.stableCounter=0;
        HX711_Data.idleCounter=0;
    }
    HX711_Data.PreValue=value;
}