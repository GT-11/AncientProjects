#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "adc.h"
#include "dac.h"
#include "math.h" 
#include "arm_math.h"  
#include "sram.h"
#include <stdlib.h>
#include <stdio.h>
#include "bsp_tft_lcd.h"
#define FFT_LENGTH		1024 		//FFT长度，默认是1024点FFT

uint32_t  ADC_DAC_TIME=0x7aF;

float fft_inputbuf[FFT_LENGTH*2];	//FFT输入数组
float fft_outputbuf[FFT_LENGTH];	//FFT输出数组

arm_cfft_radix4_instance_f32 scfft,scifft;



u16 ADC_Value[1024];
u16 DAC_Value[1024];
u16 FSMC_Value[480*100];


int main(void)
{
	u32 i,j;
	u8 x;
	SystemInit();
	uart_init(115200);
	delay_init(84);

	Adc_Init();
	ADCDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,(u32)&ADC_Value,1024);
	
	Dac1_Init();
	DACDMA_Config(DMA1_Stream5,DMA_Channel_7,(u32)&DAC->DHR12R1,(u32)&DAC_Value,1024);
	
	
	LCD_InitHard();
//	FSMC_SRAM_Init();
//	for(i=0;i<FFT_LENGTH;i++)//生成信号序列
//	{
//			FSMC_Value[i]=i;
//	}
//	FSMCDMA_Config(DMA2_Stream7,DMA_Channel_1,(u32)&FSMC_Value,0x6C000000,1024);
	
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
	arm_cfft_radix4_init_f32(&scifft,FFT_LENGTH,1,1);//初始化scfft结构体，设定FFT相关参数
			
  while(1)
	{
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 fft_inputbuf[2*i]=(ADC_Value[i<<1]);
				 fft_inputbuf[2*i+1]=0;//虚部全部为0
			}
			arm_cfft_radix4_f32(&scfft,fft_inputbuf);	//FFT计算（基4）
			arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//把运算结果复数求模得幅值 
			
			
			
		
			
			
			

				//for(i=0;i<8;i++)
					RA8875_DrawBMP(0,100,480,100,FSMC_Value);

			
			
			
			
			
			
			arm_cfft_radix4_f32(&scifft,fft_inputbuf);	//IFFT计算（基4）
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 DAC_Value[i]=fft_inputbuf[2*i];//虚部全部为0
			}

	}
}


