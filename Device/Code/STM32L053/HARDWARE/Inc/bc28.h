#ifndef __BC28_H
#define __BC28_H
#include "main.h"

#define BC28_RST_GPIO GPIOB
#define BC28_RST_PIN LL_GPIO_PIN_2

#if defined(DEBUG_MODE)
#define Clear_Buffer() Clear_LPUART1_Buffer_2_USART1()
#else
#define Clear_Buffer()
#endif

typedef enum
{
    ResTyp_Weight = 0,
    ResTyp_VDDA = 1
} Resource_Typedef;

void Clear_LPUART1_Buffer_2_USART1(void); //清空缓存
uint8_t BC28_Init(void);
void BC28_CreateUDPSokcet(void);
void BC28_UDPSend(uint8_t *len, uint8_t *data);
void BC28_CreateSokcet(void);
void BC28_ChecekConStatus(void);
void BC28_RECData(void);
void BC28_CreateInstance(void);
void ONENET_Readdata(void);
void BC28_NotifyResource(uint16_t ResourceValue, Resource_Typedef ResTyp);
void BC28_EnablePSM(void);
void BC28_DisablePSM(void);
void BC28_Sleep(void);

typedef struct
{
    uint8_t CSQ;
    uint8_t Socketnum; //编号
    uint8_t reclen;    //获取到数据的长度
    uint8_t res;
    uint8_t recdatalen[10];
    uint8_t recdata[100];
    uint8_t netstatus;        //网络指示灯
    uint8_t Observe_ID[10];   //observe ID
    uint8_t Resource_ID[10];  //获取的资源ID号
    uint8_t ReadSource[50];   //读取状态值
    uint8_t ReadSourceHD[50]; //读取状态值
    uint8_t ReadSourceED[50]; //读取状态值
} BC28;

#endif
