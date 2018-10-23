#ifndef __OLED64_32_H
#define __OLED64_32_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define _74AC573_En_H GPIOB->BSRR = 0x200
#define _74AC573_En_L GPIOB->BRR = 0x200

void LED_Init(void);//初始化
void Lock_74AC573(u8 data);	    
#endif
