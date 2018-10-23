/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V3.0
*	˵    �� : ��ʵ����Ҫ
*              ʵ��Ŀ�ģ�
*                1. ѧϰFreeRTOS��������ƺ���ʹ�á�
*              ʵ�����ݣ�
*                1. ��������4���û�����
*              ע�����
*                1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                   V5��������������С�
*                2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��   ����        ����     ˵��
*		v1.0    2013-09-10  Eric2013  �������ļ���ST�̼���汾ΪV1.0.2
*		V2.0    2014-02-24  Eric2013  ����FreeRTOS��V8.0.0������F4�̼��⵽1.3.0 
*	    V3.0    2015-04-01  Eric2013  1. �����̼��⵽V1.5.0
*                                     2. ����BSP�弶֧�ְ�
*									  3. ����FreeRTOS��V8.2.0
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "bsp.h"
#include "GUI.h"

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void AppTaskCreate (void);
extern void vSetupTimerTest( void );

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
xTaskHandle xHandleTask3;
int8_t pcWriteBuffer[500];
u8 pcWriteBuffer2[500];

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
u8 DATA[32];
u8 DATA_N[32];
char a[5];
char b[5];
/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
    /* Ӳ����ʼ�� */
    bsp_Init();
	  
	/* �������� */
	AppTaskCreate();
    
    vSetupTimerTest();
	
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();

	/* ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п����ǿ�������heap�ռ䲻����ɴ���ʧ�� */
    while (1)
    {
    } 
}

/*
*********************************************************************************************************
*	�� �� ��: vTask1
*	����˵��: LED��˸		
*	��    �Σ�pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vTask1( void *pvParameters )
{
    while(1)
    {
			
			
			
			
				if(DATA[3]>0)
				{
					//sprintf(a,"%d C",buf[1]);	
					//sprintf(b,"%d %%",buf[3]);
					a[0]=(DATA[3]/10)+0x30;
					a[1]=(DATA[3]%10)+0x30;	
					a[2]=' ';
					a[3]='C';
					a[4]=0x00;
						
					b[0]=(DATA[5]/10)+0x30;
					b[1]=(DATA[5]%10)+0x30;	
					b[2]=' ';
					b[3]='%';
					b[4]=0x00;
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), a);
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), b);
			  }
				else if(DATA_N[0]>10)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), "NULL");
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), "NULL");	
				}
				
				
				
				
				if(DATA[8]>0)
				{
					//sprintf(a,"%d C",buf[1]);	
					//sprintf(b,"%d %%",buf[3]);
					a[0]=(DATA[9]/10)+0x30;
					a[1]=(DATA[9]%10)+0x30;	
					a[2]=' ';
					a[3]='C';
					a[4]=0x00;
						
					b[0]=(DATA[11]/10)+0x30;
					b[1]=(DATA[11]%10)+0x30;	
					b[2]=' ';
					b[3]='%';
					b[4]=0x00;
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), a);
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), b);
			  }
				else if(DATA_N[1]>10)
				{
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), "NULL");
					TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), "NULL");	
				}
			

			
			
			

			vTaskDelay( 1000 );

	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTask5
*	����˵��: LED��˸		
*	��    �Σ�pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vTask5( void *pvParameters )
{
		
	u8 delay;
	DATA_N[0]=0;
	DATA_N[1]=0;
    while(1)
    {
			///////////////////////////////////////////////////////////////////////
			for(delay=0;delay<200;delay++){}
			NRF24L01_TX_Mode();
			for(delay=0;delay<200;delay++){}
			NRF24L01_TxPacket("******_Read_Address_0001_******");
			for(delay=0;delay<200;delay++){}
			NRF24L01_RX_Mode();
			for(delay=0;delay<200;delay++){}
			delay=0;
			while(1)
			{
				if(!NRF24L01_RxPacket(buff))
				{
					if(buff[0]==0x00&&buff[1]==0x01)
					{
						DATA_N[0]=0;
						DATA[0]=0x00;
						DATA[1]=0x01;
						DATA[2]=buff[2];
						DATA[3]=buff[3];
						DATA[4]=buff[4];
						DATA[5]=buff[5];
						break;
					}
				}
				else if(delay>10)
				{
					DATA_N[0]++;
					DATA[0]=0x00;
					DATA[1]=0x01;
					DATA[2]=0x00;
					DATA[3]=0x00;
					DATA[4]=0x00;
					DATA[5]=0x00;
					break;
				}
				if(delay<100)delay++;
				vTaskDelay( 10 );
			}
			/////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////
			for(delay=0;delay<200;delay++){}
			NRF24L01_TX_Mode();
			for(delay=0;delay<200;delay++){}
			NRF24L01_TxPacket("******_Read_Address_0002_******");
			for(delay=0;delay<200;delay++){}
			NRF24L01_RX_Mode();
			for(delay=0;delay<200;delay++){}
			delay=0;
			while(1)
			{
				if(!NRF24L01_RxPacket(buff))
				{
					if(buff[0]==0x00&&buff[1]==0x02)
					{
						DATA_N[1]=0;
						DATA[6]=0x00;
						DATA[7]=0x02;
						DATA[8]=buff[2];
						DATA[9]=buff[3];
						DATA[10]=buff[4];
						DATA[11]=buff[5];
						break;
					}
				}
				else if(delay>10)
				{
					DATA_N[1]++;
					DATA[6]=0x00;
					DATA[7]=0x02;
					DATA[8]=0x00;
					DATA[9]=0x00;
					DATA[10]=0x00;
					DATA[11]=0x00;
					break;
				}
				if(delay<100)delay++;
				vTaskDelay( 10 );
			}
			/////////////////////////////////////////////////////////////////////////
			vTaskDelay( 500 );
    }
		//vTaskDelay( 1000 );
}

/*
*********************************************************************************************************
*	�� �� ��: vTask2
*	����˵��: LED��˸		
*	��    �Σ�pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vTask2( void *pvParameters )
{
	portTickType xLastWakeTime;
    const portTickType xFrequency = 10;

	/* ��ȡϵͳʱ�� */
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
*	�� �� ��: vTask3
*	����˵��: ��������3	
*	��    �Σ�pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
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
		/* �����Լ�������дNULL����xHandleTask3����������Ե� */
			GUI_Delay(5);
		
    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTask4
*	����˵��: ��ӡ������Ϣ	
*	��    �Σ�pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vTask4( void *pvParameters )
{
	portTickType xLastWakeTime;
    const portTickType xFrequency = 1000;

	 /* ��ȡϵͳʱ�� */
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
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
		xTaskCreate(    vTask1,     /* ������  */
                    "Number",   /* ������    */
                    500,        /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,       /* �������  */
                    4,          /* �������ȼ�*/
                    NULL );     /* ������  */
	
	xTaskCreate(    vTask2,     /* ������  */
                    "Touch",   /* ������    */
                    500,        /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,       /* �������  */
                    3,          /* �������ȼ�*/
                    NULL );     /* ������  */
	
	xTaskCreate(    vTask3,     /* ������  */
                    "GUI",   /* ������    */
                    500,        /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,       /* �������  */
                    2,          /* �������ȼ�*/
                    NULL );     /* ������  */
	
	
	xTaskCreate(    vTask4,     /* ������  */
                    "Print",   /* ������    */
                    500,        /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,       /* �������  */
                    1,          /* �������ȼ�*/
                    NULL );     /* ������  */
										
										
	xTaskCreate(    vTask5,     /* ������  */
                    "24L01",   /* ������    */
                    500,        /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,       /* �������  */
                    5,          /* �������ȼ�*/
                    NULL );     /* ������  */
									
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

