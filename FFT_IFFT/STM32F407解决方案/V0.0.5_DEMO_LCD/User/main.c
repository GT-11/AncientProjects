/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V3.0
*	说    明 : 本实验主要
*              实验目的：
*                1. 学习FreeRTOS的任务控制函数使用。
*              实验内容：
*                1. 共创建了4个用户任务
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V5开发板光盘里面有。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期        作者     说明
*		v1.0    2013-09-10  Eric2013  创建该文件，ST固件库版本为V1.0.2
*		V2.0    2014-02-24  Eric2013  升级FreeRTOS到V8.0.0，升级F4固件库到1.3.0 
*	    V3.0    2015-04-01  Eric2013  1. 升级固件库到V1.5.0
*                                     2. 升级BSP板级支持包
*									  3. 升级FreeRTOS到V8.2.0
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "stm32f4xx.h"
#include "adc.h"
#include "dac.h"
#include "math.h" 
#include "arm_math.h"  
#include "sram.h"
#include <stdlib.h>
#include <stdio.h>
#define FFT_LENGTH		4096 		//FFT长度，默认是1024点FFT

float fft_inputbuf[FFT_LENGTH*2];	//FFT输入数组
float fft_outputbuf[FFT_LENGTH];	//FFT输出数组
arm_cfft_radix4_instance_f32 scfft,scifft;
u16 bbo[4096];
u16 ADC_Value[8192];
u16 DAC_Value[4096];
u16 FSMC_Value[1024];

char string_[40][256];
float cache[2048+4];	//FFT输出数组
float max[100];
u16 max_add[100];

u16 u16_cache[100];
float max_sort[100];
u16 max_add_sort[100];


/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
    /* 硬件初始化 */
    char x=0;
	u8 R=0,G=0,B=0,RF=1,GF=1,BF=1;
    u32 i,k=0;
	double buff;
	SystemInit();

    bsp_Init();
	  
	Adc_Init();
	ADCDMA_Config(DMA2_Stream0,DMA_Channel_0,(u32)&ADC1->DR,(u32)&ADC_Value,8192);
	
	Dac1_Init();
	DACDMA_Config(DMA1_Stream5,DMA_Channel_7,(u32)&DAC->DHR12R1,(u32)&DAC_Value,4096);
	
//	FSMC_SRAM_Init();
//	for(i=0;i<FFT_LENGTH;i++)//生成信号序列
//	{
//			FSMC_Value[i]=i;
//	}
//	FSMCDMA_Config(DMA2_Stream7,DMA_Channel_1,(u32)&FSMC_Value,0x6C000000,1024);
	
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
	//arm_cfft_radix4_init_f32(&scifft,FFT_LENGTH,1,1);//初始化scfft结构体，设定FFT相关参数
	RA8875_ClrScr(65535);
	
  while(1)
	{
			
			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
			{
				 fft_inputbuf[2*i]=(ADC_Value[i<<1]);
				 fft_inputbuf[2*i+1]=0;//虚部全部为0
				 bbo[i]=(ADC_Value[i<<1])/8.0;
			}
			arm_cfft_radix4_f32(&scfft,fft_inputbuf);	//FFT计算（基4）



			arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//把运算结果复数求模得幅值 
			
			
			fft_outputbuf[8]-=30000;
			
			
			
			////////////////////////////////////////////////////////////////////
			                //取数组前20最大的值存在max里 并将最大值的位置存在max_add里
			
			for(i=0;i<2048;i++)
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
				for(i=1;i<2048;i++)
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
			
			for(i=0;i<100;i++)
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
			
			

			
			
			
			
			
			
			
			
			
			
			
			
			
			snprintf(string_[0],   sizeof(string_),   "%d ",   max_add_sort[0]); 
			snprintf(string_[1],   sizeof(string_),   "%d ",   max_add_sort[1]); 
			snprintf(string_[2],   sizeof(string_),   "%d ",   max_add_sort[2]); 
			snprintf(string_[3],   sizeof(string_),   "%d ",   max_add_sort[3]); 
			snprintf(string_[4],   sizeof(string_),   "%d ",   max_add_sort[4]); 
			snprintf(string_[5],   sizeof(string_),   "%d ",   max_add_sort[5]);
			snprintf(string_[6],   sizeof(string_),   "%d ",   max_add_sort[6]); 
			snprintf(string_[7],   sizeof(string_),   "%d ",   max_add_sort[7]); 
			snprintf(string_[8],   sizeof(string_),   "%d ",   max_add_sort[8]);
			snprintf(string_[9],   sizeof(string_),   "%d ",   max_add_sort[9]); 
			snprintf(string_[10],   sizeof(string_),   "%f ",   max_sort[0]); 
			snprintf(string_[11],   sizeof(string_),   "%f ",   max_sort[1]); 
			snprintf(string_[12],   sizeof(string_),   "%f ",   max_sort[2]); 
			snprintf(string_[13],   sizeof(string_),   "%f ",   max_sort[3]); 
			snprintf(string_[14],   sizeof(string_),   "%f ",   max_sort[4]); 
			snprintf(string_[15],   sizeof(string_),   "%f ",   max_sort[5]);
			snprintf(string_[16],   sizeof(string_),   "%f ",   max_sort[6]); 
			snprintf(string_[17],   sizeof(string_),   "%f ",   max_sort[7]); 
			snprintf(string_[18],   sizeof(string_),   "%f ",   max_sort[8]);
			snprintf(string_[19],   sizeof(string_),   "%f ",   max_sort[9]); 
			

			snprintf(string_[20],   sizeof(string_),   "%f ",   max_add[0]*5.2083); 
			snprintf(string_[21],   sizeof(string_),   "%f ",   max_add[1]*5.2083); 
			snprintf(string_[22],   sizeof(string_),   "%f ",   max_add[2]*5.2083); 
			snprintf(string_[23],   sizeof(string_),   "%f ",   max_add[3]*5.2083); 
			snprintf(string_[24],   sizeof(string_),   "%f ",   max_add[4]*5.2083); 
			snprintf(string_[25],   sizeof(string_),   "%f ",   max_add[5]*5.2083);
			snprintf(string_[26],   sizeof(string_),   "%f ",   max_add[6]*5.2083); 
			snprintf(string_[27],   sizeof(string_),   "%f ",   max_add[7]*5.2083); 
			snprintf(string_[28],   sizeof(string_),   "%f ",   max_add[8]*5.2083);
			snprintf(string_[29],   sizeof(string_),   "%f ",   max_add[9]*5.2083); 
//			snprintf(string_[30],   sizeof(string_),   "%f ",   max[0]); 
//			snprintf(string_[31],   sizeof(string_),   "%f ",   max[1]); 
//			snprintf(string_[32],   sizeof(string_),   "%f ",   max[2]); 
//			snprintf(string_[33],   sizeof(string_),   "%f ",   max[3]); 
//			snprintf(string_[34],   sizeof(string_),   "%f ",   max[4]); 
//			snprintf(string_[35],   sizeof(string_),   "%f ",   max[5]);
//			snprintf(string_[36],   sizeof(string_),   "%f ",   max[6]); 
//			snprintf(string_[37],   sizeof(string_),   "%f ",   max[7]); 
//			snprintf(string_[38],   sizeof(string_),   "%f ",   max[8]);
//			snprintf(string_[39],   sizeof(string_),   "%f ",   max[9]); 
			
//		  RA8875_ClrScr(0);
//			RA8875_SetBackColor(0);
//			RA8875_SetFrontColor(65535);
			
			
			
			
			
			
			
			
			
			RA8875_DrawBMP(0,0,120,200,bbo);
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
//		  RA8875_DispAscii(10,0," Address:[0]:");
//			RA8875_DispAscii(114,0,string_[0]);RA8875_DispAscii(214,0,string_[20]);
//			RA8875_DispAscii(10,16," Address:[1]:");
//			RA8875_DispAscii(114,16,string_[1]);RA8875_DispAscii(214,16,string_[21]);
//			RA8875_DispAscii(10,32," Address:[2]:");
//			RA8875_DispAscii(114,32,string_[2]);RA8875_DispAscii(214,32,string_[22]);
//			RA8875_DispAscii(10,48," Address:[3]:");
//			RA8875_DispAscii(114,48,string_[3]);RA8875_DispAscii(214,48,string_[23]);
//			RA8875_DispAscii(10,64," Address:[4]:");
//			RA8875_DispAscii(114,64,string_[4]);RA8875_DispAscii(214,64,string_[24]);
//			RA8875_DispAscii(10,80," Address:[5]:");
//			RA8875_DispAscii(114,80,string_[5]);RA8875_DispAscii(214,80,string_[25]);
//			RA8875_DispAscii(10,96," Address:[6]:");
//			RA8875_DispAscii(114,96,string_[6]);RA8875_DispAscii(214,96,string_[26]);
//			RA8875_DispAscii(10,112," Address:[7]:");
//			RA8875_DispAscii(114,112,string_[7]);RA8875_DispAscii(214,112,string_[27]);
//			RA8875_DispAscii(10,128," Address:[8]:");
//			RA8875_DispAscii(114,128,string_[8]);RA8875_DispAscii(214,128,string_[28]);
//			RA8875_DispAscii(10,144," Address:[9]:");
//			RA8875_DispAscii(114,144,string_[9]);RA8875_DispAscii(214,144,string_[29]);
//		  RA8875_DispAscii(410,0,"Max Value[0]:");
//			RA8875_DispAscii(522,0,string_[10]);
//			RA8875_DispAscii(410,16,"Max Value[1]:");
//			RA8875_DispAscii(522,16,string_[11]);
//			RA8875_DispAscii(410,32,"Max Value[2]:");
//			RA8875_DispAscii(522,32,string_[12]);
//			RA8875_DispAscii(410,48,"Max Value[3]:");
//			RA8875_DispAscii(522,48,string_[13]);
//			RA8875_DispAscii(410,64,"Max Value[4]:");
//			RA8875_DispAscii(522,64,string_[14]);
//			RA8875_DispAscii(410,80,"Max Value[5]:");
//			RA8875_DispAscii(522,80,string_[15]);
//			RA8875_DispAscii(410,96,"Max Value[6]:");
//			RA8875_DispAscii(522,96,string_[16]);
//			RA8875_DispAscii(410,112,"Max Value[7]:");
//			RA8875_DispAscii(522,112,string_[17]);
//			RA8875_DispAscii(410,128,"Max Value[8]:");
//			RA8875_DispAscii(522,128,string_[18]);
//			RA8875_DispAscii(410,144,"Max Value[9]:");
//			RA8875_DispAscii(522,144,string_[19]);
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			
//			for(i=0,k=0;i<800;i++,k+=5)//生成信号序列
//			{
//					RA8875_DrawLine(i,bbo[i],i,bbo[i+1],RGB(255,G,B));
//			}

//			for(i=0;i<800;i++)
//			{
//				buff=480-(fft_outputbuf[i]*0.003);
//				
//				for(k=0;k<100;k++)
//				{
//					if(i==max_add[k])
//					{
//						if(buff<0) buff=0;
//						RA8875_DrawVLine(i,buff,480,RGB(k*3,255,2*k));
//						break;
//					}
//			  }
//				if(k==100)
//				RA8875_DrawVLine(i,buff,480,RGB(0,0,255));
//			}
//			for(i=0;i<800;i++)
//			{
//				buff=240-(fft_outputbuf[i+800]*0.001);
//				RA8875_DrawVLine(i,buff,240,RGB(R,G,255));
//			}	



//			arm_cfft_radix4_f32(&scifft,fft_inputbuf);	//IFFT计算（基4）
//			for(i=0;i<FFT_LENGTH;i++)//生成信号序列
//			{
//				 DAC_Value[i]=fft_inputbuf[2*i];//虚部全部为0
//			}

	}
}

