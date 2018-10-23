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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

//	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		jishu++;
		}
}

 int main(void)
 {	 
 	u16 i=0,l=0,z=0,b=0,bb;	     
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	 TIM3_Int_Init(899,0);
	 
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




