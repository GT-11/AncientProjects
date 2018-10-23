#include "OLED64_32.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 


int main(void)
{	 
	int a=0,cc=0;
	u8 c=0xff;
	delay_init();	    	 //延时函数初始化	  
	//NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	while(1)
	{
		
			for(a=0;a<31;a++)
			{
					_74AC573_En_H;
					Lock_74AC573(0x81);
					Lock_74AC573(0x42);
					Lock_74AC573(0x24);
					Lock_74AC573(0x18);
					Lock_74AC573(0x18);
					Lock_74AC573(0x24);
					Lock_74AC573(0x42);
					Lock_74AC573(0x81);
					Lock_74AC573(0x81);
					Lock_74AC573(0x42);
					Lock_74AC573(0x24);
					Lock_74AC573(0x18);
					Lock_74AC573(0x18);
					Lock_74AC573(0x24);
					Lock_74AC573(0x42);
					Lock_74AC573(0x81);
					_74AC573_En_L;
				//delay_us(1);
			}
		
		
		
	}
}


