#ifndef HX711_H_
#define HX711_H_

#include "main.h"

// 1: channel A, gain factor 128
// 2: channel B, gain factor 32
// 3: channel A, gain factor 64
#define HX711_gain 2
#define HX711_stableTime 3  // 稳定多少次可以上传数据
#define HX711_stableThr 30000   //连续两次ADC value在此阈值内算稳定
#define HX711_defaultRatio 100000   // weight=value-offset/ratio
#define HX711_idleTIme 30   //30次读数idle则睡眠

#define SCK_GPIO GPIOA
#define SCK_PIN LL_GPIO_PIN_5
#define DOUT_GPIO GPIOA
#define DOUT_PIN LL_GPIO_PIN_6

typedef struct _hx711_data
{
    int PreValue;
    float PreWeight;
    int offset;
    int ratio;
    uint8_t stableCounter;
    uint8_t idleCounter;
    uint8_t stableFlag;
} _HX711_DATA;

extern _HX711_DATA HX711_Data;

void HX711_Init();
void HX711_Tare(uint8_t times);
int HX711_Value();
int HX711_AverageValue(uint8_t times);
void HX711_Get_Weight(uint8_t times);

#endif /* HX711_H_ */
