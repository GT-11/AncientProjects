#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
#include "dac.h"
#include "math.h"
//ALIENTEK战舰STM32开发板实验37
//外部SRAM 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
u16 SendBuff[128];
u16 ADC_Value[4096];
uint16_t DAC_Value[4096];
 int main(void)
 {	 
// 	u16 i=0;	     
//	delay_init();	    	 //延时函数初始化	  
//	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	uart_init(9600);	 	//串口初始化为9600
//  FSMC_SRAM_Init();		//初始化外部SRAM  
//	 
//	FSMCDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1通道4,外设为FSMC,存储器为SendBuff,长度5168.   
//	 
//  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,4096);//DMA1通道1,外设为ADC,存储器为ADC_Value,长度5168.  
//  
//	Adc_Init();
//	
//	 
//	DACDMA_Config(DMA2_Channel3,DAC->DHR12R1,(u32)ADC_Value,4096);
//	
//	DACDMA_Enable(DMA2_Channel3); 	
//	 
//  Dac1_Init();	 
//	 
////	 ADC_Value[3]=Get_Adc_Average(1,100);
//  ADCDMA_Enable(DMA1_Channel1); 	 
//	 
////	Timerx_Init(72000000/20000,0); 
//	
//  while(i<255)
//	{	
//		//	*(vu8*)((u32)(0x68000000))=i;
//		  SendBuff[i]=i;
//			i++;
//	}	
//	//Dac1_Set_Vol(1000);
//	FSMCDMA_Enable(DMA1_Channel4);
	while(1)
	{
		//DACDMA_Enable(DMA1_Channel2); 	
		uint16_t i=0;
		double a=0;
		//DACDMA_Config(DMA2_Channel4,DAC->DHR12R1,(u32)ADC_Value,4096);
       Dac1_Init();	 
		for(i=0;i<4095;i++)
		{
			DAC_Value[i]=(uint16_t)((sin(a*3.1415926)+1)*2000);
			a+=0.03125;
		}
		i=0;
	//	DACDMA_Config(DMA2_Channel4,DAC->DHR12R1,(u32)DAC_Value,64);
		
	//	DACDMA_Enable(DMA2_Channel4); 	
        while(1)
         {   
           // DAC_SetChannel1Data(DAC_Align_12b_R,DAC_Value[i]);
					 DAC->DHR12R1=DAC_Value[i];
             i++;
            if(i==64) i=0;					 
					// DACDMA_Enable(DMA2_Channel4); 	
        }
	}
}




