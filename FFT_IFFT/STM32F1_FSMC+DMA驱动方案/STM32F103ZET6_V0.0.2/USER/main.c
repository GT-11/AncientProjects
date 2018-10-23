#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
//ALIENTEK战舰STM32开发板实验37
//外部SRAM 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
u16 SendBuff[5200];
u16 ADC_Value[4096];
 int main(void)
 {	 
 	u16 i=0;	     
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
  FSMC_SRAM_Init();		//初始化外部SRAM  
	 
	FSMCDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1通道4,外设为FSMC,存储器为SendBuff,长度5168.   
	 
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,4096);//DMA1通道1,外设为ADC,存储器为ADC_Value,长度5168.  
  
	Adc_Init();
	 
//	 ADC_Value[3]=Get_Adc_Average(1,100);
  ADCDMA_Enable(DMA1_Channel1); 	 
	 
  while(i<255)
	{	
		//	*(vu8*)((u32)(0x68000000))=i;
		  SendBuff[i]=i;
			i++;
	}	
	FSMCDMA_Enable(DMA1_Channel4);
	while(1);
}




