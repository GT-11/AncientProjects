#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
#include "dac.h"
#include "math.h"
#include "FFT_IFFT.h"
//ALIENTEKս��STM32������ʵ��37
//�ⲿSRAM ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	 
	 
	 //////////////////////////////////////////////////////
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,1024);//DMA1ͨ��1,����ΪADC,�洢��ΪADC_Value,����5168.  
  
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
	
	Init_FFT(1024,Buffer_FFT_Data,Buffer_FFT_Table);		//��ʼ������������˺���ֻ��ִ��һ��
	while(1)
	{

    InputData(ADC_Value,0.01);		//����ԭʼ����
		ADCDMA_Enable(DMA1_Channel1);
    FFT();				//���� FFT����

//    printf("\n\n");
//    for (i=0; i<4096; i++)
//    {
//        printf("%f\n",RESULT(i));
//    }

    IFFT();//���� IFFT����
//    printf("\n\n");
   for (i=0; i<N_FFT; i++)
   {
      // printf("%f\n",data_of_N_FFT[i].real/N_FFT);
		  DAC_Value[i]=(data_of_N_FFT[i].real/N_FFT)*100;
   }

  //  Close_FFT();		//���� FFT���㣬�ͷ�����ڴ�
	}
}




