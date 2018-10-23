#include "stm32f0xx.h"
#include "PWM.h"
#include "led.h"
void TIM3_PWM_Init(uint16_t period, uint16_t prescaler)
{ 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //Ê¹ÄÜ¶¨Ê±Æ÷3Ê±ÖÓ
    TIM_TimeBaseStructure.TIM_Period        = period;// ÉèÖÃ×Ô¶¯ÖØ×°ÖÜÆÚÖµ
    TIM_TimeBaseStructure.TIM_Prescaler     = prescaler;//ÉèÖÃÔ¤·ÖÆµÖµ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;//ÉèÖÃÊ±ÖÓ·Ö¸î
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//ÏòÉÏÂ¼ÆÊı
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);    //³õÊ¼»¯¶¨Ê±Æ÷3
    
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2;// PWM2Ä£Ê½
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//±È½ÏÊä³öÊ¹ÄÜ
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low;//Êä³ö¸ß
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//Ê¹ÄÜÔ¤×°ÔØ¼Ä´æÆ÷
    TIM_Cmd(TIM3, ENABLE);                          //  Ê¹ÄÜ¶¨Ê±Æ÷3
}
