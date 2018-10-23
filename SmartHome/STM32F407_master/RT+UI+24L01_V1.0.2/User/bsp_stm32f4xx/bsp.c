/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F4XX)
*	文件名称 : bsp.c
*	版    本 : V1.1
*	说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*			   bsp = Borad surport packet 板级支持包
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-03-01  armfly   正式发布
*		V1.1    2013-06-20  armfly   规范注释，添加必要说明
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
extern u8 TX_ADDRESS[TX_ADR_WIDTH];
extern u8 TX_ADDRESS2[TX_ADR_WIDTH];
extern u8 RX_ADDRESS[RX_ADR_WIDTH];
/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
//	u8 buf[32];
	u32 x=0;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    
	bsp_InitDWT();      /* 初始化DWT */
	bsp_InitUart(); 	/* 初始化串口 */
//	bsp_InitKey();		/* 初始化按键变量（必须在 bsp_InitTimer() 之前调用） */
	bsp_InitI2C();
	NRF24L01_Init(); 
	for(x=0;x<500000;x++);  
//	bsp_InitSPIBus();	/* 配置SPI总线 */
	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
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
	
	
	
	
//	bsp_InitLed(); 		/* 初始LED指示灯端口 */
	
	LCD_InitHard();	    /* 初始化显示器硬件(配置GPIO和FSMC,给LCD发送初始化指令) */
	TOUCH_InitHard();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_DelayUS
*    功能说明: us级延迟。
*    形    参:  n : 延迟长度，单位1 us
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
		    	 
	ticks = _ulDelayTime * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */ 		 
	tcnt = 0;
	told = DWT_CYCCNT;         /* 刚进入时的计数器值 */

	while(1)
	{
		tnow = DWT_CYCCNT;	
        
		if(tnow != told)
		{	
		    /* 32位计数器是递增计数器 */    
			if(tnow > told)
			{
				tcnt += tnow - told;	
			}
			/* 重新装载 */
			else 
			{
				tcnt += UINT32_MAX - told + tnow;	
			}	
			
			told = tnow;

			/*时间超过/等于要延迟的时间,则退出 */
			if(tcnt >= ticks)break;
		}  
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
