/*
*********************************************************************************************************
*
*	模块名称 : 定时器
*	文件名称 : timertest.c
*	版    本 : V3.0
*	说    明 : 为FreeRTOS的任务信息获取提供支持，需要创建一个定时器。
*	修改记录 :
*		版本号   日期        作者     说明
*		v1.0    2013-09-10  Eric2013  创建该文件，ST固件库版本为V1.0.2
*		V2.0    2014-02-24  Eric2013  未做修改
*	    V3.0    2015-04-01  Eric2013  整理注释
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "FreeRTOS.h"
#include "stm32f4xx.h"


/* 定时器频率  */
#define  timerINTERRUPT_FREQUENCY		( ( unsigned portSHORT ) 20000 )
/* 中断优先级 */
#define  timerHIGHEST_PRIORITY			( 0 )
volatile unsigned long ulHighFrequencyTimerTicks;


/*
*********************************************************************************************************
*	函 数 名: vSetupTimerTest
*	功能说明: 创建定时器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void vSetupTimerTest( void )
{
	unsigned long ulFrequency;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 使能定时器时钟 */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

	/* 初始化定时器 */
	TIM_DeInit( TIM2 );
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

	/* Time base configuration for timer 2 - which generates the interrupts. */
    /* 这里要注意 
       HCLK = SYSCLK / 1     (AHB1Periph)     = 168MHz
       PCLK2 = HCLK / 2      (APB2Periph)      = 84MHz
       PCLK1 = HCLK / 4      (APB1Periph)      = 42MHz
    
       因为APB1 prescaler != 1, 所以APB1上的 TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
       因为APB2 prescaler != 1, 所以APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

        APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
        APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
        
        这里是定义50us进一次中断 
    */
	ulFrequency = configCPU_CLOCK_HZ / timerINTERRUPT_FREQUENCY / 2 - 1;	
	TIM_TimeBaseStructure.TIM_Period = ( unsigned portSHORT ) ( ulFrequency & 0xffffUL );
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( TIM2, ENABLE );

	/* 设置NVIC */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timerHIGHEST_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );	
	TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

	/* 使能定时器 */
	TIM_Cmd( TIM2, ENABLE );
}

/*
*********************************************************************************************************
*	函 数 名: TIM2_IRQHandler
*	功能说明: TIM2中断服务程序。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM2_IRQHandler( void )
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		ulHighFrequencyTimerTicks++;
		TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/






