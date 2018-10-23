/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32F4XX)
*	�ļ����� : bsp.c
*	��    �� : V1.1
*	˵    �� : ����Ӳ���ײ�������������ļ���ÿ��c�ļ����� #include "bsp.h" ���������е���������ģ�顣
*			   bsp = Borad surport packet �弶֧�ְ�
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-03-01  armfly   ��ʽ����
*		V1.1    2013-06-20  armfly   �淶ע�ͣ���ӱ�Ҫ˵��
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
extern u8 TX_ADDRESS[TX_ADR_WIDTH];
extern u8 TX_ADDRESS2[TX_ADR_WIDTH];
extern u8 RX_ADDRESS[RX_ADR_WIDTH];
/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
//	u8 buf[32];
	u32 x=0;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    
	bsp_InitDWT();      /* ��ʼ��DWT */
	bsp_InitUart(); 	/* ��ʼ������ */
//	bsp_InitKey();		/* ��ʼ������������������ bsp_InitTimer() ֮ǰ���ã� */
	bsp_InitI2C();
	NRF24L01_Init(); 
	for(x=0;x<500000;x++);  
//	bsp_InitSPIBus();	/* ����SPI���� */
	while(NRF24L01_Check())	//���NRF24L01�Ƿ���λ.	
	{
		for(x=0;x<5000;x++);
			
	}			
//for(x=0;x<5000;x++);
	NRF24L01_RX_Mode(RX_ADDRESS);
//	for(x=0;x<5000;x++);
	
//		while(1)
//		{	  		   				 
//			NRF24L01_RxPacket(buf);
//			printf("s");
//			printf("%s",buf);
//		  for(x=0;x<500000;x++);  
//		}
//		while(1)
//		{
//			NRF24L01_TxPacket("1234567890");
//			for(x=0;x<500000;x++);  
//		}
	
	
	
	
//	bsp_InitLed(); 		/* ��ʼLEDָʾ�ƶ˿� */
	
	LCD_InitHard();	    /* ��ʼ����ʾ��Ӳ��(����GPIO��FSMC,��LCD���ͳ�ʼ��ָ��) */
	TOUCH_InitHard();
}

/*
*********************************************************************************************************
*    �� �� ��: bsp_DelayUS
*    ����˵��: us���ӳ١�
*    ��    ��:  n : �ӳٳ��ȣ���λ1 us
*    �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
		    	 
	ticks = _ulDelayTime * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */ 		 
	tcnt = 0;
	told = DWT_CYCCNT;         /* �ս���ʱ�ļ�����ֵ */

	while(1)
	{
		tnow = DWT_CYCCNT;	
        
		if(tnow != told)
		{	
		    /* 32λ�������ǵ��������� */    
			if(tnow > told)
			{
				tcnt += tnow - told;	
			}
			/* ����װ�� */
			else 
			{
				tcnt += UINT32_MAX - told + tnow;	
			}	
			
			told = tnow;

			/*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
			if(tcnt >= ticks)break;
		}  
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
