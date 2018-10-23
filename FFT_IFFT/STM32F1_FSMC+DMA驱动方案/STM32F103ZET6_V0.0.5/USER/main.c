#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
#include "dac.h"
#include "math.h"
#include "FFT_IFFT.h"
//ALIENTEK战舰STM32开发板实验37
//外部SRAM 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
u16 SendBuff[128];
u16 ADC_Value[1024];
uint16_t DAC_Value[1024];
float Buffer_FFT_Data[8192];
float Buffer_FFT_Table[1028];



extern int N_FFT;
extern ptr_complex_of_N_FFT data_of_N_FFT;

 int main(void)
 {	 
 	u16 i=0;	     
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
	 
	 
	 //////////////////////////////////////////////////////
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,1024);//DMA1通道1,外设为ADC,存储器为ADC_Value,长度5168.  
  
	Adc_Init();
	
	ADCDMA_Enable(DMA1_Channel1);
	/////////////////////////////////////////////////////// 
	 
	 
	/////////////////////////////////////////////////// 
	TIM6_Configuration();
	 
	Dac1_Init();
	 
	DACDMA_Config(DMA2_Channel3,0x40007408,(u32)DAC_Value,1024);
	
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
	
	Init_FFT(1024,Buffer_FFT_Data,Buffer_FFT_Table);		//初始化各项参数，此函数只需执行一次
	while(1)
	{

    InputData(ADC_Value,0.01);		//输入原始数据
		ADCDMA_Enable(DMA1_Channel1);
    FFT();				//进行 FFT计算

//    printf("\n\n");
//    for (i=0; i<4096; i++)
//    {
//        printf("%f\n",RESULT(i));
//    }

    IFFT();//进行 IFFT计算
//    printf("\n\n");
   for (i=0; i<N_FFT; i++)
   {
      // printf("%f\n",data_of_N_FFT[i].real/N_FFT);
		  DAC_Value[i]=(data_of_N_FFT[i].real/N_FFT)*100;
   }

  //  Close_FFT();		//结束 FFT运算，释放相关内存
	}
}




