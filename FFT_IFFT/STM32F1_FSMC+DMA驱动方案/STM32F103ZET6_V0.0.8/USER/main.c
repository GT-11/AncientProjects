#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
#include "dac.h"
#include "math.h"
#include "FFT_IFFT.h"
#include "stm32_dsp.h"
#include "table_fft.h"

#define NPT 256
//ALIENTEK战舰STM32开发板实验37
//外部SRAM 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
u16 SendBuff[128];
u16 ADC_Value[1024];
u32 DAC_Value[1024];
u32 FFT_Value[1024];
u32 FFT_Out[1024];
u32 FFTOut[1024];

void GetPowerMag()
{

			int16_t lX,lY;
			uint16_t i;
			double mag;

			/* ???? */
			for (i=0; i< 1024; i++)
			{
				lX= (FFT_Out[i]<<16)>>16; /* ??*/
				lY=(FFT_Out[i]>> 16); /* ?? */ 
				mag =sqrtf(lX*lX+ lY*lY); /* ?? */
				FFTOut[i]=mag*2; /* ?????2??????,????????2 */
			}

			/* ???????2,???????????2 */
			FFTOut[0] =FFTOut[0]>>1;
			
}

 int main(void)
 {	 
 	u16 i=0;	     
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为9600
	 
	 
	 //////////////////////////////////////////////////////
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,1024);//DMA1通道1,外设为ADC,存储器为ADC_Value,长度5168.  
  
	Adc_Init();
	
	ADCDMA_Enable(DMA1_Channel1);
	/////////////////////////////////////////////////////// 
	 
	 
	/////////////////////////////////////////////////// 
	TIM6_Configuration();
	 
	Dac1_Init();
	 
	DACDMA_Config(DMA2_Channel3,0x40007408,(u32)ADC_Value,1024);
	
	DACDMA_Enable(DMA2_Channel3); 	 
 //////////////////////////////////////////////////////
 

  while(i<255)
	{	
		//	*(vu8*)((u32)(0x68000000))=i;
		  SendBuff[i]=i;
			i++;
	}	
	
	/////////////////////////////////////////////////
  FSMC_SRAM_Init();		//初始化外部SRAM  
	 
	FSMCDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1通道4,外设为FSMC,存储器为SendBuff,长度5168.   
	
	FSMCDMA_Enable(DMA1_Channel4);
///////////////////////////////////////////////////

	while(1)
	{
		for (i=0; i<1024; i++)
    {
			  FFT_Value[i]=(ADC_Value[i]-1820)<<16;
    }
    cr4_fft_1024_stm32(FFT_Out,FFT_Value, 1024);
		GetPowerMag();


	}
}




