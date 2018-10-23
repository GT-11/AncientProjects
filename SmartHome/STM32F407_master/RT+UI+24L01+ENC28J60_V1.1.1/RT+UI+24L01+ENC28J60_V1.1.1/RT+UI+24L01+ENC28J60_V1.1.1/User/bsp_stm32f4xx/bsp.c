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
//const u8 mymac[6]={0x00,0x00,0x00,0x00,0x00,0x00};	//MAC��ַ
uint32_t lwip_localtime=0;
/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void TIM3_Int_Init(u16 arr,u16 psc)
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//??TIM3??���
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;//?????��
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//????��'
	TIM_TimeBaseInitStructure.TIM_Period=arr;//?????��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//???TIM3����
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//?????3????��
	TIM_Cmd(TIM3,ENABLE);//?????���
  NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;//?????�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;//?????1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;//????3��
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void bsp_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
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
			
	}			

	NRF24L01_RX_Mode();
	TIM3_Int_Init(9,8399);
	
	//ENC28J60_SPI3_Init();
//	while(ENC28J60_Init((u8*)mymac));
//	bsp_InitLed(); 		/* ��ʼLEDָʾ�ƶ˿� */
	
	LCD_InitHard();	    /* ��ʼ����ʾ��Ӳ��(����GPIO��FSMC,��LCD���ͳ�ʼ��ָ��) */
	TOUCH_InitHard();
	
	

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_2| GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	//PG6 7 ���� 	 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ��©ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */  	 
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��ָ��IO
	GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12);//PG6,7,8����				
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

u8 bsp_GetKey(void)
{

	if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)) return 1;

	if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)) return 2;

	if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)) return 3;

	if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)) return 4;

	if(!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)) return 5;
	
	return 0;
}




void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		lwip_localtime++;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//???????
	}
}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
