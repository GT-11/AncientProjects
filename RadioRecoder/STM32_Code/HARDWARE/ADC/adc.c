#include "adc.h"
#include "delay.h"	
#include "recorder.h" 
#include "RecordingGyroAccelerometerData.h"
#define DMA_Streamx_IRQn 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern uint32_t ADC_DAC_TIME;
//u8 DMA_Rx_Flag=0;
void TIM3_Configuration(u16 arr,u32 psc) 
{
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
NVIC_InitTypeDef NVIC_InitStructure; 

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //???? 

//???TIM3??? 
TIM_TimeBaseStructure.TIM_Period = arr; //???????????????????????????	
TIM_TimeBaseStructure.TIM_Prescaler =psc; //??????TIMx??????????? 
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //??????:TDTS = Tck_tim 
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM?????? 
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //??????????TIMx??????? 
  
TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE ); //?????TIM3??,?????? 
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //???,?????????????? 
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);           //??TIM2?update???????? 	
     
  //?????NVIC?? 
NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3?? 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //?????0? 
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //????3? 
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ????? 
NVIC_Init(&NVIC_InitStructure);  //???NVIC??? 

TIM_Cmd(TIM3, ENABLE);  //??TIMx	 
}
void TIM3_IRQHandler(void)   //TIM3?? 
{ 
if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //??TIM3???????? 
    { 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //??TIMx??????  
    } 
		
}
void  Random_Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
 
	ADC_Cmd(ADC1, ENABLE);//����ADת����	

}				  
//��ʼ��ADC															   
void  Adc_Init(u32 fs)
{    
	u32 times;
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	times = (unsigned int)((50000000.0/fs));
	
	TIM3_Configuration(1, times); 
	
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMAʹ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_3Cycles );
	
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); //????????DMA??
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����	
	
	ADC_SoftwareStartConv(ADC1);


  ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); //????????DMA??
	
  ADC_DMACmd(ADC1,ENABLE);//??ADC?DMA??

}				  


//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void ADCDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u8* mar,u8* mar2,u16 ndtr)
{ 
  NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ�� 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//�ȴ�DMA������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)mar;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = ndtr;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;//�洢�����ݳ���:16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ������ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA_Streamx, &DMA_InitStructure);//��ʼ��DMA Stream
	
		
	DMA_DoubleBufferModeConfig(DMA_Streamx,(u32)mar2,DMA_Memory_0);//˫����ģʽ����
 
  DMA_DoubleBufferModeCmd(DMA_Streamx,ENABLE);//˫����ģʽ����
 
  DMA_ITConfig(DMA_Streamx,DMA_IT_TC,ENABLE);//������������ж�
		
	DMA_Cmd(DMA_Streamx, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0x02;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
} 

//I2S DMA�ص�����ָ��
void (*adc_tx_callback)(void);	//TX�ص�����
void (*adc_rx_callback)(void);	//RX�ص�����
//DMA2_Stream0�жϷ�����
void DMA2_Stream0_IRQHandler(void)
{      
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0)==SET)	//DMA1_Stream3,������ɱ�־
	{ 
		DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);	//�����������ж�
		wav_second_level_cache();
		gad_second_level_cache();
		//DMA_Rx_Flag = 1;
      //	adc_rx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}  											 
} 
//I2S��ʼ¼��
void ADC_Rec_Start(void)
{   	    
	DMA_Cmd(DMA2_Stream0,ENABLE);//����DMA TX����,��ʼ¼��	
}
//�ر�I2S¼��
void ADC_Rec_Stop(void)
{  
	DMA_Cmd(DMA2_Stream0,DISABLE);//�ر�DMA,����¼��	 
	 
}
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void ADCDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
 
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
}	  




//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
	  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 
	 









