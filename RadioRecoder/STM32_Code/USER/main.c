#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "malloc.h"  
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "recorder.h"
#include "adc.h"
#include "24l01.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "RecordingGyroAccelerometerData.h"
#include "string.h"
#include "stdlib.h"
#include "key.h"
#include "oled.h"
#include "stmflash.h"
#include "rng.h"
#include <math.h>

#define ADDRESS "ZZAABBBB"
#define FLASH_SAVE_ADDR 0X08020004
//#define Test_Define 

#define Blink  0x01
#define Begin 0x02
#define End 0x04
#define Init 0x08

unsigned __int64 Windows_time;
unsigned __int64 Windows_time_ref;

unsigned char Real_Time_Year=0,Real_Time_Month=0,Real_Time_Day=0,Real_Time_Hour=0,Real_Time_Minute=0,Real_Time_Second=0;
unsigned int Real_Time_Millise=0;


unsigned char Real_Time_Year_STOP=0,Real_Time_Month_STOP=0,Real_Time_Day_STOP=0,Real_Time_Hour_STOP=0,Real_Time_Minute_STOP=0,Real_Time_Second_STOP=0;
unsigned int Real_Time_Millise_STOP=0;


typedef struct FLASH_SAVE
{
	unsigned char WIFI_NAME[20];
	unsigned char WIFI_PASS[20];
	unsigned char SERVER_IP[20];
	unsigned char SERVER_PORT[20];
	unsigned char BIND_NAME[20];
	unsigned char DEVICE_ID[28];
}FLASH_SAVE;


FLASH_SAVE FLASH_DATA;

int RecvComLoc1 = 0;
int RecvComLoc2 = 0;
int RecvComLoc3 = 0;
int RecvComLoc4 = 0;

u8 String_Windows_Time[64] = {0};
u8 Send24L01Buffer[32] = {"2S&0000000011111111AB33333333&E "};
u8 Recv24L01Buffer[128];
u8 Recv24L01Loc = 0;

u8 initfilename[20] = {0};
char SessID[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char SoftID[9] = {"12345678"};
char BordID[9] = {ADDRESS};
char *recv_A = "WIFI CONNECTED\r\nWIFI GOT IP\r\n\r\nOK\r\n";
char *recv_B = "CONNECT";

char *recvcom1 = "SORID:";
char *recvcom2 = "TARID:";
char *recvcom3 = "#End#";


char recv_wifi[20] = {0};
char recv_pass[20] = {0};
char recv_ip[20] = {0};
char recv_port[20] = {0};

unsigned char Binding_Bool = 1;

extern u8 *i2srecbuf1;
extern u8 *i2srecbuf2;
extern u8 AM_Factor;
void Clock_Config(void){
     uint32_t           PLL_M;      
     uint32_t           PLL_N;
     uint32_t           PLL_P;
     uint32_t           PLL_Q;
     RCC_DeInit();
     RCC_HSEConfig(RCC_HSE_ON);
     while( RCC_WaitForHSEStartUp() != SUCCESS );
     PLL_M         =    8;  
     PLL_N        =    400;
     PLL_P         =    4;
     PLL_Q         =    9;
     RCC_PLLConfig(RCC_PLLSource_HSE, PLL_M, PLL_N, PLL_P, PLL_Q);
     RCC_PLLCmd(ENABLE);
     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
     RCC_HCLKConfig(RCC_HCLK_Div1);
     RCC_PCLK1Config(RCC_HCLK_Div2);
     RCC_PCLK2Config(RCC_HCLK_Div1);
}
int StrEqual(char *target,char *str,int targetlen, int strlen)
{
	char *p = target;
	int i = 0;
	int j = 0;
	for(i = 0;i<targetlen;i++)
	{
		if(p[i] == str[j])
			j++;
		else
			j=0;
		if(j==strlen)
			return i;
	}
	return -1;
}

void ClearBuffer(char *target,int targetlen)
{
	int i = 0;
	for(i = 0;i<targetlen;i++)
	target[i] = 0;
}

//u8 Recv24L01Data(u8 *buff,u8 bufflen,u8 *com)
//{
//	int RecvLoc = 0;
//	int i = 0;
//	u8 sum = 0;
//	if(NRF24L01_RxPacket(buff)!=0)
//			return 0;
//	RecvLoc = StrEqual(buff,com,bufflen,strlen(com));
//	if(RecvLoc != -1)
//	{
//		sum = 0;
//		for(i = RecvLoc - 11;i < RecvLoc - 11 + 24;i++)
//			sum+=buff[i];
//		if(sum == buff[RecvLoc -11 + 24])
//		{
//			for(i = 0;i<12;i++)
//			{
//				SoftID[i] = buff[RecvLoc + 1 + i];
//			}
//				
//			break;
//		}
//	}
//}

u8 Send24L01Data(u8*mode,u8 *data)
{
	int i = 0;
	u8 sum = 0;
	delay_ms(10);
	NRF24L01_TX_Mode();
	delay_ms(10);
	for(i=0;i<8;i++)
		Send24L01Buffer[i+2 + 1] = SoftID[i];
	for(i=0;i<8;i++)
		Send24L01Buffer[i+10+1] = BordID[i];
				
	Send24L01Buffer[18+1] = mode[0];
	Send24L01Buffer[19+1] = mode[1];
	
	for(i=0;i<8;i++)
		Send24L01Buffer[i+20+1] = data[i];
	
	for(i=0;i<30;i++)
		sum +=Send24L01Buffer[i];
	Send24L01Buffer[30 + 1] = sum;
	Send24L01Buffer[0] = 31;
	i = NRF24L01_TxPacket(Send24L01Buffer);
	delay_ms(10);
	NRF24L01_RX_Mode();
	delay_ms(10);
	return i;
}

void Timer_Config()
{
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		 NVIC_InitTypeDef NVIC_InitStructure;
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
 TIM_DeInit(TIM2);
   /* ?????? */       //84000000/84=1000000,???????1M      
 TIM_TimeBaseStructure.TIM_Prescaler= (100 - 1);
 /* ??????????????(???) */ /* ?? TIM_Period?????????????? */
 TIM_TimeBaseStructure.TIM_Period =100000-1 ;  
/* ???? */
 TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
/* ?????? */ 
 TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
 //??????? 
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 /* ???????? */ 
 TIM_ClearFlag(TIM2, TIM_FLAG_Update);  
/*????TIM_FLAG_Update??*/        
 TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
/* ???? */
 TIM_Cmd(TIM2, ENABLE); 
	
	

 /* Enable TIM2 interrupt IRQ channel */ 
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //??????
	{
		Real_Time_Millise+=100;
		if(Real_Time_Millise >= 1000)
		{
			Real_Time_Millise-=1000;
			Real_Time_Second++;
			if(Real_Time_Second >= 60)
			{
				Real_Time_Second = 0;
				Real_Time_Minute++;
				if(Real_Time_Minute>=60)
				{
					Real_Time_Minute = 0;
					Real_Time_Hour++;
					if(Real_Time_Hour >= 24)
					{
						Real_Time_Hour = 0;
						Real_Time_Day++;
					}
				}
			}
		}

	}
	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update );        //??????
}


int main(void)
{
	FIL initfile;
	DIR recdir;	 					//目录  
	u8 state = 0;
	u8 connectstate = 0;
	u8 *str1;
	u32 FS = 0,GADFS = 0;
	u32 timecnt = 0;
	u8 WavFlag = 0xff;
	u8 GadFlag = 0xff;
	u8 Buffer[64];
	u8 i,sum = 0;
	u8 presskeyvalue = 1;
	int random_i;
	unsigned int random_srand = 0;
	
//	uart_init(115200);		//初始化串口波特率为115200
//	delay_ms(5000);
//	printf("AT+UART=921600,8,1,0,0\r\n");
//	while(1);
	
	
	Clock_Config();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	Timer_Config();
	delay_init(100);  //初始化延时函数
	
	
	
	
	
	
	
	
	
	
	
	i = sizeof(FLASH_SAVE);
	STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)&FLASH_DATA,sizeof(FLASH_SAVE) / 4);
	
	if(FLASH_DATA.DEVICE_ID[0] == 0xff)
	{
		OLED_Init( );	 
		OLED_Clear( );
		OLED_ShowString(0,0,"Initialization..",16);
		OLED_ShowString(0,2,"Do Not Turn off",16);
		Random_Adc_Init();
		for (random_i = 0; random_i < 1024;random_i++)
			random_srand += Get_Adc(ADC_Channel_1);
		srand(random_srand);
		FLASH_DATA.DEVICE_ID[0] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[1] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[2] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[3] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[4] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[5] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[6] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[7] = (rand() % 26) + 'A';
		FLASH_DATA.DEVICE_ID[8] = 0;
		
		
		delay_ms(200);
		STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)&FLASH_DATA,sizeof(FLASH_SAVE) / 4);
		
		
		uart_init(115200/6.25);		//初始化串口波特率为115200
		delay_ms(2000);	
		printf("AT+UART=921600,8,1,0,0\r\n");
		delay_ms(2000);	
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		OLED_Clear( );
		OLED_ShowString(0,0,"Please turn off",16);
		while(1);
	}
	
	
	
	
	
	
	
	
	
	
	
	#ifndef Test_Define
	uart_init(921600/6.25);		//初始化串口波特率为115200
	#else
	uart_init(115200/6.25);		//初始化串口波特率为115200
	#endif
	LED_Init();					//初始化LED 
	LED0 = 1;
	LED1 = 1;
	delay_ms(70);	
	KEY_Init();
	OLED_Init( );	 
	OLED_Clear( );
	
	#ifdef Test_Define
	delay_ms(5000);
	printf("AT+UART=921600,8,1,0,0\r\n");
	#endif
	
	OLED_ShowString(0,0,"Check NRF24L01..",16);
	
	
	NRF24L01_Init();
	
	
	for (i = 0; i < 8; i++)
			BordID[i] = FLASH_DATA.DEVICE_ID[i];
	
	while(NRF24L01_Check())
	{
		LED1=!LED1;//DS0闪烁  
		delay_ms(200);
	}
	
	delay_ms(100);
	NRF24L01_RX_Mode();
	delay_ms(100);

//while(1)
//{
//	Send24L01Data("12","12345678");
//	//Send24L01Buffer[0] = 31;
//	//NRF24L01_TxPacket(Send24L01Buffer);
//	delay_ms(2000);
//}

	OLED_ShowString(0,2,"Check MPU6050..",16);
	MPU_Init();
	delay_ms(200);
	while(mpu_dmp_init())
	{
		LED1 = !LED1;
 		delay_ms(200);
	}
	
//		while(1)
//	{
//		MPU_Get_Gyroscope(&a,&b,&c);
//		MPU_Get_Accelerometer(&d,&e,&f);
//		//mpu_dmp_get_data(&a,&b,&c,&d,&e,&f,&g,&h,&i);
//		delay_ms(200);
//	}
	
//	while(1)
//	{
//		mpu_dmp_get_data(&a,&b,&c,&d,&e,&f,&g,&h,&i);
//		delay_ms(200);
//	}
	
	
	OLED_ShowString(0,4,"Check SDCard..",16);
	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
	
  if(f_mount(fs[0],"0:",1)) 		//挂载SD卡  
	{

	}
	
//	if(f_open(&initfile,(const TCHAR*)"0:/RECORDER/Init.ini", FA_READ) != FR_NO_FILE)
//	{
//		f_read(&initfile,initfilename,20,&br);
//		f_close(&initfile);
//	}
//	else
//	{
//		while(1)
//		{
//			LED1 = !LED1;
//			delay_ms(100);
//		}
//	}
	
	
	while(f_opendir(&recdir,"0:/RECORDER"))//打开录音文件夹
	{	 			  
		LED1 = !LED1;
		delay_ms(200);				  
		f_mkdir("0:/RECORDER");				//创建该目录   
	}
	
	OLED_ShowString(0,6,"Check Over!",16);
	delay_ms(2000);
	OLED_Clear( );
	delay_ms(500);
	OLED_ShowString(0,0,"Waiting Network",16);

	//////////////////////////////////////////////////
	//binding
	binding:
	presskeyvalue = 1;
	LED1 = 0;
	LED0 = 0;
	while(1)
	{
		LED0 = !LED0;
		LED1 = !LED1;
		delay_ms(50);
		//NRF24L01_RX_Mode();
		delay_ms(50);
		if(presskeyvalue == 1 || SessID[0] == 0)
			presskeyvalue = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);

		if(SessID[0] != 0)
			timecnt++;
		if(timecnt > 20)
		{
			timecnt=0;
			OLED_Clear( );
			delay_ms(10);
			OLED_ShowString(0,0,"Waiting Binding",16);
		}
		
		if(NRF24L01_RxPacket(Buffer)!=0)
			continue;
		RecvComLoc3 = StrEqual(Buffer,"SERCH:Begin_",32,strlen("SERCH:Begin_"));
		if(connectstate < 2 && RecvComLoc3 != -1)
		{
			sum = 0;
			for(i = RecvComLoc3 - 11;i < RecvComLoc3 - 11 + 20;i++)
				sum+=Buffer[i];
			if(sum == Buffer[RecvComLoc3 -11 + 20])
			{
				for(i = 0;i<8;i++)
				{
					SoftID[i] = Buffer[RecvComLoc3 + 1 + i];
				}
				
				
				if(connectstate == 1)
				{
					//sum = Send24L01Data("AB","00000000");
					continue;
				}
				OLED_ShowString(0,2,"Request from:",16);
				OLED_ShowString(0,4,(u8*)SoftID,16);
				OLED_ShowString(0,6,"Connect?",16);
				if(PressKey() == 0)
				{
					//sum = Send24L01Data("AB","00000000");
					connectstate = 1;
					
					OLED_Clear( );
					delay_ms(10);
					OLED_ShowString(0,0,"Connection",16);
					OLED_ShowString(0,2,"Succeeded!",16);
					OLED_ShowString(0,4,(u8*)SoftID,16);
					delay_ms(2000);
					OLED_Clear( );
					delay_ms(10);
					OLED_ShowString(0,0,"Waiting WIFI",16);
					break;
				}
				else
				{
					sum = 0;
					
					for(i = 0;i<8;i++)
						SoftID[i] = 0;

					for(i = 0;i<64;i++)
						Buffer[i]=0;
					
					OLED_Clear( );
					delay_ms(10);
					OLED_ShowString(0,0,"Waiting Network",16);
				}
				continue;
				
				
			}
			
		}
		
		

//		RecvComLoc3 = StrEqual(Buffer,"Sess_",32,strlen("Sess_"));
//		if(RecvComLoc3 != -1)
//		{
//			RecvComLoc2 = StrEqual(Buffer,"#",32,strlen("#"));
//			if(RecvComLoc2 < RecvComLoc3)
//				continue;
//			
//			sum = 0;
//			for(i = RecvComLoc3 - 4;i < RecvComLoc2 + 1;i++)
//				sum+=Buffer[i];
//			if(sum == Buffer[RecvComLoc2 + 1])
//			{
//				sum = 0;
//				for(i = 0;i < 8;i++)
//				{
//					if(SoftID[i] != Buffer[RecvComLoc3 + 1 + i])
//						sum = 1;
//				}
//				if(sum == 1)
//					continue;
//				for(i = 0;i< RecvComLoc2 - RecvComLoc3 - 9;i++)
//				{
//					SessID[i] = Buffer[RecvComLoc3 + 9 + i];
//				}
//				connectstate = 2;

//			}
//		}
		
		

		
		
		
		
		

		
	}
	
	
	
	
	if(FLASH_DATA.WIFI_NAME[0] != 0xff)
	{
			OLED_Clear( );
			delay_ms(10);
			OLED_ShowString(0,0,"WiFi Server:",16);
			OLED_ShowString(0,2,(u8*)FLASH_DATA.WIFI_NAME,16);
			OLED_ShowString(0,4,(u8*)FLASH_DATA.SERVER_IP,16);
			OLED_ShowString(0,6,"Connect?",16);
			if(PressKey() == 0)
			{
				for(i = 0; i< 20;i++)
					recv_wifi[i] = FLASH_DATA.WIFI_NAME[i];
				for(i = 0; i< 20;i++)
					recv_pass[i] = FLASH_DATA.WIFI_PASS[i];
				for(i = 0; i< 20;i++)
					recv_ip[i] = FLASH_DATA.SERVER_IP[i];
				for(i = 0; i< 20;i++)
					recv_port[i] = FLASH_DATA.SERVER_PORT[i];
				goto connect_wifi;
			}
			else
			{
				OLED_Clear( );
				delay_ms(10);
				OLED_ShowString(0,0,"Waiting WIFI",16);
			}
	}
	
	
	
	
	
	
	
	
	
	
	//////////////////////////////////////////////////
	
//	while(1)
//	{
//		LED0 = !LED0;
//		LED1 = !LED1;
//		NRF24L01_RX_Mode();
//		delay_ms(40);
//	}
	
	//////////////////////////////////////////////////
	//wifi
	LED1 = 1;
	LED0 = 0;
	while(1)
	{
		LED0 = !LED0;
		LED1 = !LED1;
		delay_ms(25);
		NRF24L01_RX_Mode();
		delay_ms(25);

		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
			goto binding;
		
		if(NRF24L01_RxPacket(Buffer)!=0)
			continue;
		RecvComLoc1 = StrEqual(Buffer,"$WIFI",32,strlen("$WIFI"));
		RecvComLoc2 = StrEqual(Buffer,"$PASS",32,strlen("$PASS"));
		RecvComLoc3 = StrEqual(Buffer,"$IPAD",32,strlen("$IPAD"));
		RecvComLoc4 = StrEqual(Buffer,"$PORT",32,strlen("$PORT"));
		if(RecvComLoc1 != -1)
		{
			RecvComLoc2 = StrEqual(Buffer,"\n",32,strlen("\n"));
			if(RecvComLoc2 < RecvComLoc1)
				continue;
			
			sum = 0;
			for(i = RecvComLoc1 - 12;i < RecvComLoc2 + 1;i++)
				sum+=Buffer[i];
			if(sum == Buffer[RecvComLoc2 + 1])
			{
				for(i = 0;i<RecvComLoc2 - RecvComLoc1 - 1;i++)
				{
					recv_wifi[i] = Buffer[i + RecvComLoc1 + 1];
					FLASH_DATA.WIFI_NAME[i] = recv_wifi[i];
				}
				FLASH_DATA.WIFI_NAME[i] = 0;
			}
		}
		else if (RecvComLoc2 != -1)
		{
			RecvComLoc1 = StrEqual(Buffer,"\n",32,strlen("\n"));
			if(RecvComLoc1 < RecvComLoc2)
				continue;
			
			sum = 0;
			for(i = RecvComLoc2 - 12;i < RecvComLoc1 + 1;i++)
				sum+=Buffer[i];
			if(sum == Buffer[RecvComLoc1 + 1])
			{
				for(i = 0;i<RecvComLoc1 - RecvComLoc2 - 1;i++)
				{
					recv_pass[i] = Buffer[i + RecvComLoc2 + 1];
					FLASH_DATA.WIFI_PASS[i] = recv_pass[i];
				}
				FLASH_DATA.WIFI_PASS[i] = 0;
			}
		}
		else if (RecvComLoc3 != -1)
		{
			RecvComLoc2 = StrEqual(Buffer,"\n",32,strlen("\n"));
			if(RecvComLoc2 < RecvComLoc3)
				continue;
			
			sum = 0;
			for(i = RecvComLoc3 - 12;i < RecvComLoc2 + 1;i++)
				sum+=Buffer[i];
			if(sum == Buffer[RecvComLoc2 + 1])
			{
				for(i = 0;i<RecvComLoc2 - RecvComLoc3 - 1;i++)
				{
					recv_ip[i] = Buffer[i + RecvComLoc3 + 1];
					FLASH_DATA.SERVER_IP[i] = recv_ip[i];
				}
				FLASH_DATA.SERVER_IP[i] = 0;
			}
		}
		else if (RecvComLoc4 != -1)
		{
			RecvComLoc2 = StrEqual(Buffer,"\n",32,strlen("\n"));
			if(RecvComLoc2 < RecvComLoc4)
				continue;
			
			sum = 0;
			for(i = RecvComLoc4 - 12;i < RecvComLoc2 + 1;i++)
				sum+=Buffer[i];
			if(sum == Buffer[RecvComLoc2 + 1])
			{
				for(i = 0;i<RecvComLoc2 - RecvComLoc4 - 1;i++)
				{
					recv_port[i] = Buffer[i + RecvComLoc4 + 1];
					FLASH_DATA.SERVER_PORT[i] = recv_port[i];
				}
				FLASH_DATA.SERVER_PORT[i] = 0;
			}
		}
		else
		{
			
		}
		if(recv_wifi[0] != 0 && recv_pass[0] != 0 && recv_ip[0] != 0 && recv_port[0] != 0)
		{
			break;
		}
	}
	
	
	STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)&FLASH_DATA,sizeof(struct FLASH_SAVE));
	/////////////////////////////////////////////////
	
	
	
	//////////////////////////////////////////////////
	connect_wifi:
	
	OLED_Clear( );
	delay_ms(10);
	OLED_ShowString(0,0,"Connect WIFI",16);
	OLED_ShowString(0,2,"...",16);
	
	///////////////////////////////////////////////////
	//ESP8266 Init
	printf("AT+CWMODE=1\r\n");
	delay_ms(1000);
	
	do
	{
		//printf("AT+CWJAP=\"CU_kh2m\",\"m9svhdhq\"\r\n");
		printf("AT+CWJAP=\"%s\",\"%s\"\r\n",recv_wifi,recv_pass);
		ClearBuffer(USART_RX_BUF,USART_REC_LEN);
		LED0 = 1;
		delay_ms(5000);
		LED0 = 0;
		delay_ms(5000);
		printf("%s",USART_RX_BUF);
	}while(StrEqual(USART_RX_BUF,recv_A,USART_REC_LEN,strlen(recv_A)) == -1);
	
	ClearBuffer(USART_RX_BUF,USART_REC_LEN);
	
	
	
	OLED_Clear( );
	delay_ms(10);
	OLED_ShowString(0,0,"Connect Server",16);
	OLED_ShowString(0,2,"...",16);
	
	
	do
	{
		printf("AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",recv_ip,recv_port);
		ClearBuffer(USART_RX_BUF,USART_REC_LEN);
		LED1 = 1;
		delay_ms(2500);
		LED1 = 0;
		delay_ms(2500);
	}while(StrEqual(USART_RX_BUF,recv_B,USART_REC_LEN,strlen(recv_B)) == -1);
	delay_ms(200);
	printf("AT+CIPMODE=1\r\n");
	delay_ms(200);
	printf("AT+CIPSEND\r\n");
	delay_ms(200);
	//printf("###Recorder Bord ID = %s###END###",initfilename);
	//////////////////////////////////////////////////////
	
	
	
	
	
	
	
	
	if(FLASH_DATA.BIND_NAME[0] != 0xff)
	{
			OLED_Clear( );
			delay_ms(10);
			OLED_ShowString(0,0,"Bind Name:",16);
			OLED_ShowString(0,2,(u8*)FLASH_DATA.BIND_NAME,16);
			//OLED_ShowString(0,4,(u8*)FLASH_DATA.SERVER_IP,16);
			OLED_ShowString(0,6,"Bind?",16);
			if(PressKey() == 0)
			{
				for(i = 0;i<8;i++)
					initfilename[i] = BordID[i];
				for(i = 0;i< 12;i++)
					initfilename[i + 8] = FLASH_DATA.BIND_NAME[i];
				
				
				
				
				while(1)
				{
					LED0 = !LED0;
					LED1 = !LED1;
					delay_ms(25);

					delay_ms(25);
				
					RecvComLoc3 = StrEqual(USART_RX_BUF,"Bindind Check Confirm\r\n",USART_REC_LEN,strlen("Bindind Check Confirm\r\n"));
					if(RecvComLoc3 != -1)
					{
						OLED_Clear( );
						delay_ms(10);
						OLED_ShowString(0,0,"Binding",16);
						OLED_ShowString(0,2,"Succeeded!",16);
						delay_ms(2000);
						break;
					}
					
					
					RecvComLoc2 = strlen(FLASH_DATA.BIND_NAME);
					
					
					
					delay_ms(50);
					
					printf("Do Bindind Cheking\r\n");
					RecvComLoc3 = 0;
					UARTSendData(&((u8*)&RecvComLoc2)[3],1);
					UARTSendData(&((u8*)&RecvComLoc2)[2],1);
					UARTSendData(&((u8*)&RecvComLoc2)[1],1);
					UARTSendData(&((u8*)&RecvComLoc2)[0],1);
					for(i = 0;i<strlen("Do Bindind Cheking\r\n");i++)
						RecvComLoc3+="Do Bindind Cheking\r\n"[i];
					for(i = 0;i<8;i++)
						RecvComLoc3+=BordID[i];
					for(i = 0;i< RecvComLoc2;i++)
						RecvComLoc3+=initfilename[i + 8];
					UARTSendData(&((u8*)&RecvComLoc3)[3],1);
					UARTSendData(&((u8*)&RecvComLoc3)[2],1);
					UARTSendData(&((u8*)&RecvComLoc3)[1],1);
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
					UARTSendData(BordID,8);
					UARTSendData(&initfilename[8],RecvComLoc2);
				}
				
				
				
				
				
				
				goto main_start;
			}
			else
			{

			}
	}
	
	
	
	
	
	
	
	
	
	
	
	OLED_Clear( );
	delay_ms(10);
	OLED_ShowString(0,0,"Waiting Binding",16);
	while(1)
	{
		delay_ms(200);
		presskeyvalue = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
		//Synchronous Device Info\r\n
		RecvComLoc3 = StrEqual(USART_RX_BUF,"Synchronous Device Info\r\n",USART_REC_LEN,strlen("Synchronous Device Info\r\n"));
		if(RecvComLoc3 != -1)
		{
			printf("Synchronous Device Info\r\n");
			RecvComLoc3 = 0;
			UARTSendData((u8*)&RecvComLoc3,4);
			for(i = 0;i<strlen("Synchronous Device Info\r\n");i++)
				RecvComLoc3+="Synchronous Device Info\r\n"[i];
			for(i = 0;i<8;i++)
				RecvComLoc3+=BordID[i];
			
			
			UARTSendData(&((u8*)&RecvComLoc3)[3],1);
			UARTSendData(&((u8*)&RecvComLoc3)[2],1);
			UARTSendData(&((u8*)&RecvComLoc3)[1],1);
			UARTSendData(&((u8*)&RecvComLoc3)[0],1);
			UARTSendData(BordID,8);
			ClearBuffer(USART_RX_BUF,USART_REC_LEN);
			continue;
		}
		RecvComLoc3 = StrEqual(USART_RX_BUF,"Request Binding\r\n",USART_REC_LEN,strlen("Request Binding\r\n"));
		if(RecvComLoc3 != -1)
		{
			
			RecvComLoc2 = USART_RX_BUF[RecvComLoc3 + 1] << 24;
			RecvComLoc2 |= USART_RX_BUF[RecvComLoc3 + 2] << 16;
			RecvComLoc2 |= USART_RX_BUF[RecvComLoc3 + 3] << 8;
			RecvComLoc2 |= USART_RX_BUF[RecvComLoc3 + 4];
			
			RecvComLoc1 = USART_RX_BUF[RecvComLoc3 + 5] << 24;
			RecvComLoc1 |= USART_RX_BUF[RecvComLoc3 + 6] << 16;
			RecvComLoc1 |= USART_RX_BUF[RecvComLoc3 + 7] << 8;
			RecvComLoc1 |= USART_RX_BUF[RecvComLoc3 + 8];
			
			
			RecvComLoc4 = 0;
			for(i = 0; i < (u8)RecvComLoc2 + 8; i++)
				RecvComLoc4 += USART_RX_BUF[RecvComLoc3 + 9 + i];
			for(i = 0; i < strlen("Request Binding\r\n"); i++)
				RecvComLoc4 += "Request Binding\r\n"[i];
			
			
			
			if(RecvComLoc4 == RecvComLoc1)
			{
				for(i = 0;i<8;i++)
				{
					initfilename[i] = BordID[i];
				}
				for(i = 0;i< RecvComLoc2;i++)
				{
					initfilename[i + 8] = USART_RX_BUF[RecvComLoc3 + 17 + i];
					FLASH_DATA.BIND_NAME[i] = initfilename[i + 8];
				}
				FLASH_DATA.BIND_NAME[i] = 0;
				timecnt = 0;
				OLED_ShowString(0,4,"                ",16);
				OLED_ShowString(0,2,"Request from:",16);
				OLED_ShowString(0,4,(u8*)&initfilename[8],16);
				OLED_ShowString(0,6,"Bind?",16);
				ClearBuffer(USART_RX_BUF,USART_REC_LEN);
				if(presskeyvalue == 1)
				{
					Binding_Bool = 1;
					continue;
				}
				
				
				OLED_Clear( );
				delay_ms(10);
				OLED_ShowString(0,0,"Binding...",16);
				OLED_ShowString(0,2,(u8*)&initfilename[8],16);
				
				
				
				while(1)
				{
					LED0 = !LED0;
					LED1 = !LED1;
					delay_ms(25);
					//NRF24L01_RX_Mode();
					delay_ms(25);
					
					
					//if(NRF24L01_RxPacket(Buffer)!=0)
					//	continue;
					
					

					
					
					
					RecvComLoc3 = StrEqual(USART_RX_BUF,"Bindind Check Confirm\r\n",USART_REC_LEN,strlen("Bindind Check Confirm\r\n"));
					if(RecvComLoc3 != -1)
					{
						OLED_Clear( );
						delay_ms(10);
						OLED_ShowString(0,0,"Binding",16);
						OLED_ShowString(0,2,"Succeeded!",16);
						delay_ms(2000);
						break;
					}
					
					
					
					
					
					delay_ms(50);
					//Send24L01Data("CK","00000000");
					printf("Do Bindind Cheking\r\n");
					RecvComLoc3 = 0;
					UARTSendData(&((u8*)&RecvComLoc2)[3],1);
					UARTSendData(&((u8*)&RecvComLoc2)[2],1);
					UARTSendData(&((u8*)&RecvComLoc2)[1],1);
					UARTSendData(&((u8*)&RecvComLoc2)[0],1);
					for(i = 0;i<strlen("Do Bindind Cheking\r\n");i++)
						RecvComLoc3+="Do Bindind Cheking\r\n"[i];
					for(i = 0;i<8;i++)
						RecvComLoc3+=BordID[i];
					for(i = 0;i< RecvComLoc2;i++)
						RecvComLoc3+=initfilename[i + 8];
					UARTSendData(&((u8*)&RecvComLoc3)[3],1);
					UARTSendData(&((u8*)&RecvComLoc3)[2],1);
					UARTSendData(&((u8*)&RecvComLoc3)[1],1);
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
					UARTSendData(BordID,8);
					UARTSendData(&initfilename[8],RecvComLoc2);
				}
				break;
			}
			//ClearBuffer(USART_RX_BUF,USART_REC_LEN);
		}
		else
		{
			if(Binding_Bool == 1)
			{
				Binding_Bool = 0;
				OLED_Clear( );
				delay_ms(10);
				OLED_ShowString(0,0,"Waiting Binding",16);
			}
					printf("Device is Idle\r\n");
					RecvComLoc3 = 1;
					UARTSendData(&((u8*)&RecvComLoc3)[3],1);
					UARTSendData(&((u8*)&RecvComLoc3)[2],1);
					UARTSendData(&((u8*)&RecvComLoc3)[1],1);
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
					RecvComLoc3 = 0;
					for(i = 0;i<strlen("Device is Idle\r\n");i++)
						RecvComLoc3+="Device is Idle\r\n"[i];
					for(i = 0;i<8;i++)
						RecvComLoc3+=BordID[i];
					RecvComLoc3+=1;
					UARTSendData(&((u8*)&RecvComLoc3)[3],1);
					UARTSendData(&((u8*)&RecvComLoc3)[2],1);
					UARTSendData(&((u8*)&RecvComLoc3)[1],1);
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
					UARTSendData(BordID,8);
					RecvComLoc3 = 2;
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
		}
	}
	STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)&FLASH_DATA,sizeof(struct FLASH_SAVE));
	main_start:
	
	ClearBuffer(USART_RX_BUF,USART_REC_LEN);
	delay_ms(200);
	
	
	//i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存1申请
	//i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存2申请  

	//recoder_enter_rec_mode();	//开启DMA，ADC传输
	
	OLED_Clear( );
	delay_ms(10);
	OLED_ShowString(0,0,"Wait Recording",16);
	OLED_ShowString(0,2,"SoftID:",16);
	OLED_ShowString(56,2,SoftID,16);
	OLED_ShowString(0,4,"HardID:",16);
	OLED_ShowString(56,4,BordID,16);
	
	
	LED1 = 1;
	LED0 = 0;
	while(1)
	{
//		if(StrEqual(USART_RX_BUF,"Get Bord ID::",USART_REC_LEN,strlen("Get Bord ID::")) != -1)
//		{
//			printf("###Recorder Bord ID = %s%s###END###",SessID,initfilename);
//			ClearBuffer(USART_RX_BUF,USART_REC_LEN);
//		}
		//printf("Test!!!!!!");
		if(NRF24L01_RxPacket(Buffer)==0)
		{
			str1 = (u8*)strstr((char *)Buffer,(char *)"CALIB");
			if(str1)
			{
				sum = 0;
				for(i = 0;i<21;i++)
				{
					sum += *(str1 + i);
				}
				if(sum == *(str1 + 21))
				{
					sum = 0;
					for(i = 0;i<8;i++)
						if(*(str1 + i + 5) != SoftID[i])
							sum = 1;
					if(sum == 0)
					{
						LED0=!LED0;
						LED1=!LED1;
						Real_Time_Year = *(str1 + 13);
						Real_Time_Month = *(str1 + 14);
						Real_Time_Day = *(str1 + 15);
						Real_Time_Hour = *(str1 + 16);
						Real_Time_Minute = *(str1 + 17);
						Real_Time_Second = *(str1 + 18);
						Real_Time_Millise = 0;
						Real_Time_Millise = *(str1 + 19) << 8;
						Real_Time_Millise |= *(str1 + 20);
					}
				}
			}
			
			
			str1 = (u8*)strstr((char *)Buffer,(char *)"BEGIN:");
			if(str1)
			{
				sum = 0;
				for(i = 0;i<8;i++)
					if(*(str1 + i - 8) != SoftID[i])
						sum = 1;
			}
			if(str1 && sum == 0)
			{
				if(*(str1 + 6)=='B' && *(str1 + 7)=='l' && *(str1 + 8)=='i' && *(str1 + 9)=='n' && *(str1 + 10)=='k')
				{
					state = (~state & Blink) | (state & ~Blink);
					LED1 = 1;
					LED0 = 0;
				}
				if(*(str1 + 6)=='I' && *(str1 + 7)=='n' && *(str1 + 8)=='i' && *(str1 + 9)=='t')
				{
					FS = atoi((char *)(str1 + 10));
					//printf("Begin Create Wave File!File Name=%s%s&&&End",SessID,initfilename);
					if(*(str1 + 15) == '-')
					{
						GADFS = atoi((char *)(str1 + 16));
						if((GADFS / 100) == 0)
						{
							AM_Factor = atoi((char *)(str1 + 18));
						}
					}
					else if(*(str1 + 14) == '-')
					{
						GADFS = atoi((char *)(str1 + 15));
						if((GADFS / 100) == 0)
						{
							AM_Factor = atoi((char *)(str1 + 17));
						}
					}
					else
						continue;
					
					
					
					
					if(FS > 36000 || FS < 8000 || GADFS == 0)
						continue;
					if(GadFlag == 0xff && WavFlag == 0xff)
					{
						state &= ~Blink;
						state |= Init;
						LED1 = 1;
						LED0 = 1;
						
						OLED_Clear( );
						delay_ms(10);
						OLED_ShowString(0,0,"Recording...",16);
						OLED_ShowString(0,2,"SoftID:",16);
						OLED_ShowString(56,2,SoftID,16);
						OLED_ShowString(0,4,"HardID:",16);
						OLED_ShowString(56,4,BordID,16);

					}
				}
				if(*(str1 + 6)=='B' && *(str1 + 7)=='e' && *(str1 + 8)=='g' && *(str1 + 9)=='i' && *(str1 + 10)=='n')
				{
					if(GadFlag == 0xab && WavFlag == 0xab)
					{
						state &= ~Blink;
						state |= Begin;
						LED1 = 1;
						LED0 = 0;
					}
				}
				if(*(str1 + 6)=='E' && *(str1 + 7)=='n' && *(str1 + 8)=='d')
				{
					if(GadFlag == 0x80 && WavFlag == 0x80)
					{
						state &= ~Blink;
						state |= End;
						LED1 = 1;
						LED0 = 0;
					}
				}
				*str1 = 0x30;
			}
		}
		
		if(GadFlag==0x80 && WavFlag==0x80)
		{
			gad_recorder(3,0,0);   //获得数据 
			wav_recorder(3,0);
		}
		
		if(state & Init)
		{
			state &= ~Init;
			if(GadFlag == 0xff && WavFlag == 0xff)
			{
				GadFlag = gad_recorder(2,GADFS,FS);//初始化，采样频率
				WavFlag = wav_recorder(2,FS);//初始化
			}
		}
				
		if(state & Begin)
		{
			state &= ~Begin;
			if(GadFlag == 0xab && WavFlag == 0xab)
			{
				Real_Time_Year_STOP = Real_Time_Year;
				Real_Time_Month_STOP = Real_Time_Month;
				Real_Time_Day_STOP = Real_Time_Day;
				Real_Time_Hour_STOP = Real_Time_Hour;
				Real_Time_Minute_STOP = Real_Time_Minute;
				Real_Time_Second_STOP = Real_Time_Second;
				Real_Time_Millise_STOP = Real_Time_Millise;

				GadFlag = gad_recorder(4,0,0); //开始
				WavFlag = wav_recorder(4,0);//开始
			}
		}
			
		if(state & End)
		{
			state &= ~End;
			if(GadFlag == 0x80 && WavFlag == 0x80)
			{
				GadFlag = gad_recorder(1,0,0);       //停止
				WavFlag = wav_recorder(1,0);       //停止
				OLED_Clear( );
				delay_ms(10);
				OLED_ShowString(0,0,"Wait Recording",16);
				OLED_ShowString(0,2,"SoftID:",16);
				OLED_ShowString(56,2,SoftID,16);
				OLED_ShowString(0,4,"HardID:",16);
				OLED_ShowString(56,4,BordID,16);
			}
		}


		if((state&Blink) && (GadFlag == 0xff) && (WavFlag == 0xff))
		{
			LED0=!LED0;
			LED1=!LED1;
			delay_ms(250);
			//printf("State"); //告诉服务器目前待机
		}
		if(WavFlag == 0xab && GadFlag == 0xab)    //可以录音提示
		{
			LED0=!LED0;
			LED1=!LED1;
			delay_ms(100);
			//printf("State"); //告诉服务器目前待机
			//printf("Begin Create Wave File!File Name=%s%s&&&End",SessID,initfilename);
			printf("Ready To Start Recoder\r\n");
			RecvComLoc3 = 0;
			UARTSendData((u8*)&RecvComLoc3,4);
			for(i = 0;i<strlen("Ready To Start Recoder\r\n");i++)
				RecvComLoc3+="Ready To Start Recoder\r\n"[i];
			for(i = 0;i<8;i++)
				RecvComLoc3+=BordID[i];
			
			
			UARTSendData(&((u8*)&RecvComLoc3)[3],1);
			UARTSendData(&((u8*)&RecvComLoc3)[2],1);
			UARTSendData(&((u8*)&RecvComLoc3)[1],1);
			UARTSendData(&((u8*)&RecvComLoc3)[0],1);
			UARTSendData(BordID,8);
		}
		else if(WavFlag == 0x80 && GadFlag == 0x80)    //正在录音提示
		{
			timecnt++;
			if((timecnt%8000)==0)
			{
					LED0=!LED0;//DS0闪烁  
					//printf("State is Idle!"); //告诉服务器目前待机
			}
		}
		else
		{
			timecnt++;
			if((timecnt%32000)==0) //告诉服务器目前待机
			{
				
				
					sprintf(String_Windows_Time,"%0.2d",Real_Time_Month);
					OLED_ShowString(0,6,String_Windows_Time,16);
				OLED_ShowString(16,6,"-",16);
				
				sprintf(String_Windows_Time,"%0.2d",Real_Time_Day);
					OLED_ShowString(24,6,String_Windows_Time,16);
				OLED_ShowString(40,6," ",16);
				
				sprintf(String_Windows_Time,"%0.2d",Real_Time_Hour);
					OLED_ShowString(48,6,String_Windows_Time,16);
				
				OLED_ShowString(64,6,":",16);
				
				sprintf(String_Windows_Time,"%0.2d",Real_Time_Minute);
					OLED_ShowString(72,6,String_Windows_Time,16);
				OLED_ShowString(88,6,":",16);
				
				sprintf(String_Windows_Time,"%0.2d",Real_Time_Second);
					OLED_ShowString(96,6,String_Windows_Time,16);
				
				
				
					printf("Device is Idle\r\n");
					RecvComLoc3 = 1;
					UARTSendData(&((u8*)&RecvComLoc3)[3],1);
					UARTSendData(&((u8*)&RecvComLoc3)[2],1);
					UARTSendData(&((u8*)&RecvComLoc3)[1],1);
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
					RecvComLoc3 = 0;
					for(i = 0;i<strlen("Device is Idle\r\n");i++)
						RecvComLoc3+="Device is Idle\r\n"[i];
					for(i = 0;i<8;i++)
						RecvComLoc3+=BordID[i];
					RecvComLoc3+=1;
					UARTSendData(&((u8*)&RecvComLoc3)[3],1);
					UARTSendData(&((u8*)&RecvComLoc3)[2],1);
					UARTSendData(&((u8*)&RecvComLoc3)[1],1);
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
					UARTSendData(BordID,8);
					RecvComLoc3 = 1;
					UARTSendData(&((u8*)&RecvComLoc3)[0],1);
			}

		}
	} 
}




