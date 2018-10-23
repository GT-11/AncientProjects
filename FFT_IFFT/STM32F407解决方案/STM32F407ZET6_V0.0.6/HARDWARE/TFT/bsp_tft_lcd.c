/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : bsp_tft_lcd.c
*	版    本 : V4.0
*	说    明 : 支持3.0， 3.5， 4.3， 5.0， 7.0寸显示模块.
*			  3.0寸的支持的LCD内部驱动芯片型号有: SPFD5420A、OTM4001A、R61509V
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-08-21 armfly  ST固件库版本 V3.5.0版本。
*					a) 取消访问寄存器的结构体，直接定义
*		V2.0    2011-10-16 armfly  增加R61509V驱动，实现图标显示函数
*		V2.1    2012-07-06 armfly  增加RA8875驱动，支持4.3寸屏
*		V2.2    2012-07-13 armfly  改进LCD_DispStr函数，支持12点阵字符;修改LCD_DrawRect,解决差一个像素问题
*		V2.3    2012-08-08 armfly  将底层芯片寄存器操作相关的函数放到单独的文件，支持RA8875
*   	V3.0    2013-05-20 增加图标结构; 修改	LCD_DrawIconActive  修改DispStr函数支持文本透明
*		V3.1    2013-06-12 解决LCD_DispStr()函数BUG，如果内嵌字库中汉字个数多于256，则出现死循环。
*		V3.2    2013-06-28 完善Label控件, 当显示字符串比之前短时，自动清除多余的文字
*		V3.3    2013-06-29 FSMC初始化时，配置时序，写时序和读时序分开设置。 LCD_FSMCConfig 函数。
*		V3.4    2013-07-06 增加显示32位带Alpha图标的函数 LCD_DrawIcon32
*		V3.5    2013-07-24 增加显示32位带Alpha图片的函数 LCD_DrawBmp32
*		V3.6    2013-07-30 修改 DispEdit() 支持12点阵汉字对齐
*		V3.7    2014-09-06 修改 LCD_InitHard() 同时支持 RA8875-SPI接口和8080接口
*		V3.8    2014-09-15 增加若干函数:
*					（1） LCD_DispStrEx() 可以自动对齐自动填白的显示字符串函数
*					（2） LCD_GetStrWidth() 计算字符串的像素宽度
*		V3.9    2014-10-18
*					(1) 增加 LCD_ButtonTouchDown() LCD_ButtonTouchRelease 判断触摸坐标并重绘按钮
*					(2) 增加3.5寸LCD驱动
*					(3) 增加 LCD_SetDirection() 函数，设置显示屏方向（横屏 竖屏动态切换）
*
*
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp_tft_lcd.h"
#include "LCD_RA8875.h"
#include "stm32f4xx.h"
#include "delay.h"
/* 下面3个变量，主要用于使程序同时支持不同的屏 */
uint16_t g_ChipID = IC_8875;		/* 驱动芯片ID */
uint16_t g_LcdHeight = 480;			/* 显示屏分辨率-高度 */
uint16_t g_LcdWidth = 800;			/* 显示屏分辨率-宽度 */
uint8_t s_ucBright;					/* 背光亮度参数 */
uint8_t g_LcdDirection;				/* 显示方向.0，1，2，3 */

static void LCD_CtrlLinesConfig(void);
static void LCD_FSMCConfig(void);
void LCD_SetPwmBackLight(uint8_t _bright);

/*
*********************************************************************************************************
*	函 数 名: LCD_InitHard
*	功能说明: 初始化LCD
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitHard(void)
{

	/* 配置LCD控制口线GPIO */
	LCD_CtrlLinesConfig();

	/* 配置FSMC接口，数据总线 */
	LCD_FSMCConfig();

	/* FSMC重置后必须加延迟才能访问总线设备  */
	delay_us(20000);

	
			g_ChipID = IC_8875;
			RA8875_InitHard();	/* 初始化RA8875芯片 */

	//LCD_SetDirection(0);
	LCD_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */

	RA8875_SetBackLight(BRIGHT_MAX);	 /* 打开背光，设置为缺省亮度 */
}



/*
*********************************************************************************************************
*	函 数 名: LCD_GetHeight
*	功能说明: 读取LCD分辨率之高度
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_GetHeight(void)
{
	return g_LcdHeight;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetWidth
*	功能说明: 读取LCD分辨率之宽度
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_GetWidth(void)
{
	return g_LcdWidth;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_DispOn
*	功能说明: 打开显示
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: LCD_DispOff
*	功能说明: 关闭显示
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
		RA8875_DispOff();
}

/*
*********************************************************************************************************
*	函 数 名: LCD_ClrScr
*	功能说明: 根据输入的颜色值清屏
*	形    参: _usColor : 背景色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_ClrScr(uint16_t _usColor)
{
		RA8875_ClrScr(_usColor);
}



/*
*********************************************************************************************************
*	函 数 名: LCD_CtrlLinesConfig
*	功能说明: 配置LCD控制口线，FSMC管脚设置为复用功能
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
/*
	安富莱STM32-V5开发板接线方法:

	PD0/FSMC_D2
	PD1/FSMC_D3
	PD4/FSMC_NOE		--- 读控制信号，OE = Output Enable ， N 表示低有效
	PD5/FSMC_NWE		--- 写控制信号，WE = Output Enable ， N 表示低有效
	PD8/FSMC_D13
	PD9/FSMC_D14
	PD10/FSMC_D15
	PD13/FSMC_A18		--- 地址 RS
	PD14/FSMC_D0
	PD15/FSMC_D1

	PE4/FSMC_A20		--- 和主片选一起译码
	PE5/FSMC_A21		--- 和主片选一起译码
	PE7/FSMC_D4
	PE8/FSMC_D5
	PE9/FSMC_D6
	PE10/FSMC_D7
	PE11/FSMC_D8
	PE12/FSMC_D9
	PE13/FSMC_D10
	PE14/FSMC_D11
	PE15/FSMC_D12

	PG12/FSMC_NE4		--- 主片选（TFT, OLED 和 AD7606）

	PC3/TP_INT			--- 触摸芯片中断 （对于RA8875屏，是RA8875输出的中断)  本例程未使用硬件中断

	---- 下面是 TFT LCD接口其他信号 （FSMC模式不使用）----
	PD3/LCD_BUSY		--- 触摸芯片忙       （RA8875屏是RA8875芯片的忙信号)
	PF6/LCD_PWM			--- LCD背光PWM控制  （RA8875屏无需此脚，背光由RA8875控制)

	PI10/TP_NCS			--- 触摸芯片的片选		(RA8875屏无需SPI接口触摸芯片）
	PB3/SPI3_SCK		--- 触摸芯片SPI时钟		(RA8875屏无需SPI接口触摸芯片）
	PB4/SPI3_MISO		--- 触摸芯片SPI数据线MISO(RA8875屏无需SPI接口触摸芯片）
	PB5/SPI3_MOSI		--- 触摸芯片SPI数据线MOSI(RA8875屏无需SPI接口触摸芯片）
*/
static void LCD_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能FSMC时钟 */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* 使能 GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

	/* 设置 PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.14(D0), PD.15(D1) 为复用推挽输出 */

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

	/* 设置 PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) 为复用推挽输出 */

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

	/* 设置 PD.13(A18 (RS))  为复用推挽输出 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* 设置 PG12 (LCD/CS)) 为复用推挽输出 */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_FSMCConfig
*	功能说明: 配置FSMC并口访问时序
*	形    参: 无
*	返 回 值: 无
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
	/* 摄像头DMA麻点，需设置 4 0 5 2 0 0 */
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
	init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	/* 注意旧库无这个成员 */
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
*	函 数 名: LCD_GetBackLight
*	功能说明: 获得背光亮度参数
*	形    参: 无
*	返 回 值: 背光亮度参数
*********************************************************************************************************
*/
uint8_t LCD_GetBackLight(void)
{
	return s_ucBright;
}


/******************************************用于emwin接口函数******************************************/
/******************************************用于emwin接口函数******************************************/
/******************************************用于emwin接口函数******************************************/
/*
*********************************************************************************************************
*	函 数 名: LCD_PutPixel
*	功能说明: 画1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  : 像素颜色
*	返 回 值: 无
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
*	函 数 名: LCD_GetPixel
*	功能说明: 读取1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  : 像素颜色
*	返 回 值: RGB颜色值
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
*	函 数 名: GUI_FillRect
*	功能说明: 绘制填充的矩形（用于emWin的接口函数）
*	形    参:  x0， y0   起始点
*              x1， y1   结束点
*              _usColor  颜色
*	返 回 值: 无
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
*	函 数 名: emWin_DrawHLine
*	功能说明: 绘制一条固定颜色的水平线 （主要用于emWin的接口函数）
*	形    参：_usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usX2    ：结束点X坐标
*			  _usColor : 颜色
*	返 回 值: 无
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
*	函 数 名: emWin_DrawHColorLine
*	功能说明: 绘制一条彩色水平线 （主要用于emWin的接口函数）
*	形    参：_usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usWidth ：直线的宽度
*			  _pColor : 颜色缓冲区
*	返 回 值: 无
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
*	函 数 名: emWin_DrawVLine
*	功能说明: 绘制一条垂直线条 （主要用于emWin的接口函数）
*	形    参： _usX1    : 垂直线的X坐标
*			  _usY1    : 起始点Y坐标
*			  _usY2    : 结束点Y坐标
*			  _usColor : 颜色
*	返 回 值: 无
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

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
