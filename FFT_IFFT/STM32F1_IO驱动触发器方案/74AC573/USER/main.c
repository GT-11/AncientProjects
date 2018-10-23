#include "OLED64_32.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 


int main(void)
{	 
	int a=0,cc=0;
	u8 c=0xff;
	delay_init();	    	 //��ʱ������ʼ��	  
	//NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
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


