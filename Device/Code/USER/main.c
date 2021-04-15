/*
  BC28_ONENET_LWM2M+GPS 测试 demo
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

extern char RxBuffer[100], RxCounter;
u8 timeout;

int main(void)
{
	delay_init();		  //延时函数初始化
	NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	usart1_init(9600);	  //串口1--BC28
	usart2_init(9600);	  //串口2--PC
	//usart6_init(9600);//可连接PC进行打印模块返回数据
	usart2_send_str("串口初始化成功\r\n");
	btn1_exti_init();
	BC28_Init();		   //对设备初始化
	BC28_CreateInstance(); //创建连接对象到ONENET
	
	Save_Data.counter=0;
	usart2_send_str("BC28初始化成功\r\n");
	while (1)
	{
		//ONENET_Readdata();//接收ONENET的数据下发指令请求
		/* 有可用坐标数据则上传 */
		if (Save_Data.isUsefull)
		{
			usart2_send_str("检测到按键\r\n");
			Save_Data.isUsefull = 0;
			BC28_NotifyResource();
			timeout++;
			delay_ms(1000);
		}
		if (timeout >= 120) //2分钟刷新一次连接防掉线
		{
			printf("AT+MIPLUPDATE=0,600,0\r\n"); //10分钟在线时间
			RxCounter = 0;
			timeout = 0;
		}
		//printGpsBuffer();
	}
}
