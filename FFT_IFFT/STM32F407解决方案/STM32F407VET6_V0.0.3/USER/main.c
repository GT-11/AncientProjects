#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "adc.h"
#include "dac.h"
#include "math.h" 
#include "arm_math.h"  

#define FFT_LENGTH		1024 		//FFT长度，默认是1024点FFT

float fft_inputbuf[FFT_LENGTH*2];	//FFT输入数组
float fft_outputbuf[FFT_LENGTH];	//FFT输出数组
arm_cfft_radix4_instance_f32 scfft,scifft;

u16 fft[FFT_LENGTH];	//FFT输出数组
u16 ADC_Value[1024];
u16 DAC_Value[1024];
int main(void)
{
	u32 i;
	SystemInit();
	uart_init(115200);
	delay_init(84);

	
	Adc_Init();
	ADCDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,(u32)&ADC_Value,1024);
	
	
	Dac1_Init();
	DACDMA_Config(DMA1_Stream5,DMA_Channel_7,(u32)&DAC->DHR12R1,(u32)&DAC_Value,1024);
	
	
	
	
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
	arm_cfft_radix4_init_f32(&scifft,FFT_LENGTH,1,1);//初始化scfft结构体，设定FFT相关参数
  while(1)
	{
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 fft_inputbuf[2*i]=(ADC_Value[i]);
				 fft_inputbuf[2*i+1]=0;//虚部全部为0
			}
			arm_cfft_radix4_f32(&scfft,fft_inputbuf);	//FFT计算（基4）
			//arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//把运算结果复数求模得幅值 

//			for(i=4;i<16;i++)//生成信号序列
//			{
//				 fft_inputbuf[2*i]=fft_inputbuf[2*i]*2;
//				 fft_inputbuf[2*i+1]=fft_inputbuf[2*i+1]*2;//虚部全部为0
//			}
//			
//			for(i=16;i<1024;i++)//生成信号序列
//			{
//				 fft_inputbuf[2*i]=0;
//				 fft_inputbuf[2*i+1]=0;//虚部全部为0
//			}
			arm_cfft_radix4_f32(&scifft,fft_inputbuf);	//FFT计算（基4）
			//arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//把运算结果复数求模得幅值 
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 DAC_Value[i]=fft_inputbuf[2*i];//虚部全部为0
			}
			//DACDMA_Enable(DMA1_Stream5,64);
	}
}


