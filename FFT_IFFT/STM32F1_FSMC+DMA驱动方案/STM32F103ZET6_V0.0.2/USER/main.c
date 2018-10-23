#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "sram.h" 
#include "adc.h"
//ALIENTEKս��STM32������ʵ��37
//�ⲿSRAM ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
u16 SendBuff[5200];
u16 ADC_Value[4096];
 int main(void)
 {	 
 	u16 i=0;	     
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
  FSMC_SRAM_Init();		//��ʼ���ⲿSRAM  
	 
	FSMCDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1ͨ��4,����ΪFSMC,�洢��ΪSendBuff,����5168.   
	 
  ADCDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_Value,4096);//DMA1ͨ��1,����ΪADC,�洢��ΪADC_Value,����5168.  
  
	Adc_Init();
	 
//	 ADC_Value[3]=Get_Adc_Average(1,100);
  ADCDMA_Enable(DMA1_Channel1); 	 
	 
  while(i<255)
	{	
		//	*(vu8*)((u32)(0x68000000))=i;
		  SendBuff[i]=i;
			i++;
	}	
	FSMCDMA_Enable(DMA1_Channel4);
	while(1);
}




