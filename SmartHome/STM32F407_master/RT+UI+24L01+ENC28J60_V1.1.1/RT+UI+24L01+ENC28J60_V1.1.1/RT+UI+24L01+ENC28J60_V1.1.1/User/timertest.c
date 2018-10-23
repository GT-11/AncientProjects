/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ��
*	�ļ����� : timertest.c
*	��    �� : V3.0
*	˵    �� : ΪFreeRTOS��������Ϣ��ȡ�ṩ֧�֣���Ҫ����һ����ʱ����
*	�޸ļ�¼ :
*		�汾��   ����        ����     ˵��
*		v1.0    2013-09-10  Eric2013  �������ļ���ST�̼���汾ΪV1.0.2
*		V2.0    2014-02-24  Eric2013  δ���޸�
*	    V3.0    2015-04-01  Eric2013  ����ע��
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "FreeRTOS.h"
#include "stm32f4xx.h"


/* ��ʱ��Ƶ��  */
#define  timerINTERRUPT_FREQUENCY		( ( unsigned portSHORT ) 20000 )
/* �ж����ȼ� */
#define  timerHIGHEST_PRIORITY			( 0 )
volatile unsigned long ulHighFrequencyTimerTicks;


/*
*********************************************************************************************************
*	�� �� ��: vSetupTimerTest
*	����˵��: ������ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vSetupTimerTest( void )
{
	unsigned long ulFrequency;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ʹ�ܶ�ʱ��ʱ�� */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

	/* ��ʼ����ʱ�� */
	TIM_DeInit( TIM2 );
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

	/* Time base configuration for timer 2 - which generates the interrupts. */
    /* ����Ҫע�� 
       HCLK = SYSCLK / 1     (AHB1Periph)     = 168MHz
       PCLK2 = HCLK / 2      (APB2Periph)      = 84MHz
       PCLK1 = HCLK / 4      (APB1Periph)      = 42MHz
    
       ��ΪAPB1 prescaler != 1, ����APB1�ϵ� TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
       ��ΪAPB2 prescaler != 1, ����APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

        APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
        APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
        
        �����Ƕ���50us��һ���ж� 
    */
	ulFrequency = configCPU_CLOCK_HZ / timerINTERRUPT_FREQUENCY / 2 - 1;	
	TIM_TimeBaseStructure.TIM_Period = ( unsigned portSHORT ) ( ulFrequency & 0xffffUL );
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( TIM2, ENABLE );

	/* ����NVIC */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timerHIGHEST_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );	
	TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

	/* ʹ�ܶ�ʱ�� */
	TIM_Cmd( TIM2, ENABLE );
}

/*
*********************************************************************************************************
*	�� �� ��: TIM2_IRQHandler
*	����˵��: TIM2�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/






