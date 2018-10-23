/*
*********************************************************************************************************
*
*	模块名称 : 电阻式触摸板驱动模块
*	文件名称 : bsp_touch.c
*	版    本 : V1.7
*	说    明 : 驱动TS2046芯片 和 RA8875内置触摸
*	修改记录 :
*		版本号  日期        作者    说明
*       v1.0    2012-07-06 armfly  ST固件库V3.5.0版本。
*		v1.1    2012-10-22 armfly  增加4点校准
*		v1.2    2012-11-07 armfly  解决4点校准的XY交换分支的bug
*		v1.3    2012-12-17 armfly  触摸校准函数增加入口参数:等待时间
*		V1.4    2013-07-26 armfly  更改 TOUCH_DataFilter() 滤波算法
*		V1.5    2013-07-32 armfly  修改TOUCH_WaitRelease(),计数器需要清零
*		V1.6    2014-10-20 armfly
*					(1) 修改 TOUCH_PutKey() 函数，实现触摸屏的横屏和竖屏动态切换.
*					(2) param 结构增加校准时当前的屏幕方向变量 TouchDirection
*					(3) 调试3.5寸的触摸芯片。修改SPI相关配置函数。
*					(4) 由于触摸芯片TSC2046和串行FLASH,NRF24L01,MP3等模块共享SPI总线。因此需要
*						在触摸中断函数中判断总线冲突. 增加函数 bsp_SpiBusBusy() 判忙.
*					(5) TSC2046增加软件模拟SPI (软件模拟方式方便SPI设备共享)
*		V1.7    2015-01-02 armfly  计划将触摸扫描由1ms一次修改为10ms一次。未定。
*
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
//#include "param.h"	/* 包含参数存储模块 */
#include "GUI.h"

/*
【1】安富莱STM32-V5开发板 + 3.5寸显示模块， 显示模块上的触摸芯片为 TSC2046或其兼容芯片
	PI10   --> TP_CS
	PD3/LCD_BUSY   --> TP_BUSY
	PB3/SPI3_SCK   --> TP_SCK
	PB4/SPI3_MISO   --> TP_MISO
	PB5/SPI3_MOSI   --> TP_MOSI
	PI3/TP_INT   --> TP_PEN_INT

【2】安富莱STM32开发板 + 4.3寸或7寸显示模块（内置RA8875芯片)
	RA8875内置触摸屏接口，因此直接通过FSMC访问RA8875相关寄存器即可。


	本程序未使用触笔中断功能。在1ms周期的 Systick定时中断服务程序中对触摸ADC值进行采样和
	滤波处理。当触笔按下超过40ms后，开始采集ADC值（每1ms采集1次，连续采集10次），然后对
	这10个样本进行排序，丢弃2头的样本，对中间一段样本进行求和并计算平均值。

	采样2点校准方法，校准后保存2个校准点的ADC值，实际工作时，根据2点直线方程计算屏幕坐标。
	校准参数有保存接口，本程序主要用于演示，未做保存功能。
	大家可以自己修改  TOUCH_SaveParam() 和 TOUCH_LoadParam() 两个函数实现保存功能。

*/
/* TSC2046 内部ADC通道号 */
#define ADC_CH_X	1		/* X通道，测量X位置 */
#define ADC_CH_Y	5		/* Y通道，测量Y位置 */

/* 每1ms扫描一次坐标 */
#define DOWN_VALID		30	/* 按下30ms 后, 开始统计ADC */
#define SAMPLE_COUNT	10	/* 按下后连续采集40个样本 */

/*
触摸屏校准点相对屏幕像素四角的偏移像素
第1个点 ： x1 = CALIB_OFFSET, y1 = CALIB_OFFSET
第2个点 ： x2 = LCD_GetWidth() - CALIB_OFFSET, y2 = LCD_GetHeight() - CALIB_OFFSET
*/
#define CALIB_OFFSET	20
#define TP_X1	CALIB_OFFSET
#define TP_Y1	CALIB_OFFSET

#define TP_X2	(LCD_GetWidth() - CALIB_OFFSET)
#define TP_Y2	(LCD_GetHeight() - CALIB_OFFSET)

#define TP_X3	CALIB_OFFSET
#define TP_Y3	(LCD_GetHeight() - CALIB_OFFSET)

#define TP_X4	(LCD_GetWidth() - CALIB_OFFSET)
#define TP_Y4	CALIB_OFFSET

/* 有效ADC值的判断门限. 太接近ADC临界值的坐标认为无效 */
#define ADC_VALID_OFFSET	2

//#define WaitTPReady() while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))
#define WaitTPReady() {}

/* 触屏模块用到的全局变量 */
TOUCH_T g_tTP;

//static uint8_t	TOUCH_PressValid(uint16_t _usX, uint16_t _usY);
//static uint16_t TOUCH_DataFilter(uint16_t *_pBuf, uint8_t _ucCount);
//static void TOUCH_LoadParam(void);
//static void TOUCH_SaveParam(void);
//static int32_t CalTwoPoint(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x);
//static int16_t TOUCH_TransX(uint16_t _usAdcX, uint16_t _usAdcY);
//static int16_t TOUCH_TransY(uint16_t _usAdcX, uint16_t _usAdcY);
int32_t TOUCH_Abs(int32_t x);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitTouch
*	功能说明: 配置STM32和触摸相关的口线，使能硬件SPI1, 片选由软件控制
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_InitHard(void)
{
	uint16_t i;
	
    g_tTP.Enable = 0;

	/* 50ms，等待GT811复位就绪，才能探测GT811芯片 ID */
	for (i = 0; i < 5; i++)
	{
		if (i2c_CheckDevice(GT811_I2C_ADDR) == 0)
		{
			GT811_InitHard();
			return;
		}
		bsp_DelayUS(10000);
	}
		
//	TOUCH_LoadParam();	/* 读取校准参数 */

    g_tTP.Write = g_tTP.Read = 0;

	g_tTP.Enable = 1;
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_ReadAdcX
*	功能说明: 获得触摸板X方向ADC采样值， 已进行滤波处理
*	形    参:  无
*	返 回 值: X 方向ADC值
*********************************************************************************************************
*/
uint16_t TOUCH_ReadAdcX(void)
{
	uint16_t usAdc;

	//__set_PRIMASK(1);  		/* 关中断 */
	usAdc = g_tTP.usAdcNowX;
	//__set_PRIMASK(0);  		/* 开中断 */

	return usAdc;
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_ReadAdcY
*	功能说明: 获得触摸板Y方向ADC采样值， 已进行滤波处理
*	形    参:  无
*	返 回 值: Y 坐标值，允许负值
*********************************************************************************************************
*/
uint16_t TOUCH_ReadAdcY(void)
{
	uint16_t usAdc;

	//__set_PRIMASK(1);  		/* 关中断 */
	usAdc = g_tTP.usAdcNowY;
	//__set_PRIMASK(0);  		/* 开中断 */

	return usAdc;
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_PutKey
*	功能说明: 将1个触摸点坐标值压入触摸FIFO缓冲区。用于电阻触摸屏。
*	形    参: _usX, _usY 坐标值
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_PutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY)
{
	uint16_t xx, yy;
	uint16_t x = 0, y = 0;

	g_tTP.Event[g_tTP.Write] = _ucEvent;

	if (g_GT811.Enable == 1)	/* 电容屏 (无需校准) */
	{
		xx = _usX;
		yy = _usY;
	}
	else	/* 电阻屏 */
	{
//		xx = TOUCH_TransX(_usX, _usY);
//		yy = TOUCH_TransY(_usX, _usY);
	}
	
	/* 横屏和竖屏方向识别 */
	switch (0)
	{
		case 0:	/* 校准触摸时，屏幕方向为0 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			break;

		case 1:	/* 校准触摸时，屏幕方向为1 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			break;

		case 2:	/* 校准触摸时，屏幕方向为2 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			break;

		case 3:	/* 校准触摸时，屏幕方向为3 */
			if (g_LcdDirection == 0)		/* 横屏 */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* 横屏180°*/
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 2)	/* 竖屏 */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 3)	/* 竖屏180° */
			{
				x = xx;
				y = yy;
			}
			break;

		default:
		//	g_tParam.TouchDirection = 0;	/* 方向参数无效时，纠正为缺省的横屏 */
			break;
	}

	g_tTP.XBuf[g_tTP.Write] = x;
	g_tTP.YBuf[g_tTP.Write] = y;

	if (++g_tTP.Write  >= TOUCH_FIFO_SIZE)
	{
		g_tTP.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_GetKey
*	功能说明: 从触摸FIFO缓冲区读取一个坐标值。
*	形    参:  无
*	返 回 值:
*			TOUCH_NONE      表示无事件
*			TOUCH_DOWN      按下
*			TOUCH_MOVE      移动
*			TOUCH_RELEASE	释放
*********************************************************************************************************
*/
uint8_t TOUCH_GetKey(int16_t *_pX, int16_t *_pY)
{
	uint8_t ret;

	if (g_tTP.Read == g_tTP.Write)
	{
		return TOUCH_NONE;
	}
	else
	{
		ret = g_tTP.Event[g_tTP.Read];
		*_pX = g_tTP.XBuf[g_tTP.Read];
		*_pY = g_tTP.YBuf[g_tTP.Read];

		if (++g_tTP.Read >= TOUCH_FIFO_SIZE)
		{
			g_tTP.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_CelarFIFO
*	功能说明: 清除触摸FIFO缓冲区
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_CelarFIFO(void)
{
	//__set_PRIMASK(1);  		/* 关中断 */
	g_tTP.Write = g_tTP.Read;
	//__set_PRIMASK(0);  		/* 开中断 */
}



/*
*********************************************************************************************************
*	函 数 名: TOUCH_Scan
*	功能说明: 触摸板事件检测程序。该函数被周期性调用，每ms调用1次. 见 bsp_Timer.c
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
GUI_PID_STATE State;  
void TOUCH_Scan(void)
{
//	uint16_t usAdcX;
//	uint16_t usAdcY;
//	static uint16_t s_usXBuf[SAMPLE_COUNT];
//	static uint16_t s_usYBuf[SAMPLE_COUNT];
//	static uint8_t s_ucPos = 0;
//	static uint8_t s_count = 0;
	
	
	/* 获得原始的ADC值，未滤波 */
	if (g_ChipID == IC_8875)
	{
		/* 如果主程序正在访问RA8875,则丢弃本次触摸采集，避免影响显示 */
		if (RA8875_IsBusy())
		{
			return;
		}

		//usAdcX = RA8875_TouchReadX();
		//usAdcY = RA8875_TouchReadY();
	}
	
}


/*
*********************************************************************************************************
*	函 数 名: TOUCH_GetX
*	功能说明: 获得当前的的触摸坐标X
*	形    参:  无
*	返 回 值: X 坐标值，允许负值
*********************************************************************************************************
*/
int16_t TOUCH_GetX(void)
{
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_GetY
*	功能说明: 获得当前的的触摸坐标Y
*	形    参:  无
*	返 回 值: Y 坐标值，允许负值
*********************************************************************************************************
*/
int16_t TOUCH_GetY(void)
{
	return 0;
}




