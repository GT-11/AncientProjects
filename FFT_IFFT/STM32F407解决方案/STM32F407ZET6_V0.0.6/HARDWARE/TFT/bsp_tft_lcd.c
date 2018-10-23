/*
*********************************************************************************************************
*
*	ģ������ : TFTҺ����ʾ������ģ��
*	�ļ����� : bsp_tft_lcd.c
*	��    �� : V4.0
*	˵    �� : ֧��3.0�� 3.5�� 4.3�� 5.0�� 7.0����ʾģ��.
*			  3.0���֧�ֵ�LCD�ڲ�����оƬ�ͺ���: SPFD5420A��OTM4001A��R61509V
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-08-21 armfly  ST�̼���汾 V3.5.0�汾��
*					a) ȡ�����ʼĴ����Ľṹ�壬ֱ�Ӷ���
*		V2.0    2011-10-16 armfly  ����R61509V������ʵ��ͼ����ʾ����
*		V2.1    2012-07-06 armfly  ����RA8875������֧��4.3����
*		V2.2    2012-07-13 armfly  �Ľ�LCD_DispStr������֧��12�����ַ�;�޸�LCD_DrawRect,�����һ����������
*		V2.3    2012-08-08 armfly  ���ײ�оƬ�Ĵ���������صĺ����ŵ��������ļ���֧��RA8875
*   	V3.0    2013-05-20 ����ͼ��ṹ; �޸�	LCD_DrawIconActive  �޸�DispStr����֧���ı�͸��
*		V3.1    2013-06-12 ���LCD_DispStr()����BUG�������Ƕ�ֿ��к��ָ�������256���������ѭ����
*		V3.2    2013-06-28 ����Label�ؼ�, ����ʾ�ַ�����֮ǰ��ʱ���Զ�������������
*		V3.3    2013-06-29 FSMC��ʼ��ʱ������ʱ��дʱ��Ͷ�ʱ��ֿ����á� LCD_FSMCConfig ������
*		V3.4    2013-07-06 ������ʾ32λ��Alphaͼ��ĺ��� LCD_DrawIcon32
*		V3.5    2013-07-24 ������ʾ32λ��AlphaͼƬ�ĺ��� LCD_DrawBmp32
*		V3.6    2013-07-30 �޸� DispEdit() ֧��12�����ֶ���
*		V3.7    2014-09-06 �޸� LCD_InitHard() ͬʱ֧�� RA8875-SPI�ӿں�8080�ӿ�
*		V3.8    2014-09-15 �������ɺ���:
*					��1�� LCD_DispStrEx() �����Զ������Զ���׵���ʾ�ַ�������
*					��2�� LCD_GetStrWidth() �����ַ��������ؿ��
*		V3.9    2014-10-18
*					(1) ���� LCD_ButtonTouchDown() LCD_ButtonTouchRelease �жϴ������겢�ػ水ť
*					(2) ����3.5��LCD����
*					(3) ���� LCD_SetDirection() ������������ʾ�����򣨺��� ������̬�л���
*
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp_tft_lcd.h"
#include "LCD_RA8875.h"
#include "stm32f4xx.h"
#include "delay.h"
/* ����3����������Ҫ����ʹ����ͬʱ֧�ֲ�ͬ���� */
uint16_t g_ChipID = IC_8875;		/* ����оƬID */
uint16_t g_LcdHeight = 480;			/* ��ʾ���ֱ���-�߶� */
uint16_t g_LcdWidth = 800;			/* ��ʾ���ֱ���-��� */
uint8_t s_ucBright;					/* �������Ȳ��� */
uint8_t g_LcdDirection;				/* ��ʾ����.0��1��2��3 */

static void LCD_CtrlLinesConfig(void);
static void LCD_FSMCConfig(void);
void LCD_SetPwmBackLight(uint8_t _bright);

/*
*********************************************************************************************************
*	�� �� ��: LCD_InitHard
*	����˵��: ��ʼ��LCD
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitHard(void)
{

	/* ����LCD���ƿ���GPIO */
	LCD_CtrlLinesConfig();

	/* ����FSMC�ӿڣ��������� */
	LCD_FSMCConfig();

	/* FSMC���ú������ӳٲ��ܷ��������豸  */
	delay_us(20000);

	
			g_ChipID = IC_8875;
			RA8875_InitHard();	/* ��ʼ��RA8875оƬ */

	//LCD_SetDirection(0);
	LCD_ClrScr(CL_BLACK);	/* ��������ʾȫ�� */

	RA8875_SetBackLight(BRIGHT_MAX);	 /* �򿪱��⣬����Ϊȱʡ���� */
}



/*
*********************************************************************************************************
*	�� �� ��: LCD_GetHeight
*	����˵��: ��ȡLCD�ֱ���֮�߶�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetHeight(void)
{
	return g_LcdHeight;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetWidth
*	����˵��: ��ȡLCD�ֱ���֮���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetWidth(void)
{
	return g_LcdWidth;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispOn
*	����˵��: ����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispOn(void)
{
	if (g_ChipID == IC_8875)
	{
		RA8875_DispOn();
	}
	else if (g_ChipID == IC_9488)
	{
//		ILI9488_DispOn();
	}
	else	/* 61509, 5420, 4001 */
	{
//		SPFD5420_DispOn();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispOff
*	����˵��: �ر���ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
		RA8875_DispOff();
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_ClrScr
*	����˵��: �����������ɫֵ����
*	��    ��: _usColor : ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_ClrScr(uint16_t _usColor)
{
		RA8875_ClrScr(_usColor);
}



/*
*********************************************************************************************************
*	�� �� ��: LCD_CtrlLinesConfig
*	����˵��: ����LCD���ƿ��ߣ�FSMC�ܽ�����Ϊ���ù���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
/*
	������STM32-V5��������߷���:

	PD0/FSMC_D2
	PD1/FSMC_D3
	PD4/FSMC_NOE		--- �������źţ�OE = Output Enable �� N ��ʾ����Ч
	PD5/FSMC_NWE		--- д�����źţ�WE = Output Enable �� N ��ʾ����Ч
	PD8/FSMC_D13
	PD9/FSMC_D14
	PD10/FSMC_D15
	PD13/FSMC_A18		--- ��ַ RS
	PD14/FSMC_D0
	PD15/FSMC_D1

	PE4/FSMC_A20		--- ����Ƭѡһ������
	PE5/FSMC_A21		--- ����Ƭѡһ������
	PE7/FSMC_D4
	PE8/FSMC_D5
	PE9/FSMC_D6
	PE10/FSMC_D7
	PE11/FSMC_D8
	PE12/FSMC_D9
	PE13/FSMC_D10
	PE14/FSMC_D11
	PE15/FSMC_D12

	PG12/FSMC_NE4		--- ��Ƭѡ��TFT, OLED �� AD7606��

	PC3/TP_INT			--- ����оƬ�ж� ������RA8875������RA8875������ж�)  ������δʹ��Ӳ���ж�

	---- ������ TFT LCD�ӿ������ź� ��FSMCģʽ��ʹ�ã�----
	PD3/LCD_BUSY		--- ����оƬæ       ��RA8875����RA8875оƬ��æ�ź�)
	PF6/LCD_PWM			--- LCD����PWM����  ��RA8875������˽ţ�������RA8875����)

	PI10/TP_NCS			--- ����оƬ��Ƭѡ		(RA8875������SPI�ӿڴ���оƬ��
	PB3/SPI3_SCK		--- ����оƬSPIʱ��		(RA8875������SPI�ӿڴ���оƬ��
	PB4/SPI3_MISO		--- ����оƬSPI������MISO(RA8875������SPI�ӿڴ���оƬ��
	PB5/SPI3_MOSI		--- ����оƬSPI������MOSI(RA8875������SPI�ӿڴ���оƬ��
*/
static void LCD_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��FSMCʱ�� */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* ʹ�� GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

	/* ���� PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.14(D0), PD.15(D1) Ϊ����������� */

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* ���� PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) Ϊ����������� */

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                            GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* ���� PD.13(A18 (RS))  Ϊ����������� */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* ���� PG12 (LCD/CS)) Ϊ����������� */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_FSMCConfig
*	����˵��: ����FSMC���ڷ���ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void LCD_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef  init;
	FSMC_NORSRAMTimingInitTypeDef  timingWrite;
	FSMC_NORSRAMTimingInitTypeDef  timingRead;

	/*-- FSMC Configuration ------------------------------------------------------*/
	/*----------------------- SRAM Bank 4 ----------------------------------------*/
	/* FSMC_Bank1_NORSRAM4 configuration */
	/* ����ͷDMA��㣬������ 4 0 5 2 0 0 */
	timingWrite.FSMC_AddressSetupTime = 4;
	timingWrite.FSMC_AddressHoldTime = 0;
	timingWrite.FSMC_DataSetupTime = 6;
	timingWrite.FSMC_BusTurnAroundDuration = 1;
	timingWrite.FSMC_CLKDivision = 0;
	timingWrite.FSMC_DataLatency = 0;
	timingWrite.FSMC_AccessMode = FSMC_AccessMode_A;

	timingRead.FSMC_AddressSetupTime = 4;
	timingRead.FSMC_AddressHoldTime = 0;
	timingRead.FSMC_DataSetupTime = 8;
	timingRead.FSMC_BusTurnAroundDuration = 1;
	timingRead.FSMC_CLKDivision = 0;
	timingRead.FSMC_DataLatency = 0;
	timingRead.FSMC_AccessMode = FSMC_AccessMode_A;


	/*
	 LCD configured as follow:
	    - Data/Address MUX = Disable
	    - Memory Type = SRAM
	    - Data Width = 16bit
	    - Write Operation = Enable
	    - Extended Mode = Enable
	    - Asynchronous Wait = Disable
	*/
	init.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	/* ע��ɿ��������Ա */
	init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	init.FSMC_WrapMode = FSMC_WrapMode_Disable;
	init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

	init.FSMC_ReadWriteTimingStruct = &timingRead;
	init.FSMC_WriteTimingStruct = &timingWrite;

	FSMC_NORSRAMInit(&init);

	/* - BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}




/*
*********************************************************************************************************
*	�� �� ��: LCD_GetBackLight
*	����˵��: ��ñ������Ȳ���
*	��    ��: ��
*	�� �� ֵ: �������Ȳ���
*********************************************************************************************************
*/
uint8_t LCD_GetBackLight(void)
{
	return s_ucBright;
}


/******************************************����emwin�ӿں���******************************************/
/******************************************����emwin�ӿں���******************************************/
/******************************************����emwin�ӿں���******************************************/
/*
*********************************************************************************************************
*	�� �� ��: LCD_PutPixel
*	����˵��: ��1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	if (g_ChipID == IC_8875)
	{
		RA8875_PutPixelGUI(_usX, _usY, _usColor);
	}
	else if (g_ChipID == IC_9488)
	{
//		ILI9488_PutPixel(_usX, _usY, _usColor);
	}
	else
	{
//		SPFD5420_PutPixel(_usX, _usY, _usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetPixel
*	����˵��: ��ȡ1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usRGB;

	if (g_ChipID == IC_8875)
	{
		usRGB = RA8875_GetPixelGUI(_usX, _usY);
	}
	else if (g_ChipID == IC_9488)
	{
//		usRGB = ILI9488_GetPixel(_usX, _usY);
	}
	else
	{
//		usRGB = SPFD5420_GetPixel(_usX, _usY);
	}

	return usRGB;
}

/*
*********************************************************************************************************
*	�� �� ��: GUI_FillRect
*	����˵��: �������ľ��Σ�����emWin�Ľӿں�����
*	��    ��:  x0�� y0   ��ʼ��
*              x1�� y1   ������
*              _usColor  ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void emWin_FillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t _usColor) 
{
	if (g_ChipID == IC_8875)
	{
		RA8875_RTERect( x0,  y0,  x1,  y1,  _usColor); 
	}
	else
	{
//		ILI9488_FillRect(x0, y0, y1-y0+1, x1-x0+1, _usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: emWin_DrawHLine
*	����˵��: ����һ���̶���ɫ��ˮƽ�� ����Ҫ����emWin�Ľӿں�����
*	��    �Σ�_usX1    ����ʼ��X����
*			  _usY1    ��ˮƽ�ߵ�Y����
*			  _usX2    ��������X����
*			  _usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void emWin_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor)
{
	if (g_ChipID == IC_8875)
	{
		 RA8875_DrawHLine( _usX1 ,  _usY1 ,  _usX2 ,  _usColor);
	}
	else
	{
//		ILI9488_DrawHLine( _usX1 ,  _usY1 ,  _usX2 ,  _usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: emWin_DrawHColorLine
*	����˵��: ����һ����ɫˮƽ�� ����Ҫ����emWin�Ľӿں�����
*	��    �Σ�_usX1    ����ʼ��X����
*			  _usY1    ��ˮƽ�ߵ�Y����
*			  _usWidth ��ֱ�ߵĿ��
*			  _pColor : ��ɫ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void emWin_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{
	if (g_ChipID == IC_8875)
	{
		 RA8875_DrawHColorLine( _usX1 ,  _usY1,  _usWidth,  _pColor);
	}
	else
	{
//		ILI9488_DrawHColorLine( _usX1 ,  _usY1,  _usWidth,  _pColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: emWin_DrawVLine
*	����˵��: ����һ����ֱ���� ����Ҫ����emWin�Ľӿں�����
*	��    �Σ� _usX1    : ��ֱ�ߵ�X����
*			  _usY1    : ��ʼ��Y����
*			  _usY2    : ������Y����
*			  _usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void emWin_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor)
{
	if (g_ChipID == IC_8875)
	{
		 RA8875_DrawVLine( _usX1 ,  _usY1 ,  _usY2 ,  _usColor);
	}
	else
	{
//		ILI9488_DrawVLine( _usX1 ,  _usY1 ,  _usY2 ,  _usColor);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
