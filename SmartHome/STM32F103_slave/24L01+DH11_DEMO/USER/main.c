#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24l01.h" 	 
#include "dht11.h" 
//ALIENTEKս��STM32������ʵ��32
//����ͨ�� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  
 int main(void)
 {	 
	u8 key,mode;
	u16 t=0;			 
	u8 tmp_buf[33];		 
		u8 t1=0;			    
	u8 temperature;  	    
	u8 humidity;   
	 u8 num[4]={0xfe,0,0xff,0};
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();	//��ʼ��LCD
	KEY_Init();	 //������ʼ��	
 
 	NRF24L01_Init();    	//��ʼ��NRF24L01 

	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"NRF24L01 TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2012/9/13");		  
 	while(NRF24L01_Check())	//���NRF24L01�Ƿ���λ.	
	{
		LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}			
	while(DHT11_Init())	//DHT11��ʼ��	
	{
		LCD_ShowString(60,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,130,200,16,16,"NRF24L01 OK");
 	while(1)//�ڸò���ȷ�������ĸ�ģʽ!
	{
		key=KEY_Scan(0);
		if(key==KEY_RIGHT)
		{
			mode=0;   
			break;
		}else if(key==KEY_DOWN)
		{
			mode=1;
			break;
		}
		t++;
		if(t==100)LCD_ShowString(10,150,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //��˸��ʾ��ʾ��Ϣ
 		if(t==200)
		{	
			LCD_Fill(10,150,230,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}   
 	LCD_Fill(10,150,240,166,WHITE);//����������ʾ		  
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	if(mode==0)//RXģʽ
	{
		LCD_ShowString(60,150,200,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(60,170,200,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  
		while(1)
		{	  		    		    				 
			if(NRF24L01_Rx_CRC(tmp_buf)==0)//һ�����յ���Ϣ,����ʾ����.
			{
				tmp_buf[32]=0;//�����ַ���������
				LCD_ShowString(0,190,239,32,16,tmp_buf);    
			}else delay_us(100);	   
			t++;
			if(t==10000)//��Լ1s�Ӹı�һ��״̬
			{
				t=0;
				LED0=!LED0;
			} 				    
		};	
	}else//TXģʽ
	{							    
		LCD_ShowString(60,150,200,16,16,"NRF24L01 TX_Mode");	
		NRF24L01_TX_Mode();
		mode=' ';//�ӿո����ʼ  
		while(1)
		{	  		 



			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ					    
			LCD_ShowNum(110+40,250,temperature,2,16);		//��ʾ�¶�	   		   
			LCD_ShowNum(110+40,270,humidity,2,16);			//��ʾʪ��	 	   
			
			num[1]=temperature;
			num[3]=humidity;
			
			if(NRF24L01_Tx_CRC(num)==TX_OK)
			{
				LCD_ShowString(60,170,239,32,16,"Sended DATA:");	
				LCD_ShowString(0,190,239,32,16,num); 
//				key=mode;
//				for(t=0;t<32;t++)
//				{
//					key++;
//					if(key>('~'))key=' ';
//					tmp_buf[t]=key;	
//				}
//				mode++; 
//				if(mode>'~')mode=' ';  	  
//				tmp_buf[32]=0;//���������		   
			}else
			{										   	
 				LCD_ShowString(60,170,239,32,16,"Send Failed "); 
				LCD_Fill(0,188,240,218,WHITE);//����������ʾ			   
			};
			LED0=!LED0;
			delay_ms(1500);	




							  
			
		
		};
		
		
		
		
		
	
	}     
}


