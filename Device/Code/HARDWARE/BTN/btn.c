#include "btn.h"
#include "usart.h"
/*
 * 初始化 BTN_1 中断
 * PC13
 * 中间的按钮
 */
void btn1_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 配置PC13为输入 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能PC端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;             //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;         //输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    //上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//工作频率
    GPIO_Init(GPIOC, &GPIO_InitStructure);                //根据设定参数初始化GPIOC

    /* 配置EXTI13 */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT, ENABLE);  //使能EXTIT时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
    /* 如果配置的针脚是0号，那么参数必须是GPIO_PinSource0 如果配置的针脚是3号，那么参数必须是GPIO_PinSource3 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource13);    
    EXTI_InitStructure.EXTI_Line = EXTI_Line13; //常用的就是EXTI_Line0-EXTI_Line015负责gpio管脚的那几个
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //初始化中断

    /* 配置NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;          //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级2  因为为分组为2 这里可以设置为0-3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //响应优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                           //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/* 中断服务函数 */
void EXTI15_10_IRQHandler(void)
{
    //判断外部中断13是否发生
    if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
        usart2_send_str("检测到按键\r\n");
        Save_Data.isUsefull = 1;
        Save_Data.counter=(Save_Data.counter+1)%200;
        //清除中断标志位
		EXTI_ClearITPendingBit(EXTI_Line13);
	 }
}