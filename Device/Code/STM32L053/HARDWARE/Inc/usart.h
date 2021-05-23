#ifndef __USART_H__
#define __USART_H__

#include "main.h"
#include "stdio.h"

#define USART_REC_LEN 200
extern uint8_t  USART1_RX_BUF[USART_REC_LEN];

void USART1_Init(void);
void USART1_SendStr(char*SendBuf);


#endif /* __USART_H__ */