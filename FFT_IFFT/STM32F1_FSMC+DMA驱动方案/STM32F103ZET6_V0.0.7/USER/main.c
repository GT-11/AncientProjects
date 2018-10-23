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
//ALIENTEKս��STM32������ʵ��37
//�ⲿSRAM ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
u16 SendBuff[128];
u16 ADC_Value[1024];
uint16_t DAC_Value[1024];
//float Buffer_FFT_Data[8192];
//float Buffer_FFT_Table[1028];
u32 FFT_Out[1024];
u8 FFTOut[1024];
u8 box[128][32];
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
 int main(void)
 {	 
 	u16 i=0,l=0,z=0,b=0;	     
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	 
	 
	 //////////////////////////////////////////////////////
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,1024);//DMA1ͨ��1,����ΪADC,�洢��ΪADC_Value,����5168.  
  
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
  FSMC_SRAM_Init();		//��ʼ���ⲿSRAM  
	 
	FSMCDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1ͨ��4,����ΪFSMC,�洢��ΪSendBuff,����5168.   
	
	FSMCDMA_Enable(DMA1_Channel4);
///////////////////////////////////////////////////
	
	
	//    int i = 0;
//
//    Init_FFT(64);		//��ʼ������������˺���ֻ��ִ��һ��
//
//    InputData();		//����ԭʼ����
//    FFT();				//���� FFT����
//
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",RESULT(i));
//    }
//
//    IFFT();//���� IFFT����
//    printf("\n\n");
//    for (i=0; i<N_FFT; i++)
//    {
//        printf("%f\n",data_of_N_FFT[i].real/N_FFT);
//    }
//
//    Close_FFT();		//���� FFT���㣬�ͷ�����ڴ�
	
//	Init_FFT(64,Buffer_FFT_Data,Buffer_FFT_Table);		//��ʼ������������˺���ֻ��ִ��һ��
	while(1)
	{

//    InputData(ADC_Value,0.01);		//����ԭʼ����
//		ADCDMA_Enable(DMA1_Channel1);
//    FFT();				//���� FFT����
		
		for (i=0; i<1024; i++)
    {
			  DAC_Value[i]=ADC_Value[i]-1800;
    }
    cr4_fft_1024_stm32(FFT_Out,DAC_Value, 1024);
		GetPowerMag();
    //printf("\n");

			for (i=0; i<128; i++)
			{
				for(z=0;z<FFTOut[i];z+=6)
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
	//	for (i=0; i<128; i++)
	//	printf("\n");
		for(l=32;l>0;l--)
		{
			for (i=0; i<128; i++)
				box[i][l]=' ';
		}	
  //  delay_ms(1);
//    IFFT();//���� IFFT����
//    printf("\n\n");
//   for (i=0; i<N_FFT; i++)
//   {
//      // printf("%f\n",data_of_N_FFT[i].real/N_FFT);
//		  DAC_Value[i]=(data_of_N_FFT[i].real/N_FFT)*100;
//   }

  //  Close_FFT();		//���� FFT���㣬�ͷ�����ڴ�
	}
}




