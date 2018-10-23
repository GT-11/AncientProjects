/*
*********************************************************************************************************
*
*	Ä£¿éÃû³Æ : BSPÄ£¿é(For STM32F4XX)
*	ÎÄ¼þÃû³Æ : bsp.c
*	°æ    ±¾ : V1.1
*	Ëµ    Ã÷ : ÕâÊÇÓ²¼þµ×²ãÇý¶¯³ÌÐòµÄÖ÷ÎÄ¼þ¡£Ã¿¸öcÎÄ¼þ¿ÉÒÔ #include "bsp.h" À´°üº¬ËùÓÐµÄÍâÉèÇý¶¯Ä£¿é¡£
*			   bsp = Borad surport packet °å¼¶Ö§³Ö°ü
*	ÐÞ¸Ä¼ÇÂ¼ :
*		°æ±¾ºÅ  ÈÕÆÚ        ×÷Õß     ËµÃ÷
*		V1.0    2013-03-01  armfly   ÕýÊ½·¢²¼
*		V1.1    2013-06-20  armfly   ¹æ·¶×¢ÊÍ£¬Ìí¼Ó±ØÒªËµÃ÷
*
*	Copyright (C), 2013-2014, °²¸»À³µç×Ó www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
//const u8 mymac[6]={0x00,0x00,0x00,0x00,0x00,0x00};	//MACµØÖ·
uint32_t lwip_localtime=0;
/*
*********************************************************************************************************
*	º¯ Êý Ãû: bsp_Init
*	¹¦ÄÜËµÃ÷: ³õÊ¼»¯ËùÓÐµÄÓ²¼þÉè±¸¡£¸Ãº¯ÊýÅäÖÃCPU¼Ä´æÆ÷ºÍÍâÉèµÄ¼Ä´æÆ÷²¢³õÊ¼»¯Ò»Ð©È«¾Ö±äÁ¿¡£Ö»ÐèÒªµ÷ÓÃÒ»´Î
*	ÐÎ    ²Î£ºÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/

void TIM3_Int_Init(u16 arr,u16 psc)
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//??TIM3??   
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;//?????  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//????  '
	TIM_TimeBaseInitStructure.TIM_Period=arr;//?????  
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//???TIM3    
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//?????3????  
	TIM_Cmd(TIM3,ENABLE);//?????   
  NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;//????? 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;//?????1  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;//????3  
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void bsp_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	u32 x=0;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    
	bsp_InitDWT();      /* ³õÊ¼»¯DWT */
	bsp_InitUart(); 	/* ³õÊ¼»¯´®¿Ú */
//	bsp_InitKey();		/* ³õÊ¼»¯°´¼ü±äÁ¿£¨±ØÐëÔÚ bsp_InitTimer() Ö®Ç°µ÷ÓÃ£© */
	bsp_InitI2C();
	NRF24L01_Init(); 
	for(x=0;x<500000;x++);  
//	bsp_InitSPIBus();	/* ÅäÖÃSPI×ÜÏß */
	while(NRF24L01_Check())	//¼ì²éNRF24L01ÊÇ·ñÔÚÎ».	
	{
			
	}			

	NRF24L01_RX_Mode();
	TIM3_Int_Init(9,8399);
	
	//ENC28J60_SPI3_Init();
//	while(ENC28J60_Init((u8*)mymac));
//	bsp_InitLed(); 		/* ³õÊ¼LEDÖ¸Ê¾µÆ¶Ë¿Ú */
	
	LCD_InitHard();	    /* ³õÊ¼»¯ÏÔÊ¾Æ÷Ó²¼þ(ÅäÖÃGPIOºÍFSMC,¸øLCD·¢ËÍ³õÊ¼»¯Ö¸Áî) */
	TOUCH_InitHard();
	
	

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_2| GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	//PG6 7 ÍÆÍì 	 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ÉèÎªÊä³ö¿Ú */
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ÉèÎª¿ªÂ©Ä£Ê½ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ÉÏÏÂÀ­µç×è²»Ê¹ÄÜ */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO¿Ú×î´óËÙ¶È */  	 
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//³õÊ¼»¯Ö¸¶¨IO
	GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12);//PG6,7,8ÉÏÀ­				
}

/*
*********************************************************************************************************
*    º¯ Êý Ãû: bsp_DelayUS
*    ¹¦ÄÜËµÃ÷: us¼¶ÑÓ³Ù¡£
*    ÐÎ    ²Î:  n : ÑÓ³Ù³¤¶È£¬µ¥Î»1 us
*    ·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
		    	 
	ticks = _ulDelayTime * (SystemCoreClock / 1000000);	 /* ÐèÒªµÄ½ÚÅÄÊý */ 		 
	tcnt = 0;
	told = DWT_CYCCNT;         /* ¸Õ½øÈëÊ±µÄ¼ÆÊýÆ÷Öµ */

	while(1)
	{
		tnow = DWT_CYCCNT;	
        
		if(tnow != told)
		{	
		    /* 32Î»¼ÆÊýÆ÷ÊÇµÝÔö¼ÆÊýÆ÷ */    
			if(tnow > told)
			{
				tcnt += tnow - told;	
			}
			/* ÖØÐÂ×°ÔØ */
			else 
			{
				tcnt += UINT32_MAX - told + tnow;	
			}	
			
			told = tnow;

			/*Ê±¼ä³¬¹ý/µÈÓÚÒªÑÓ³ÙµÄÊ±¼ä,ÔòÍË³ö */
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
/***************************** °²¸»À³µç×Ó www.armfly.com (END OF FILE) *********************************/
