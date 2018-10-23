#include "OLED64_32.h"	   
#define _74AC573_LE 8
#define _74AC573_OE 9
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO,ENABLE);
         GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 PBout(_74AC573_OE)=1;
	PBout(_74AC573_LE)=0;
	GPIOB->BSRR=(0x00 & 0xff) | (~0x00 & 0xff)<<16;
}
 
void Lock_74AC573(u8 data)
{
	//GPIOB->BSRR=(data & 0xff) | (~data & 0xff)<<16;
	
	GPIOB->BSRR = data&0xff;
	GPIOB->BRR = ~data&0xff;
	GPIOB->BSRR = 0x100;
	GPIOB->BRR = 0x100;
}
