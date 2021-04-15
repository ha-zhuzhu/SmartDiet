#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define EN_USART1_RX 1
#define EN_USART2_RX 1
#define EN_USART6_RX 1

#define Weight_Buffer_Length 20

void usart1_init(u32 bound);
void usart2_init(u32 bound);
void usart6_init(u32 bound);
void usart1_send_byte(char data);
void usart2_send_byte(char data);
void usart6_send_byte(char data);
void usart1_send_str(char*SendBuf);
void usart2_send_str(char*SendBuf);
void usart6_send_str(char*SendBuf);

typedef struct SaveData
{
	char Weight_Buffer[Weight_Buffer_Length];
	u8 counter;
	char isGetData;		//是否获取到重量数据
	char isParseData;	//是否解析完成
	char isUsefull;		//信息是否有效
} _SaveData;

extern char RxBuffer2[200];
extern _SaveData Save_Data;
#endif
