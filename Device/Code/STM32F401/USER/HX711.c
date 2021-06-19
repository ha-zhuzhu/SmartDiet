/************************************************************************************
						
*************************************************************************************/
#include "HX711.h"
#include "delay.h"
#include "usart.h"
u32 HX711_Buffer;
u32 Weight_Maopi;
s32 Weight_Shiwu;
u8 Flag_Error = 0;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 716//715827.8827//430//106.5


// OUT PA6 D12
// IN PA7 D11
void Init_HX711pin(void)
{
	printf("0");
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, ENABLE);	 //使能PF端口时钟
	printf("1");
	//HX711_SCK PA6 D12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
		printf("2");
	//HX711_DOUT PA7 D11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//输入上拉
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	printf("3");
	GPIO_SetBits(GPIOA,GPIO_Pin_6);					//初始化设置为0
}



//****************************************************
//读取HX711
//****************************************************
u32 HX711_Read(void)	//增益128
{
	unsigned long count; 
	unsigned char i; 
  	HX711_DOUT=1; 
	delay_us(1);
  	HX711_SCK=0; 
  	count=0; 
  	while(HX711_DOUT); 
  	for(i=0;i<24;i++)//24
	{ 
	  	HX711_SCK=1; 
	  	count=count<<1; 
		delay_us(1);
		HX711_SCK=0; 
	  	if(HX711_DOUT)
			count++; 
		delay_us(1);
	} 
 	HX711_SCK=1; 
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
	delay_us(1);
	HX711_SCK=0;  

	delay_us(1);
	HX711_SCK=1; 
	delay_us(1);
	HX711_SCK=0;
	delay_us(1);
	HX711_SCK=1; 
	delay_us(1);
	HX711_SCK=0;
	return(count);
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//称重
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	//if(HX711_Buffer > Weight_Maopi)			
	if (1)
	{
		Weight_Shiwu = HX711_Buffer;
		//Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//获取实物的AD采样数值。
	
		Weight_Shiwu = (s32)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量
																		//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
																		//当发现测试出来的重量偏大时，增加该数值。
																		//如果测试出来的重量偏小时，减小改数值。
	}

	
}
