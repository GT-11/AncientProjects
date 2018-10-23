/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : GUI界面主函数
*		版本号   日期         作者            说明
*		v1.0    2013-11-17  Eric2013  	 1. ST固件库V1.0.2版本
*       v1.1    2014-03-22  Eric2013     1. 升级ST固件库到V1.3.0版本
*                                        2. 解决uCOS-II开启FPU的问题
*	    V1.2    2015-03-27   Eric2013    1. 升级固件库到V1.5.0
*                                        2. 升级BSP板级支持包 
*                                        3. 升级STemWin到5.26
*                                        4. 更改为新的四点触摸校准算法，并重查编排触摸检测任务
*                                        5. 添加7寸800*480分辨率电容屏支持，添加3.5寸480*320的ILI9488支持。
*                                     
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
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
