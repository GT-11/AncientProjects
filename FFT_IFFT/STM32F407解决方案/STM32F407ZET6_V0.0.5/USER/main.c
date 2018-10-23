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
#define FFT_LENGTH		4096 		//FFT长度，默认是1024点FFT

uint32_t  ADC_DAC_TIME=0x7aF;
float fft_inputbuf_[(FFT_LENGTH*2)+100];
float fft_inputbuf[FFT_LENGTH*2];	//FFT输入数组
float fft_outputbuf[FFT_LENGTH];	//FFT输出数组
arm_cfft_radix4_instance_f32 scfft,scifft;



u16 ADC_Value[8192];
u16 DAC_Value[4096];
u16 FSMC_Value[1024];


float cache[1024+4];	//FFT输出数组
float max[100];
u16 max_add[100];
u16 k=0;

u16 u16_cache[100];
float max_sort[100];
u16 max_add_sort[100];

int F_Dev[100];
float F_Coe[100];

float Range_Signal_A[100];
int Position_Signal_A[100];

float Range_Signal_B[100];
int Position_Signal_B[100];
int main(void)
{
	u32 i;
	u8 aaa=0,bbb,x;
	SystemInit();
	uart_init(115200);
	delay_init(84);

	Adc_Init();
	ADCDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,(u32)&ADC_Value,8192);
	
	Dac1_Init();
	DACDMA_Config(DMA1_Stream5,DMA_Channel_7,(u32)&DAC->DHR12R1,(u32)&DAC_Value,4096);
	
	FSMC_SRAM_Init();
	for(i=0;i<FFT_LENGTH;i++)//生成信号序列
	{
			FSMC_Value[i]=i;
	}
	FSMCDMA_Config(DMA2_Stream7,DMA_Channel_1,(u32)&FSMC_Value,0x6C000000,1024);
	
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
	arm_cfft_radix4_init_f32(&scifft,FFT_LENGTH,1,1);//初始化scfft结构体，设定FFT相关参数
	
	
				for(k=0;k<100;k++) 
			{
				Range_Signal_B[k]=0;
				Range_Signal_A[k]=0;
				Position_Signal_A[k]=0;
				Position_Signal_B[k]=0;
				F_Dev[k]=0;
				F_Coe[k]=1;
			}
			
			
  while(1)
	{
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 fft_inputbuf[2*i]=(ADC_Value[i<<1]);
				 fft_inputbuf[2*i+1]=0;//虚部全部为0
			}
			arm_cfft_radix4_f32(&scfft,fft_inputbuf);	//FFT计算（基4）
			arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//把运算结果复数求模得幅值 
			
			
			
			////////////////////////////////////////////////////////////////////
			                //取数组前20最大的值存在max里 并将最大值的位置存在max_add里
			
			for(i=0;i<1024;i++)
			{
				cache[i]=fft_outputbuf[i];
			}
			for(i=0;i<100;i++)
			{
				max[i]=-1;
			}
			for(i=0;i<100;i++)
			{
				max_add[i]=0;
			}

			
			
			for(k=0;k<100;k++)
			{
				for(i=1;i<1024;i++)
				{
					if(max[k]<cache[i])
					{
						max[k]=cache[i];
						max_add[k]=i;
					}
				}
				
				
				cache[abs(max_add[k]-4)]=-1;	
  			cache[abs(max_add[k]-3)]=-1;
				cache[abs(max_add[k]-2)]=-1;
				cache[abs(max_add[k]-1)]=-1;
				cache[max_add[k]]=-1;
				cache[max_add[k]+1]=-1;
				cache[max_add[k]+2]=-1;
				cache[max_add[k]+3]=-1;
				cache[max_add[k]+4]=-1;	
		  }
			///////////////////////////////////////////////////////////////////////
			for(i=0;i<100;i++)                    //从新排列顺序，按照频谱由低到高排列
			{
				u16_cache[i]=max_add[i];
				max_add_sort[i]=0xfffe;
			}
			
			
			for(i=0;i<100;i++)
			{
				for(k=0;k<100;k++)
					if(max_add_sort[i]>u16_cache[k])
					{
						max_add_sort[i]=u16_cache[k];
						x=k;
					}

				max_sort[i]=fft_outputbuf[u16_cache[x]];
				u16_cache[x]=0xffff;
		  }

//////////////////////////////////////////////////////////////////

//			if(aaa==1)
//			{
//				for(i=0;i<100;i++)
//				{
//					Range_Signal_A[i]=(Range_Signal_A[i]+max_sort[i])/2.0;
//					Position_Signal_A[i]=(Position_Signal_A[i]+max_add_sort[i])/2.0;
//				}
//			}		
//			
//			if(aaa==2)
//			{
//				for(i=0;i<100;i++)
//				{
//					Range_Signal_B[i]=(Range_Signal_B[i]+max_sort[i])/2.0;
//					Position_Signal_B[i]=max_add_sort[i];
//				}
//			}
//			
//			if(aaa==3)
//			{
//				for(i=0;i<100;i++)
//				{
//					F_Dev[i]=Position_Signal_A[i]-Position_Signal_B[i];
//					F_Coe[i]=Range_Signal_B[i]/Range_Signal_A[i];
//				}
//			}
			
			
			
			
			
///////////////////////////////////////////////////////////////////////////////////
			for(k=0;k<8192;k++)             /////////////////////////信号处理（根据某特征参数调整原始信号） 参数：幅度系数、偏移量
			{
				fft_inputbuf_[k]=0;
			}
			
			fft_inputbuf_[0]=fft_inputbuf[0];
			fft_inputbuf_[1]=fft_inputbuf[1];
			
//			fft_inputbuf_[120]=1000000;
//			fft_inputbuf_[8192-120]=1000000;
			//fft_inputbuf_[601]=0;
			
			
			for(k=0;k<100;k++)
			{
				
					if(max_add_sort[k]>5)
					{
						fft_inputbuf_[(max_add_sort[k]+F_Dev[k]-1)*2]= fft_inputbuf[abs(max_add_sort[k]-1)*2]*F_Coe[k];
						fft_inputbuf_[((max_add_sort[k]+F_Dev[k]-1)*2)+1]= fft_inputbuf[(abs(max_add_sort[k]-1)*2)+1]*F_Coe[k];
					
						fft_inputbuf_[(max_add_sort[k]+F_Dev[k]-2)*2]= fft_inputbuf[abs(max_add_sort[k]-2)*2]*F_Coe[k];
						fft_inputbuf_[((max_add_sort[k]+F_Dev[k]-2)*2)+1]= fft_inputbuf[(abs(max_add_sort[k]-2)*2)+1]*F_Coe[k];
						
						fft_inputbuf_[(max_add_sort[k]+F_Dev[k]-3)*2]= fft_inputbuf[abs(max_add_sort[k]-3)*2]*F_Coe[k];
						fft_inputbuf_[((max_add_sort[k]+F_Dev[k]-3)*2)+1]= fft_inputbuf[(abs(max_add_sort[k]-3)*2)+1]*F_Coe[k];
						
						fft_inputbuf_[(max_add_sort[k]+F_Dev[k]-4)*2]= fft_inputbuf[abs(max_add_sort[k]-4)*2]*F_Coe[k];
						fft_inputbuf_[((max_add_sort[k]+F_Dev[k]-4)*2)+1]= fft_inputbuf[(abs(max_add_sort[k]-4)*2)+1]*2*F_Coe[k];
					}
				//k=0;
				fft_inputbuf_[(max_add_sort[k]+F_Dev[k])*2]= fft_inputbuf[max_add_sort[k]*2]*F_Coe[k];
				fft_inputbuf_[((max_add_sort[k]+F_Dev[k])*2)+1]= fft_inputbuf[(max_add_sort[k]*2)+1]*F_Coe[k];
				
				fft_inputbuf_[(max_add_sort[k]+F_Dev[k]+1)*2]= fft_inputbuf[(max_add_sort[k]+1)*2]*F_Coe[k];
				fft_inputbuf_[((max_add_sort[k]+F_Dev[k]+1)*2)+1]= fft_inputbuf[((max_add_sort[k]+1)*2)+1]*F_Coe[k];
				
				fft_inputbuf_[(max_add_sort[k]+F_Dev[k]+2)*2]= fft_inputbuf[(max_add_sort[k]+2)*2]*F_Coe[k];
				fft_inputbuf_[((max_add_sort[k]+F_Dev[k]+2)*2)+1]= fft_inputbuf[((max_add_sort[k]+2)*2)+1]*F_Coe[k];
				
				fft_inputbuf_[(max_add_sort[k]+F_Dev[k]+3)*2]= fft_inputbuf[(max_add_sort[k]+3)*2]*F_Coe[k];
				fft_inputbuf_[((max_add_sort[k]+F_Dev[k]+3)*2)+1]= fft_inputbuf[((max_add_sort[k]+3)*2)+1]*F_Coe[k];
				
				fft_inputbuf_[(max_add_sort[k]+F_Dev[k]+4)*2]= fft_inputbuf[(max_add_sort[k]+4)*2]*F_Coe[k];
				fft_inputbuf_[((max_add_sort[k]+F_Dev[k]+4)*2)+1]= fft_inputbuf[((max_add_sort[k]+4)*2)+1]*F_Coe[k];
				
			}

/////////////////////////////////////////////////////////////////////////////////////////////////

//			  for(k=800;k>1;k--)
//				{
//					fft_inputbuf_[(k+100)*2]=fft_inputbuf[k*2];
//					fft_inputbuf_[((k+100)*2)+1]=fft_inputbuf[(k*2)+1];
//					//fft_inputbuf[k*2]=0;
//					//fft_inputbuf[(k*2)+1]=0;
//					
//					fft_inputbuf_[(4096-k-100)*2]=fft_inputbuf[(4096-k)*2];
//					fft_inputbuf_[((4096-k-100)*2)+1]=fft_inputbuf[((4096-k)*2)+1];
//					//fft_inputbuf[(4096-k)*2]=0;
//					//fft_inputbuf[((4096-k)*2)+1]=0;
//				}


//			for(k=0;k<8192;k++)
//			{
//				fft_inputbuf_[k]=fft_inputbuf[k];
//			}
			
			
			
			
			
			
			arm_cfft_radix4_f32(&scifft,fft_inputbuf_);	//IFFT计算（基4）
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 DAC_Value[i]=fft_inputbuf_[2*i];//虚部全部为0
			}

	}
}


