#include "hx711.h"

_HX711_DATA HX711_Data = {0, 0, 0, HX711_defaultRatio, 0, 0, 0, 0, 0};
int HX711_BUF[HX711_BUF_SIZE] = {0};
uint16_t HX711_BUF_Counter = 0;
/* 存放每一段即refreshtimes个样本的总和，最后存放size个样本的总和 */
int64_t HX711_BUF_SegSum[HX711_BUF_SegNum + 1] = {0};

void HX711_Init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LCD_Init() 也会对 GPIOA 时钟初始化
    if (LL_IOP_GRP1_IsEnabledClock(LL_IOP_GRP1_PERIPH_GPIOA) == 0)
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    /* HX_EN - GPIOA7 拉低控制 PMOS 导通 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);

    /* HX711_SCK - GPIOA5 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5); //先拉低使HX711不工作

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
    //HX711_Tare();
}

/* buf24位，如果sum也是int，则只能平均2^8个样点
 */
int HX711_Average_Value(uint16_t times)
{
    int64_t sum = 0;
    int8_t buf_segptr = HX711_BUF_Counter / HX711_RefreshTimes;
    for (uint16_t i = 0; i < times; ++i)
    {
        HX711_BUF[HX711_BUF_Counter] = HX711_Value();
        sum += HX711_BUF[HX711_BUF_Counter++];
        if (HX711_BUF_Counter >= HX711_BUF_SIZE)
            HX711_BUF_Counter = 0;
    }
    HX711_BUF_SegSum[HX711_BUF_SegNum] += sum - HX711_BUF_SegSum[buf_segptr];
    HX711_BUF_SegSum[buf_segptr] = sum;
    return HX711_BUF_SegSum[HX711_BUF_SegNum] / HX711_BUF_SIZE;
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

    for (uint8_t i = 0; i < HX711_gain; i++)
    {
        LL_GPIO_SetOutputPin(SCK_GPIO, SCK_PIN);
        LL_GPIO_ResetOutputPin(SCK_GPIO, SCK_PIN);
    }

    buffer = buffer ^ 0x800000;

    return buffer;
}

/* 去皮 */
void HX711_Tare()
{
    int64_t sum_seg, sum;
    sum_seg = sum = 0;
    for (uint16_t i = 0; i < HX711_BUF_SIZE; ++i)
    {
        HX711_BUF[i] = HX711_Value();
        sum_seg += HX711_BUF[i];
        sum += HX711_BUF[i];
        if ((i + 1) % HX711_RefreshTimes == 0)
        {
            HX711_BUF_SegSum[i / HX711_RefreshTimes] = sum_seg;
            sum_seg = 0;
        }
    }
    HX711_BUF_SegSum[HX711_BUF_SegNum] = sum;
    HX711_Data.offset = sum / HX711_BUF_SIZE;
}

void HX711_Get_Weight(uint16_t times)
{
    int value = 0;
    float weight = 0;
    value = HX711_Average_Value(times);
    weight = (float)(value - HX711_Data.offset) / HX711_Data.ratio;
    /* 稳定 prevalue preweight不更新 */
    // fastAbs 外要套括号先计算出结果，否则永远为真
    if ((fastAbs((value - HX711_Data.PreValue))) < HX711_stableThr)
    {
        if (weight > 1) //有效读数 1g
        {
            if (HX711_Data.stableCounter >= HX711_stableTime - 1) //可上传
            {
                HX711_Data.stableCounter = 0;
                HX711_Data.stableFlag = 1;
            }
            else
                HX711_Data.stableCounter++;
        }
        else //空盘状态
        {
            if (HX711_Data.idleCounter >= HX711_idleTime - 1) //可休眠
            {
                HX711_Data.idleCounter = 0;
                HX711_Data.idleFlag = 1;
            }
            else
                HX711_Data.idleCounter++;
        }
    }
    else //不稳定,prevalue preweight更新
    {
        HX711_Data.PreValue = value;
        HX711_Data.PreWeight = weight;
        HX711_Data.stableFlag = 0;
        HX711_Data.idleFlag = 0;
        HX711_Data.stableCounter = 0;
        HX711_Data.idleCounter = 0;
        HX711_Data.sentFlag = 0;
    }
}
