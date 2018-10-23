#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
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
 							   
void  Random_Adc_Init(void);								 
void Adc_Init(u32 fs); 				//ADCͨ����ʼ��
void ADCDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u8* mar,u8* mar2,u16 ndtr);
void ADCDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);
u16  Get_Adc(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
void ADC_Rec_Start(void);
void ADC_Rec_Stop(void);
#endif 















