#include "dac.h"
u16 DMA2_DAC_LEN;//����DMAÿ�����ݴ��͵ĳ��� 		
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//DAC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//DACͨ��1�����ʼ��
void Dac1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��PORTAͨ��ʱ��
   	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //ʹ��DACͨ��ʱ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // �˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //ģ������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 �����
					
	DAC_InitType.DAC_Trigger=DAC_Trigger_T6_TRGO;	//��ʹ�ô������� TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�ò��η���
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1�������ر� BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);	 //��ʼ��DACͨ��1

	//DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DAC1
  
   // DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
	//DAC_DMACmd(DAC_Channel_1, ENABLE);
}
void DACDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
       DMA_InitTypeDef            DMA_InitStructure;
       RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	     DMA2_DAC_LEN=cndtr;
       DMA_DeInit(DMA_CHx);
       DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;
       DMA_InitStructure.DMA_MemoryBaseAddr = cmar;
       DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
       DMA_InitStructure.DMA_BufferSize = cndtr;
       DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
       DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
       DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
       DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
       DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
       DMA_InitStructure.DMA_Priority = DMA_Priority_High;
       DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
       DMA_Init(DMA_CHx, &DMA_InitStructure);
 
      
       DMA_Cmd(DMA_CHx, ENABLE);
      
					DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DAC1
  
			//	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
				DAC_DMACmd(DAC_Channel_1, ENABLE);
	
       DMA_ITConfig(DMA_CHx,DMA_IT_TC,ENABLE);
			 
			 TIM_Cmd(TIM6, ENABLE);
}

//����һ��DMA����
void DACDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA1_Channel1,DMA2_DAC_LEN);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	
void TIM6_Configuration(void)
{
         TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
       TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
      TIM_TimeBaseStructure.TIM_Period = 0xd7;          //f=(72MHZ/(0x85+1))*6=89.552KHZ
      TIM_TimeBaseStructure.TIM_Prescaler = 0x06;      
      TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;   
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
      TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);     
      
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
}
//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	float temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}





















































