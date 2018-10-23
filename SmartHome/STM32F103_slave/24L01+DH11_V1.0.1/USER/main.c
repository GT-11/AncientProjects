#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24l01.h" 	 
#include "dht11.h" 
#include <string.h>
//ALIENTEK战舰STM32开发板实验32
//无线通信 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  
u8 addr[32]="******_Read_Address_0001_******";
u8 addr2[32]="**Write_Address_0001_DATA_";
 int main(void)
 {	 
	  	u16 led0pwmval=0;
	u8 dir=1;	
	 
	 
	u8 value=0,mode,value2=0,delay=0;	 
	u8 tmp_buf[32];		 		    
	u8 temperature;  	    
	u8 humidity;   
	 u8 num[6]={0x00,0x01,0xff,0,0xfe,0};
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	 TIM3_PWM_Init(99,0);
	//LCD_Init();	//初始化LCD
	//KEY_Init();	 //按键初始化	
 
 	NRF24L01_Init();    	//初始化NRF24L01 
	delay_ms(200);
	//POINT_COLOR=RED;//设置字体为红色 
//	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
//	LCD_ShowString(60,70,200,16,16,"NRF24L01 TEST");	
//	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(60,110,200,16,16,"2012/9/13");		  
 	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
	{
		//LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		//LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}			
	while(DHT11_Init())	//DHT11初始化	
	{
		//LCD_ShowString(60,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		//LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   

	
	
	
	
	
	
	
	
	
	
	
		//NRF24L01_RX_Mode();
//		mode=' ';//从空格键开始  
		while(1)
		{	  		 
			delay_ms(1);
			NRF24L01_RX_Mode();
			delay_ms(1);
			if(NRF24L01_RxPacket(tmp_buf)==0)
			{
				if(!strcmp(tmp_buf,addr))
				{
					delay_us(100);
					NRF24L01_TX_Mode();
					delay_us(100);
					DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值					   	   
					num[3]=temperature;
					num[5]=humidity;
					NRF24L01_TxPacket(num);
				}
				if(tmp_buf[2]==0x57&&tmp_buf[3]==0x72&&tmp_buf[19]==0x31)
				{
					if(tmp_buf[26]==1)
					{
						delay_ms(1);
						NRF24L01_TX_Mode();
						delay_ms(1);
	//					LCD_ShowString(60,90,200,16,16,tmp_buf[27]);
						value=tmp_buf[27];
						mode=tmp_buf[27]&0x1f;
						
						num[2]=0xfa;
						num[3]=0xfc;
						NRF24L01_TxPacket(num);
					}
				}
			}
			if(delay>10)
			{
				delay=0;
				if(value>value2) 
					value2++;
				if(value<value2) 
					value2--;
			}
			delay++;
			TIM_SetCompare3(TIM3,value2);	
			
			
			
			
			
			
		};
		
		
		
		
		
	
	     
}


