#ifndef __BC28_H
#define __BC28_H	
#include "usart.h"
#include <stm32f4xx.h>
#include "delay.h"
void Clear_Buffer(void);//清空缓存	
void BC28_Init(void);
void BC28_CreateUDPSokcet(void);
void BC28_UDPSend(uint8_t *len,uint8_t *data);
void BC28_CreateSokcet(void);
void Clear_Buffer(void);
void BC28_ChecekConStatus(void);
void BC28_RECData(void);
void BC28_CreateInstance(void);
void ONENET_Readdata(void);
void BC28_NotifyResource(void);

typedef struct
{
    uint8_t CSQ;    
    uint8_t Socketnum;   //编号
    uint8_t reclen;   //获取到数据的长度
    uint8_t res;      
    uint8_t recdatalen[10];
    uint8_t recdata[100];
    uint8_t netstatus;//网络指示灯
		uint8_t Observe_ID[10];//observe ID
    uint8_t Resource_ID[10];//获取的资源ID号
    uint8_t ReadSource[50];//读取状态值
    uint8_t ReadSourceHD[50];//读取状态值 
    uint8_t ReadSourceED[50];//读取状态值  
} BC28;

#endif





