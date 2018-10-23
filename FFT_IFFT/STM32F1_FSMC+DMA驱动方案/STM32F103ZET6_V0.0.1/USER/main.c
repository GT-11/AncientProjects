#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "sram.h" 
//ALIENTEKս��STM32������ʵ��37
//�ⲿSRAM ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
u16 SendBuff[5200];
 int main(void)
 {	 
 	u16 i=0;	     
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
  FSMC_SRAM_Init();		//��ʼ���ⲿSRAM  
	MYDMA_Config(DMA1_Channel4,0x68000000,(u32)SendBuff,128);//DMA1ͨ��4,����Ϊ����1,�洢��ΪSendBuff,����5168.   
  	 
  while(i<255)
	{	
		//	*(vu8*)((u32)(0x68000000))=i;
		  SendBuff[i]=i;
			i+=1;
	}	
	MYDMA_Enable(DMA1_Channel4);
	while(1);
}




