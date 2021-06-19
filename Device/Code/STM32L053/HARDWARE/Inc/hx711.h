#ifndef HX711_H_
#define HX711_H_

#include "main.h"

/* 
 * 配置通道与增益系数
 * 1: channel A, gain factor 128
 * 2: channel B, gain factor 32
 * 3: channel A, gain factor 64
 */
#define HX711_gain 1
#define HX711_stableTime 4          // 稳定多少次可以上传数据 4=1s 2=0.5s
#define HX711_stableThr 38          //连续两次ADC value在此阈值内算稳定 0.05g
#define HX711_defaultRatio 774.270219 // weight=value-offset/ratio
#define HX711_idleTime 20           // 多少次读数idle则睡眠
#define HX711_BUF_SIZE 80           // 动态窗大小
#define HX711_RefreshTimes 20       //收集多少个采样点刷新一次
#define HX711_BUF_SegNum HX711_BUF_SIZE / HX711_RefreshTimes

#define SCK_GPIO GPIOA
#define SCK_PIN LL_GPIO_PIN_5
#define DOUT_GPIO GPIOA
#define DOUT_PIN LL_GPIO_PIN_6

typedef struct _hx711_data
{
    int PreValue;
    float PreWeight;
    int offset;
    float ratio;
    uint8_t stableCounter;
    uint8_t idleCounter;
    uint8_t stableFlag;
    uint8_t idleFlag;
    uint8_t sentFlag;
} _HX711_DATA;

extern _HX711_DATA HX711_Data;

void HX711_Init();
void HX711_Tare();
int HX711_Value();
int HX711_AverageValue(uint16_t times);
void HX711_Get_Weight(uint16_t times);

#endif /* HX711_H_ */
