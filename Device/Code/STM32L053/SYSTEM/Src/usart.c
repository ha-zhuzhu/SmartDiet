#include "usart.h"

char LPUART1_RX_BUF[LPUART_REC_LEN];
char LPUART1_TX_BUF[80];
uint8_t RxCounter;

//#define USR_USE_HAL
#ifdef USR_USE_HAL
void Uart1_SendStr(char *SendBuf) //串口1发送字符串
{
    while (*SendBuf)
    {
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) != 1)
            ;                                                    //等待发送完成
        HAL_UART_Transmit(&huart1, (uint8_t *)SendBuf, 1, 1000); //发送数据
        SendBuf++;
    }
}
#else
void USART1_SendStr(char *SendBuf) //串口1发送字符串
{
    while (*SendBuf)
    {
        while (LL_USART_IsActiveFlag_TC(USART1) != 1)
            ;                                                                //等待发送完成
        LL_USART_TransmitData8(USART1, (uint8_t)(*SendBuf & (uint8_t)0xff)); //发送数据
        SendBuf++;
    }
}
#endif

void LPUART1_SendStr(char *SendBuf) //低功耗串口1发送字符串
{
    while (*SendBuf)
    {
        while (LL_LPUART_IsActiveFlag_TC(LPUART1) != 1)
            ;                                                                //等待发送完成
        LL_LPUART_TransmitData8(LPUART1, (uint8_t)(*SendBuf & (uint8_t)0xff)); //发送数据
        SendBuf++;
    }
}

void LPUART1_Init(void)
{
  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  /**LPUART1 GPIO Configuration
  PB10   ------> LPUART1_TX
  PB11   ------> LPUART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* LPUART1 interrupt Init */
  NVIC_SetPriority(RNG_LPUART1_IRQn, 0);
  NVIC_EnableIRQ(RNG_LPUART1_IRQn);

  LPUART_InitStruct.BaudRate = 9600;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);

  //LL_LPUART_EnableIT_RXNE(LPUART1);
  LL_LPUART_Enable(LPUART1);
  LL_LPUART_EnableIT_RXNE(LPUART1); //打开接受空中断
}


/**
 * USART1 GPIO Configuration
 * PB6   ------> USART1_TX
 * PB7   ------> USART1_RX
 */
void USART1_Init(void)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);

    USART_InitStruct.BaudRate = 9600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);
}


void LPUART1_IRQHandler(void)
{
    char tmp;
	if(LL_LPUART_IsActiveFlag_RXNE(LPUART1)) //检测是否接收中断
	{
		tmp=LL_LPUART_ReceiveData8(LPUART1);   //读取出来接收到的数据 
        LPUART1_RX_BUF[RxCounter++]=tmp;
        if (RxCounter>199)  RxCounter=0;
	}
}
