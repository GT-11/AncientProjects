/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V3.0
*	说    明 : 本实验主要
*              实验目的：
*                1. 学习FreeRTOS的任务控制函数使用。
*              实验内容：
*                1. 共创建了4个用户任务
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V5开发板光盘里面有。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期        作者     说明
*		v1.0    2013-09-10  Eric2013  创建该文件，ST固件库版本为V1.0.2
*		V2.0    2014-02-24  Eric2013  升级FreeRTOS到V8.0.0，升级F4固件库到1.3.0 
*	    V3.0    2015-04-01  Eric2013  1. 升级固件库到V1.5.0
*                                     2. 升级BSP板级支持包
*									  3. 升级FreeRTOS到V8.2.0
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"


/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void AppTaskCreate (void);
extern void vSetupTimerTest( void );

/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
xTaskHandle xHandleTask3;

xTaskHandle task;
int8_t pcWriteBuffer[500];
u8 pcWriteBuffer2[500];

extern u8 TX_ADDRESS[TX_ADR_WIDTH];
extern u8 TX_ADDRESS2[TX_ADR_WIDTH];
extern u8 RX_ADDRESS[RX_ADR_WIDTH];

WM_HWIN hWin;

char AIR_A_VALUE=32;
char AIR_B_VALUE=32;
char AIR_C_VALUE=32;
char Light_VALUE[6]={0,0,0,0,0,0};

u8 Temp_A=0;
u8 Hum_A=0;

u8 Temp_B=0;
u8 Hum_B=0;

u8 Temp_C=0;
u8 Hum_C=0;

u8 buff[32];
char a[5];
char b[5];
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
    /* 硬件初始化 */
    bsp_Init();
	  
	/* 创建任务 */
	AppTaskCreate();
    
    vSetupTimerTest();
	
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();

	/* 如果系统正常启动是不会运行到这里的，运行到这里极有可能是空闲任务heap空间不足造成创建失败 */
    while (1)
    {
    } 
}

/*
*********************************************************************************************************
*	函 数 名: vTask1
*	功能说明: LED闪烁		
*	形    参：pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
void vTask1( void *pvParameters )
{

  portTickType xDelay, xNextTime;
	u8 x=0;
	xNextTime = xTaskGetTickCount () + ( portTickType ) 100;
    while(1)
    {
			
			buff[0]=0x31;
			//NRF24L01_Tx_CRC(buff,TX_ADDRESS);
			NRF24L01_Tx_CRC(buff,TX_ADDRESS);
			while(1)
			{
				if(!NRF24L01_Rx_CRC(buff,TX_ADDRESS))
				{
					x=0;
					//sprintf(a,"%d C",buf[1]);	
					//sprintf(b,"%d %%",buf[3]);
					a[0]=(buff[1]/10)+0x30;
					a[1]=(buff[1]%10)+0x30;	
					a[2]=' ';
					a[3]='C';
					a[4]=0x00;
						
					b[0]=(buff[3]/10)+0x30;
					b[1]=(buff[3]%10)+0x30;	
					b[2]=' ';
					b[3]='%';
					b[4]=0x00;
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), a);
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), b);
					break;
			  }
				else if(x>20)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), "NULL");
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "NULL");	
					break;
				}
				if(x<100)x++;
				vTaskDelay( 100 );
			}
			
			
			
			
			
//			while(1)
//			{
//				if(!NRF24L01_Rx_CRC(buff,TX_ADDRESS2))
//				{
//					x=0;
//					//sprintf(a,"%d C",buf[1]);	
//					//sprintf(b,"%d %%",buf[3]);
//					a[0]=(buff[1]/10)+0x30;
//					a[1]=(buff[1]%10)+0x30;	
//					a[2]=' ';
//					a[3]='C';
//					a[4]=0x00;
//						
//					b[0]=(buff[3]/10)+0x30;
//					b[1]=(buff[3]%10)+0x30;	
//					b[2]=' ';
//					b[3]='%';
//					b[4]=0x00;
//					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), a);
//					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), b);
//					break;
//			  }
//				else if(x>20)
//				{
//					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), "NULL");
//					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), "NULL");	
//					break;
//				}
//				if(x<100)x++;
//				vTaskDelay( 100 );
//			}
			
			
			

		/* 用vTaskDelay实现vTaskDelayUntil() */
		xDelay = xNextTime - xTaskGetTickCount ();
		xNextTime += ( portTickType ) 100;
		if( xDelay <= ( portTickType ) 100 )
		{
			vTaskDelay( xDelay );
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: vTask2
*	功能说明: LED闪烁		
*	形    参：pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
void vTask2( void *pvParameters )
{
	portTickType xLastWakeTime;
    const portTickType xFrequency = 10;

	/* 获取系统时间 */
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
				//GUI_TOUCH_Exec();
				GT811_OnePiontScan();
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTask3
*	功能说明: 挂起任务3	
*	形    参：pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
void vTask3( void *pvParameters )
{
		GUI_Init();
    WM_SetDesktopColor(GUI_WHITE);      /* Automacally update desktop window */
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
    BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
    CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
    DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
    SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
    HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
    RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	
	
		hWin=CreateWindow();
    while(1)
    {
		/* 挂起自己，可以写NULL或者xHandleTask3句柄，都可以的 */
			GUI_Delay(5);
		
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTask4
*	功能说明: 打印任务信息	
*	形    参：pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
void vTask4( void *pvParameters )
{
	portTickType xLastWakeTime;
    const portTickType xFrequency = 1000;

	 /* 获取系统时间 */
     xLastWakeTime = xTaskGetTickCount();
     
    while(1)
    {
		//vTaskResume(xHandleTask3);
        vTaskList((char *)&pcWriteBuffer);
		printf("%s\r\n", pcWriteBuffer);
		
		vTaskGetRunTimeStats((char *)&pcWriteBuffer);
		printf("%s\r\n", pcWriteBuffer);
			
			//NRF24L01_RxPacket(pcWriteBuffer2);
		//	printf("%d\r\n", pcWriteBuffer2);
			
		//NRF24L01_TxPacket((u8*)"1234567890");
		//	NRF24L01_TxPacket((u8*)"abcdefgh");
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
		xTaskCreate(    vTask3,     /* 任务函数  */
                    "GUI",   /* 任务名    */
                    500,        /* stack大小，单位word，也就是4字节 */
                    NULL,       /* 任务参数  */
                    3,          /* 任务优先级*/
                    NULL );     /* 任务句柄  */
	
	xTaskCreate(    vTask1,     /* 任务函数  */
                    "Number",   /* 任务名    */
                    500,        /* stack大小，单位word，也就是4字节 */
                    NULL,       /* 任务参数  */
                    1,          /* 任务优先级*/
                    &task );     /* 任务句柄  */
	
	xTaskCreate(    vTask2,     /* 任务函数  */
                    "Touch",   /* 任务名    */
                    500,        /* stack大小，单位word，也就是4字节 */
                    NULL,       /* 任务参数  */
                    2,          /* 任务优先级*/
                    NULL );     /* 任务句柄  */
	
	
	xTaskCreate(    vTask4,     /* 任务函数  */
                    "Print",   /* 任务名    */
                    500,        /* stack大小，单位word，也就是4字节 */
                    NULL,       /* 任务参数  */
                    4,          /* 任务优先级*/
                    NULL );     /* 任务句柄  */
									
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

