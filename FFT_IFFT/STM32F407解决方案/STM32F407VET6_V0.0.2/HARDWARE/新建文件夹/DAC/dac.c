#include "dac.h"
u16 DMA2_DAC_LEN;//保存DMA每次数据传送的长度 		
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//DAC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/8
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//DAC通道1输出初始化
void Dac1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能PORTA通道时钟
   	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //使能DAC通道时钟 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // 端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //模拟输入
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 输出高
					
	DAC_InitType.DAC_Trigger=DAC_Trigger_T6_TRGO;	//不使用触发功能 TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1输出缓存关闭 BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1

	//DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC1
  
   // DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
	//DAC_DMACmd(DAC_Channel_1, ENABLE);
}
void DACDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
       DMA_InitTypeDef            DMA_InitStructure;
       RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	     DMA2_DAC_LEN=cndtr;
       DMA_DeInit(DMA_CHx);
       DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;
       DMA_InitStructure.DMA_MemoryBaseAddr = cmar;
       DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
       DMA_InitStructure.DMA_BufferSize = cndtr;
       DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
       DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
       DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
       DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
       DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
       DMA_InitStructure.DMA_Priority = DMA_Priority_High;
       DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
       DMA_Init(DMA_CHx, &DMA_InitStructure);
 
      
       DMA_Cmd(DMA_CHx, ENABLE);
      
					DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC1
  
			//	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
				DAC_DMACmd(DAC_Channel_1, ENABLE);
	
       DMA_ITConfig(DMA_CHx,DMA_IT_TC,ENABLE);
			 
			 TIM_Cmd(TIM6, ENABLE);
}

//开启一次DMA传输
void DACDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA1_Channel1,DMA2_DAC_LEN);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	
void TIM6_Configuration(void)
{
         TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
       TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
      TIM_TimeBaseStructure.TIM_Period = 0xd7;          //f=(72MHZ/(0x85+1))*6=89.552KHZ
      TIM_TimeBaseStructure.TIM_Prescaler = 0x06;      
      TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;   
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
      TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);     
      
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
}
//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	float temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12位右对齐数据格式设置DAC值
}





















































