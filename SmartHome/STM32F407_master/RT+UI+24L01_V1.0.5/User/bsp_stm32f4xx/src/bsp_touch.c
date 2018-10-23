/*
*********************************************************************************************************
*
*	ģ������ : ����ʽ����������ģ��
*	�ļ����� : bsp_touch.c
*	��    �� : V1.7
*	˵    �� : ����TS2046оƬ �� RA8875���ô���
*	�޸ļ�¼ :
*		�汾��  ����        ����    ˵��
*       v1.0    2012-07-06 armfly  ST�̼���V3.5.0�汾��
*		v1.1    2012-10-22 armfly  ����4��У׼
*		v1.2    2012-11-07 armfly  ���4��У׼��XY������֧��bug
*		v1.3    2012-12-17 armfly  ����У׼����������ڲ���:�ȴ�ʱ��
*		V1.4    2013-07-26 armfly  ���� TOUCH_DataFilter() �˲��㷨
*		V1.5    2013-07-32 armfly  �޸�TOUCH_WaitRelease(),��������Ҫ����
*		V1.6    2014-10-20 armfly
*					(1) �޸� TOUCH_PutKey() ������ʵ�ִ������ĺ�����������̬�л�.
*					(2) param �ṹ����У׼ʱ��ǰ����Ļ������� TouchDirection
*					(3) ����3.5��Ĵ���оƬ���޸�SPI������ú�����
*					(4) ���ڴ���оƬTSC2046�ʹ���FLASH,NRF24L01,MP3��ģ�鹲��SPI���ߡ������Ҫ
*						�ڴ����жϺ������ж����߳�ͻ. ���Ӻ��� bsp_SpiBusBusy() ��æ.
*					(5) TSC2046�������ģ��SPI (���ģ�ⷽʽ����SPI�豸����)
*		V1.7    2015-01-02 armfly  �ƻ�������ɨ����1msһ���޸�Ϊ10msһ�Ρ�δ����
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
//#include "param.h"	/* ���������洢ģ�� */
#include "GUI.h"

/*
��1��������STM32-V5������ + 3.5����ʾģ�飬 ��ʾģ���ϵĴ���оƬΪ TSC2046�������оƬ
	PI10   --> TP_CS
	PD3/LCD_BUSY   --> TP_BUSY
	PB3/SPI3_SCK   --> TP_SCK
	PB4/SPI3_MISO   --> TP_MISO
	PB5/SPI3_MOSI   --> TP_MOSI
	PI3/TP_INT   --> TP_PEN_INT

��2��������STM32������ + 4.3���7����ʾģ�飨����RA8875оƬ)
	RA8875���ô������ӿڣ����ֱ��ͨ��FSMC����RA8875��ؼĴ������ɡ�


	������δʹ�ô����жϹ��ܡ���1ms���ڵ� Systick��ʱ�жϷ�������жԴ���ADCֵ���в�����
	�˲����������ʰ��³���40ms�󣬿�ʼ�ɼ�ADCֵ��ÿ1ms�ɼ�1�Σ������ɼ�10�Σ���Ȼ���
	��10�������������򣬶���2ͷ�����������м�һ������������Ͳ�����ƽ��ֵ��

	����2��У׼������У׼�󱣴�2��У׼���ADCֵ��ʵ�ʹ���ʱ������2��ֱ�߷��̼�����Ļ���ꡣ
	У׼�����б���ӿڣ���������Ҫ������ʾ��δ�����湦�ܡ�
	��ҿ����Լ��޸�  TOUCH_SaveParam() �� TOUCH_LoadParam() ��������ʵ�ֱ��湦�ܡ�

*/
/* TSC2046 �ڲ�ADCͨ���� */
#define ADC_CH_X	1		/* Xͨ��������Xλ�� */
#define ADC_CH_Y	5		/* Yͨ��������Yλ�� */

/* ÿ1msɨ��һ������ */
#define DOWN_VALID		30	/* ����30ms ��, ��ʼͳ��ADC */
#define SAMPLE_COUNT	10	/* ���º������ɼ�40������ */

/*
������У׼�������Ļ�����Ľǵ�ƫ������
��1���� �� x1 = CALIB_OFFSET, y1 = CALIB_OFFSET
��2���� �� x2 = LCD_GetWidth() - CALIB_OFFSET, y2 = LCD_GetHeight() - CALIB_OFFSET
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

/* ��ЧADCֵ���ж�����. ̫�ӽ�ADC�ٽ�ֵ��������Ϊ��Ч */
#define ADC_VALID_OFFSET	2

//#define WaitTPReady() while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))
#define WaitTPReady() {}

/* ����ģ���õ���ȫ�ֱ��� */
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
*	�� �� ��: bsp_InitTouch
*	����˵��: ����STM32�ʹ�����صĿ��ߣ�ʹ��Ӳ��SPI1, Ƭѡ���������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TOUCH_InitHard(void)
{
	uint16_t i;
	
    g_tTP.Enable = 0;

	/* 50ms���ȴ�GT811��λ����������̽��GT811оƬ ID */
	for (i = 0; i < 5; i++)
	{
		if (i2c_CheckDevice(GT811_I2C_ADDR) == 0)
		{
			GT811_InitHard();
			return;
		}
		bsp_DelayUS(10000);
	}
		
//	TOUCH_LoadParam();	/* ��ȡУ׼���� */

    g_tTP.Write = g_tTP.Read = 0;

	g_tTP.Enable = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_ReadAdcX
*	����˵��: ��ô�����X����ADC����ֵ�� �ѽ����˲�����
*	��    ��:  ��
*	�� �� ֵ: X ����ADCֵ
*********************************************************************************************************
*/
uint16_t TOUCH_ReadAdcX(void)
{
	uint16_t usAdc;

	//__set_PRIMASK(1);  		/* ���ж� */
	usAdc = g_tTP.usAdcNowX;
	//__set_PRIMASK(0);  		/* ���ж� */

	return usAdc;
}

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_ReadAdcY
*	����˵��: ��ô�����Y����ADC����ֵ�� �ѽ����˲�����
*	��    ��:  ��
*	�� �� ֵ: Y ����ֵ������ֵ
*********************************************************************************************************
*/
uint16_t TOUCH_ReadAdcY(void)
{
	uint16_t usAdc;

	//__set_PRIMASK(1);  		/* ���ж� */
	usAdc = g_tTP.usAdcNowY;
	//__set_PRIMASK(0);  		/* ���ж� */

	return usAdc;
}

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_PutKey
*	����˵��: ��1������������ֵѹ�봥��FIFO�����������ڵ��败������
*	��    ��: _usX, _usY ����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TOUCH_PutKey(uint8_t _ucEvent, uint16_t _usX, uint16_t _usY)
{
	uint16_t xx, yy;
	uint16_t x = 0, y = 0;

	g_tTP.Event[g_tTP.Write] = _ucEvent;

	if (g_GT811.Enable == 1)	/* ������ (����У׼) */
	{
		xx = _usX;
		yy = _usY;
	}
	else	/* ������ */
	{
//		xx = TOUCH_TransX(_usX, _usY);
//		yy = TOUCH_TransY(_usX, _usY);
	}
	
	/* ��������������ʶ�� */
	switch (0)
	{
		case 0:	/* У׼����ʱ����Ļ����Ϊ0 */
			if (g_LcdDirection == 0)		/* ���� */
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 1)	/* ����180��*/
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 2)	/* ���� */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 3)	/* ����180�� */
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			break;

		case 1:	/* У׼����ʱ����Ļ����Ϊ1 */
			if (g_LcdDirection == 0)		/* ���� */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* ����180��*/
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 2)	/* ���� */
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 3)	/* ����180�� */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			break;

		case 2:	/* У׼����ʱ����Ļ����Ϊ2 */
			if (g_LcdDirection == 0)		/* ���� */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* ����180��*/
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 2)	/* ���� */
			{
				x = xx;
				y = yy;
			}
			else if (g_LcdDirection == 3)	/* ����180�� */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			break;

		case 3:	/* У׼����ʱ����Ļ����Ϊ3 */
			if (g_LcdDirection == 0)		/* ���� */
			{
				y = xx;
				x = g_LcdWidth - yy - 1;
			}
			else if (g_LcdDirection == 1)	/* ����180��*/
			{
				y = g_LcdHeight - xx - 1;
				x = yy;
			}
			else if (g_LcdDirection == 2)	/* ���� */
			{
				x = g_LcdWidth - xx - 1;
				y = g_LcdHeight - yy - 1;
			}
			else if (g_LcdDirection == 3)	/* ����180�� */
			{
				x = xx;
				y = yy;
			}
			break;

		default:
		//	g_tParam.TouchDirection = 0;	/* ���������Чʱ������Ϊȱʡ�ĺ��� */
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
*	�� �� ��: TOUCH_GetKey
*	����˵��: �Ӵ���FIFO��������ȡһ������ֵ��
*	��    ��:  ��
*	�� �� ֵ:
*			TOUCH_NONE      ��ʾ���¼�
*			TOUCH_DOWN      ����
*			TOUCH_MOVE      �ƶ�
*			TOUCH_RELEASE	�ͷ�
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
*	�� �� ��: TOUCH_CelarFIFO
*	����˵��: �������FIFO������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TOUCH_CelarFIFO(void)
{
	//__set_PRIMASK(1);  		/* ���ж� */
	g_tTP.Write = g_tTP.Read;
	//__set_PRIMASK(0);  		/* ���ж� */
}



/*
*********************************************************************************************************
*	�� �� ��: TOUCH_Scan
*	����˵��: �������¼������򡣸ú����������Ե��ã�ÿms����1��. �� bsp_Timer.c
*	��    ��:  ��
*	�� �� ֵ: ��
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
	
	
	/* ���ԭʼ��ADCֵ��δ�˲� */
	if (g_ChipID == IC_8875)
	{
		/* ������������ڷ���RA8875,�������δ����ɼ�������Ӱ����ʾ */
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
*	�� �� ��: TOUCH_GetX
*	����˵��: ��õ�ǰ�ĵĴ�������X
*	��    ��:  ��
*	�� �� ֵ: X ����ֵ������ֵ
*********************************************************************************************************
*/
int16_t TOUCH_GetX(void)
{
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_GetY
*	����˵��: ��õ�ǰ�ĵĴ�������Y
*	��    ��:  ��
*	�� �� ֵ: Y ����ֵ������ֵ
*********************************************************************************************************
*/
int16_t TOUCH_GetY(void)
{
	return 0;
}




