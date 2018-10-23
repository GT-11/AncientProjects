#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
#include "dac.h"
#include "math.h"


u16 SendBuff[128];
u16 ADC_Value[4096];
u16 DAC_Value[4096];

 int main(void)
 {	 
 	u16 i=0,xx=3500;	     
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
  FSMC_SRAM_Init();		//初始化外部SRAM  
	 
	FSMCDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1通道4,外设为FSMC,存储器为SendBuff,长度5168.   
	 
	 
	 
	
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,4096);//DMA1通道1,外设为ADC,存储器为ADC_Value,长度5168.  
  
	Adc_Init();
	
	ADCDMA_Enable(DMA1_Channel1); 
	 
	 
	 
	TIM6_Configuration();
	 
	Dac1_Init();
	 
	DACDMA_Config(DMA2_Channel3,0x40007408,(u32)DAC_Value,4096);
	
	DACDMA_Enable(DMA2_Channel3); 	 
//  Dac1_Init();	 
 
	 
	 
	 
	 
//	 ADC_Value[3]=Get_Adc_Average(1,100);
	 
	 
//	Timerx_Init(72000000/20000,0); 
	
  while(i<255)
	{	
		//	*(vu8*)((u32)(0x68000000))=i;
		  SendBuff[i]=i;
			i++;
	}	
	//Dac1_Set_Vol(1000);
	FSMCDMA_Enable(DMA1_Channel4);

	while(1)
	{
//      DAC_SetChannel1Data(DAC_Align_12b_R,ADC_Value[i]);
//		  i++;
//		if(i==4096) i=0;
		for(i=0;i<4096;i++)
		{
			DAC_Value[i]=(ADC_Value[i]*20)-35000;
		}
	}
}




