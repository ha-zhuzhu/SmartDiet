#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_tim.h"
unsigned char uart1_getok;
_SaveData Save_Data;

/* 
 * 加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
 */
#if 1
#pragma import(__use_no_semihosting)

/* 标准库需要的支持函数 */
struct __FILE
{
    int handle;
};
FILE __stdout;

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

/* 重定义fputc函数  */
int fputc(int ch, FILE *f)
{
    while ((USART2->SR & 0X40) == 0)
        ; //循环发送,直到发送完毕
    USART2->DR = (u8)ch;
    return ch;
}
#endif

void usart1_send_byte(char data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART1, data);
}

void usart2_send_byte(char data)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART2, data);
}

void usart6_send_byte(char data)
{
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET)
        ;
    USART_SendData(USART6, data);
}

//注意,读取USARTx->SR能避免莫名其妙的错误
//这个缓冲它是循环写的，写到末尾又回到开头
char RxCounter, RxBuffer[200];   //接收缓冲,最大USART_REC_LEN个字节.
char RxCounter1, RxBuffer1[100]; //接收缓冲,最大USART_REC_LEN个字节.
u16 point2 = 0;
char RxBuffer2[200];
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0; //接收状态标记
extern u8 Timeout;

/**
 * 初始化 USART1
 * TX PA9 D8
 * RX PA10 D2
 * bound: 波特率
 */
void usart1_init(u32 bound)
{
    /* GPIO端口设置 */
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 使能USART1，GPIOA时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* 串口对应引脚复用映射 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1

    /* 端口配置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化PA9，PA10

    /* USART 初始化设置 */
    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART1, &USART_InitStructure);                                       //初始化串口

/* 中断配置 */
#if EN_USART1_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启相关中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;         //串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器、
#endif

    USART_Cmd(USART1, ENABLE); //使能串口
    USART_ClearFlag(USART1, USART_FLAG_TC);
}

/**
 * 初始化 USART2
 * TX PA2 D1
 * RX PA3 D0
 * bound: 波特率
 */
void usart2_init(u32 bound)
{
    /* GPIO端口设置 */
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 使能USART2，GPIOA时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* 串口对应引脚复用映射 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //GPIOA2复用为USART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //GPIOA3复用为USART2

    /* 端口配置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //初始化PA2，PA3

    /* USART 初始化设置 */
    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART2, &USART_InitStructure);                                       //初始化串口

/* 中断配置 */
#if EN_USART2_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启相关中断

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;         //串口2中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器、
#endif

    USART_Cmd(USART2, ENABLE); //使能串口
    USART_ClearFlag(USART2, USART_FLAG_TC);
}

/**
 * 初始化 USART6
 * TX PC6 J4_3
 * RX PC7 J4_2(D9)
 * bound: 波特率
 */
void usart6_init(u32 bound)
{
    /* GPIO端口设置 */
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 使能USART6，GPIOC时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    /* 串口对应引脚复用映射 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //GPIOC11复用为USART6
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //GPIOC12复用为USART6

    /* 端口配置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA2与GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);                 //初始化PC6，PC7

    /* USART 初始化设置 */
    USART_InitStructure.USART_BaudRate = bound;                                     //波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //收发模式
    USART_Init(USART6, &USART_InitStructure);                                       //初始化串口

/* 中断配置 */
#if EN_USART6_RX
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启相关中断

    //Usart6 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;         //串口6中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           //根据指定的参数初始化NVIC寄存器、
#endif

    USART_Cmd(USART6, ENABLE); //使能串口
    USART_ClearFlag(USART6, USART_FLAG_TC);
}

/* 串口1打印数据 */
void usart1_send_str(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART1->SR & 0X40) == 0)
            ; //等待发送完成
        USART1->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

/* 串口2打印数据 */
void usart2_send_str(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART2->SR & 0X40) == 0)
            ; //等待发送完成
        USART2->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

/* 串口6打印数据 */
void usart6_send_str(char *SendBuf)
{
    while (*SendBuf)
    {
        while ((USART6->SR & 0X40) == 0)
            ; //等待发送完成
        USART6->DR = (u8)*SendBuf;
        SendBuf++;
    }
}

/* 串口1中断服务程序 */
void USART1_IRQHandler(void)
{
    char Res;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断，可以扩展来控制
    {
        Res = USART_ReceiveData(USART1);                   //接收模块的数据;
        RxBuffer[RxCounter++] = USART_ReceiveData(USART1); //接收模块的数据
        if (RxCounter > 199)                               //长度自行设定
            RxCounter = 0;
    }
}

/* 串口2中断服务程序 */
void USART2_IRQHandler(void)
{
    u8 Res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        Res = USART_ReceiveData(USART2); //(USART1->DR);	//读取接收到的数据
    }
}

/* 串口6中断服务程序 */
void USART6_IRQHandler(void)
{
    char Res;
    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) //接收模块返回的数据
    {
        Res = USART_ReceiveData(USART6); //接收模块的数据;
    }
}
