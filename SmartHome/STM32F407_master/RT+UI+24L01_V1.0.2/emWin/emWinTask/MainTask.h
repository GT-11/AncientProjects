/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI����������
*	�ļ����� : MainTask.c
*	��    �� : V1.0
*	˵    �� : GUI����������
*		�汾��   ����         ����            ˵��
*		v1.0    2013-11-17  Eric2013  	 1. ST�̼���V1.0.2�汾
*       v1.1    2014-03-22  Eric2013     1. ����ST�̼��⵽V1.3.0�汾
*                                        2. ���uCOS-II����FPU������
*	    V1.2    2015-03-27   Eric2013    1. �����̼��⵽V1.5.0
*                                        2. ����BSP�弶֧�ְ� 
*                                        3. ����STemWin��5.26
*                                        4. ����Ϊ�µ��ĵ㴥��У׼�㷨�����ز���Ŵ����������
*                                        5. ���7��800*480�ֱ��ʵ�����֧�֣����3.5��480*320��ILI9488֧�֡�
*                                     
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "DIALOG.h"
#ifndef __MainTask_H
#define __MainTask_H
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x03)
#define ID_BUTTON_3 (GUI_ID_USER + 0x04)
#define ID_BUTTON_4 (GUI_ID_USER + 0x05)
#define ID_BUTTON_5 (GUI_ID_USER + 0x06)
#define ID_TEXT_0 (GUI_ID_USER + 0x07)
#define ID_TEXT_1 (GUI_ID_USER + 0x08)
#define ID_TEXT_2 (GUI_ID_USER + 0x09)
#define ID_TEXT_3 (GUI_ID_USER + 0x0A)
#define ID_TEXT_4 (GUI_ID_USER + 0x0B)
#define ID_TEXT_5 (GUI_ID_USER + 0x0C)
#define ID_TEXT_6 (GUI_ID_USER + 0x0D)
#define ID_TEXT_7 (GUI_ID_USER + 0x0E)
#define ID_TEXT_8 (GUI_ID_USER + 0x0F)
#define ID_TEXT_9 (GUI_ID_USER + 0x10)
#define ID_TEXT_10 (GUI_ID_USER + 0x11)
#define ID_TEXT_11 (GUI_ID_USER + 0x12)
WM_HWIN CreateWindow(void);

#endif

/*****************************(END OF FILE) *********************************/
