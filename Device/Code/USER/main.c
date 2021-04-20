/*
  BC28_ONENET_LWM2M Read & Respond Mode
  自动注册 Object 和 resource
  监听 ONENET 的读 resource 请求并响应
*/
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "stdio.h"
#include "stm32f4xx_flash.h"
#include "stdlib.h"
#include "string.h"
#include "wdg.h"
#include "timer.h"
#include "stm32f4xx_tim.h"
#include "bc28.h"
#include "math.h"
#include "btn.h"
#include "led.h"

extern char RxBuffer[200], RxCounter;
u8 timeout;

int main(void)
{
    delay_init();           //延时函数初始化
    NVIC_Configuration();   //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    LED_Init();             //LED初始化
    usart1_init(9600);      //串口1--BC28
    usart2_init(9600);      //串口2--PC
    //usart6_init(9600);    //串口3
    btn1_exti_init();       //按钮中断初始化
    BC28_Init();            //对BC28设备初始化
    BC28_CreateInstance();  //创建连接对象到ONENET
    Save_Data.counter = 0;
    while (1)
    {
        LED_RED=1; LED_BLU=1; LED_GRN=0;
        ONENET_Readdata();//接收ONENET的数据下发指令请求
        /* 有可用重量数据则上传 */
        // if (Save_Data.isUsefull)
        // {
        //     LED_RED=0; LED_BLU=1; LED_GRN=0;
        //     Save_Data.isUsefull = 0;
        //     BC28_NotifyResource();
        //     timeout++;
        //     delay_ms(1000);
        // }
        if (timeout >= 120) //2分钟刷新一次连接防掉线
        {
            printf("AT+MIPLUPDATE=0,600,0\r\n"); //10分钟在线时间
            RxCounter = 0;
            timeout = 0;
        }
        //delay_ms(500);
    }
    
}
