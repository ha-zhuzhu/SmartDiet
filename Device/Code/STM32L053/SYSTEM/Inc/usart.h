#ifndef __USART_H__
#define __USART_H__

#include "main.h"

#define LPUART_REC_LEN 200
extern char LPUART1_RX_BUF[LPUART_REC_LEN];
extern char LPUART1_TX_BUF[80];
extern uint8_t RxCounter;

void USART1_Init(void);
void USART1_DeInit(void);
void USART1_SendStr(char *SendBuf);
void LPUART1_Init(void);
void LPUART1_DeInit(void);
void LPUART1_SendStr(char *SendBuf);

#endif /* __USART_H__ */
