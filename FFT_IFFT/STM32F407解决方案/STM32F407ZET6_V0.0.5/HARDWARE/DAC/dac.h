#ifndef __DAC_H
#define __DAC_H	 
#include "sys.h"	     			    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DAC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved 
////////////////////////////////////////////////////////////////////////////////// 	

void Dac1_Init(void);		//DACͨ��1��ʼ��	 	 
void Dac1_Set_Vol(u16 vol);	//����ͨ��1�����ѹ
void DACDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr);
void DACDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);
#endif

















