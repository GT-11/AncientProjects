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
#define Ti 99    //99   1799
//u8 addr[32]="******_Read_Address_000a_******";
u8 addr2=0x34;
u8 num[7]={0x00,0x04,0xff,0,0xfe,0,0};


u8 Remote_Data[13]={0xc3,0xc7,0x00,0x00,0x20,0x40,0x20,0x00,0x00,0x20,0x00,0x00,0x2a};
//u8 Remote_Data_OFF[13]={0xc3,0xc7,0x00,0x00,0x20,0x40,0x80,0x00,0x00,0x00,0x00,0x05,0x6f};

void Remote_Res()
{
		TIM_SetCompare3(TIM3,630);
		delay_us(9000);
		TIM_SetCompare3(TIM3,0);
		delay_us(4500);
		TIM_SetCompare3(TIM3,630);
		delay_us(600);
}

void Remote_1()
{
		TIM_SetCompare3(TIM3,0);
		delay_us(1650);
		TIM_SetCompare3(TIM3,630);
		delay_us(540);
}

void Remote_0()
{
		TIM_SetCompare3(TIM3,0);
		delay_us(540);
		TIM_SetCompare3(TIM3,630);
		delay_us(540);
}
void Remote_Send(u8 flag,u8 Data,u8 mode)
{
	u8 x=0,y,i=1,C=0;
	mode=mode<<1;
	if(flag) Remote_Data[9]=0x20;
	else Remote_Data[9]=0x00;
	Remote_Data[6]=mode;
	Remote_Data[1]=71+(Data-16)*8;
	for(x=0;x<12;x++)
	{
		C=C+Remote_Data[x];
	}
	Remote_Data[12]=C;
	Remote_Res();
	for(x=0;x<13;x++)
	{
		for(y=0;y<8;y++)
		{
			if(Remote_Data[x]&i) Remote_1();
			else Remote_0();
			i=i<<1;
		}
		i=1;
	}
		TIM_SetCompare3(TIM3,0);
		delay_ms(500);
}
 int main(void)
 {	 
	u8 value=0,mode=0,value2=0,delay=0,delay2=0,number=0;	 
	u8 tmp_buf[32];		 		    
	u8 temperature;  	    
	u8 humidity;   
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	 TIM3_PWM_Init(Ti,320);
 	NRF24L01_Init();    	//初始化NRF24L01 
	delay_ms(200);  
 	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
	{
		delay_ms(200);
	}			
//	while(DHT11_Init())	//DHT11初始化	
	{
		delay_ms(200);
	}								   
		while(1)
		{
			delay_ms(1);
			NRF24L01_RX_Mode();
			delay_ms(1);
			if(NRF24L01_RxPacket(tmp_buf)==0)
			{
				if(tmp_buf[7]==0x52&&tmp_buf[8]==0x65&&tmp_buf[23]==addr2)
				{
					delay_us(100);
					NRF24L01_TX_Mode();
					delay_us(100);			   	   
					num[3]=temperature;
					num[5]=humidity;
					num[6]=number;
					if(number==0) num[6]=1;
					NRF24L01_TxPacket(num);
				}
				if(tmp_buf[2]==0x57&&tmp_buf[3]==0x72&&tmp_buf[19]==addr2)
				{
					if((tmp_buf[26]&0x0f)==0)
					{
						delay_ms(1);
						NRF24L01_TX_Mode();
						delay_ms(1);
						num[2]=0xfa;
						num[3]=0xfc;
						NRF24L01_TxPacket(num);
						if(tmp_buf[26]&0x80)	
							Remote_Send(1,tmp_buf[27],tmp_buf[26]);
						else	
							Remote_Send(0,16,0x20);
						
					
					}
					else if((tmp_buf[26]&0x0f)==1)
					{
						mode=1;
						delay_ms(1);
						NRF24L01_TX_Mode();
						delay_ms(1);
	//					LCD_ShowString(60,90,200,16,16,tmp_buf[27]);
						value=tmp_buf[27];
						//mode=tmp_buf[27]&0x1f;
						
						num[2]=0xfa;
						num[3]=0xfc;
						NRF24L01_TxPacket(num);
					}
					else if((tmp_buf[26]&0x0f)==2)
					{
						delay_ms(1);
						NRF24L01_TX_Mode();
						delay_ms(1);
						number=tmp_buf[27];
	//					LCD_ShowString(60,90,200,16,16,tmp_buf[27]);
						//value=tmp_buf[27];
						TIM_SetCompare3(TIM3,number);	
						//mode=tmp_buf[27]&0x1f;
						
						
						num[2]=0xfa;
						num[3]=0xfc;
						num[4]=number;
						NRF24L01_TxPacket(num);
					}
					tmp_buf[26]=0xff;
				}
			}
			if(mode)
			{
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
		  }
			if(delay2>100)
			{
				delay2=0;
				DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值	
			}
			delay2++;
			
			
		};
		
		
		
		
		
	
	     
}


