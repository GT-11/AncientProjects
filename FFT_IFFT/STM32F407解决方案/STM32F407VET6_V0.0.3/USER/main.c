#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "adc.h"
#include "dac.h"
#include "math.h" 
#include "arm_math.h"  

#define FFT_LENGTH		1024 		//FFT���ȣ�Ĭ����1024��FFT

float fft_inputbuf[FFT_LENGTH*2];	//FFT��������
float fft_outputbuf[FFT_LENGTH];	//FFT�������
arm_cfft_radix4_instance_f32 scfft,scifft;

u16 fft[FFT_LENGTH];	//FFT�������
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
	
	
	
	
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز���
	arm_cfft_radix4_init_f32(&scifft,FFT_LENGTH,1,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز���
  while(1)
	{
			for(i=0;i<FFT_LENGTH;i++)//�����ź�����
			{
				 fft_inputbuf[2*i]=(ADC_Value[i]);
				 fft_inputbuf[2*i+1]=0;//�鲿ȫ��Ϊ0
			}
			arm_cfft_radix4_f32(&scfft,fft_inputbuf);	//FFT���㣨��4��
			//arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//��������������ģ�÷�ֵ 

//			for(i=4;i<16;i++)//�����ź�����
//			{
//				 fft_inputbuf[2*i]=fft_inputbuf[2*i]*2;
//				 fft_inputbuf[2*i+1]=fft_inputbuf[2*i+1]*2;//�鲿ȫ��Ϊ0
//			}
//			
//			for(i=16;i<1024;i++)//�����ź�����
//			{
//				 fft_inputbuf[2*i]=0;
//				 fft_inputbuf[2*i+1]=0;//�鲿ȫ��Ϊ0
//			}
			arm_cfft_radix4_f32(&scifft,fft_inputbuf);	//FFT���㣨��4��
			//arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//��������������ģ�÷�ֵ 
			for(i=0;i<FFT_LENGTH;i++)//�����ź�����
			{
				 DAC_Value[i]=fft_inputbuf[2*i];//�鲿ȫ��Ϊ0
			}
			//DACDMA_Enable(DMA1_Stream5,64);
	}
}


