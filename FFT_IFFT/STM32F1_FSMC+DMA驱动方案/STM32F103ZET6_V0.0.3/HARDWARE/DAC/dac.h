#ifndef __DAC_H
#define __DAC_H	 
#include "sys.h"	    
#include "stm32f10x_dac.h"					   
#include "stm32f10x_dma.h"					    
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
								    

void Dac1_Init(void);//�ػ�ģʽ��ʼ��		 	 
void Dac1_Set_Vol(u16 vol);
void DACDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void DACDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
void Timerx_Init(void);
#endif

















