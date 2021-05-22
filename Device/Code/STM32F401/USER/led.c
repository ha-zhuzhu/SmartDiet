#include "led.h"

/* 
 * LED 初始化
 * BLue PD2
 * Red PC10
 * Green PA5(D13)
 * 设备启动后红色，初始化成功后绿色，有信息需要上传黄色，成功后恢复绿色
 * 要是能初始化/上传时是呼吸灯就更好看了
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能PD,PC,PA端口时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);

    /* BLue PD2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);            //根据设定参数初始化GPIOD2

    /* Red PC10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //根据设定参数初始化GPIOC10

    /* Green PA5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);            //根据设定参数初始化GPIOA5

    LED_RED = 0;
    LED_BLU = 1;
    LED_GRN = 1;
}