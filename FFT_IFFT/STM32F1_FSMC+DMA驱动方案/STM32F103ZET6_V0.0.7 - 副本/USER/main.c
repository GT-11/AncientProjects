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
//float Buffer_FFT_Data[8192];
//float Buffer_FFT_Table[1028];
u32 FFT_Out[1024];
u8 FFTOut[1024];
u8 box[128][32];
u32 jishu=0;
//extern int N_FFT;
//extern ptr_complex_of_N_FFT data_of_N_FFT;
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
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		jishu++;
		}
}

 int main(void)
 {	 
 	u16 i=0,l=0,z=0,b=0,bb;	     
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为9600
	 TIM3_Int_Init(899,0);
	 
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
	
	
	//    int i = 0;
//
//    Init_FFT(64);		//初始化各项参数，此函数只需执行一次
//
//    InputData();		//输入原始数据
//    FFT();				//进行 FFT计算
//
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",RESULT(i));
//    }
//
//    IFFT();//进行 IFFT计算
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",data_of_N_FFT[i].real/N_FFT);
//    }
//
//    Close_FFT();		//结束 FFT运算，释放相关内存
	
//	Init_FFT(64,Buffer_FFT_Data,Buffer_FFT_Table);		//初始化各项参数，此函数只需执行一次
	while(1)
	{

//    InputData(ADC_Value,0.01);		//输入原始数据
//		ADCDMA_Enable(DMA1_Channel1);
//    FFT();				//进行 FFT计算
		
		for (i=0; i<1024; i++)
    {
			  FFT_Value[i]=(ADC_Value[i]-1820)<<16;
    }
		TIM_Cmd(TIM3, ENABLE); 
    cr4_fft_1024_stm32(FFT_Out,FFT_Value, 1024);
		TIM_Cmd(TIM3, DISABLE); 
		GetPowerMag();
		
		for(i=0;i<1024;i++)
		{
			FFT_Out[i]=FFT_Out[i]/2;
			bb=-(FFT_Out[i]>>16);
			FFT_Out[i]&=0x0000ffff;
			FFT_Out[i]|=bb<<16;
		}
		
		cr4_fft_1024_stm32(DAC_Value,FFT_Out, 1024);
    //printf("\n");
    for(i=0;i<1024;i++)
		{
			bb=-(DAC_Value[i]>>16);
			DAC_Value[i]&=0x0000ffff;
			DAC_Value[i]|=bb<<16;
		}
		
		
		
			for (i=0; i<128; i++)
			{
				for(z=0;z<FFTOut[i];z+=10)
				{
					if(l<32)
					box[i][l]='*';
					l++;
				}
				l=0;
			}
    for(l=32;l>0;l--)
		{
			for (i=0; i<128; i++)
				printf("%c",box[i][l]);
			printf("\n");
		}	
		printf("%d\n",jishu);
		jishu=0;
	//	for (i=0; i<128; i++)
	//	printf("\n");
		for(l=32;l>0;l--)
		{
			for (i=0; i<128; i++)
				box[i][l]=' ';
		}	
  //  delay_ms(1);
//    IFFT();//进行 IFFT计算
//    printf("\n\n");
//   for (i=0; i<N_FFT; i++)
//   {
//      // printf("%f\n",data_of_N_FFT[i].real/N_FFT);
//		  DAC_Value[i]=(data_of_N_FFT[i].real/N_FFT)*100;
//   }

  //  Close_FFT();		//结束 FFT运算，释放相关内存
	}
}




