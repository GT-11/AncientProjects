/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"

extern WM_HWIN AIRB;
extern char AIR_B_VALUE;
extern char Send_Flag_AIRB;
extern unsigned char AIR_B_MODE;

//EventsFunctionList
void OnSliderValueChanged(WM_MESSAGE * pMsg);
void OnButtonClicked(WM_MESSAGE * pMsg);
//EndofEventsFunctionList


/*********************************************************************
*
*       static data
*
**********************************************************************
*/



/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate_B[] = {
    { FRAMEWIN_CreateIndirect,  "Air_B",             0,                       0,  0,  493,280,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "OK",                GUI_ID_BUTTON0,          97, 199,87, 31, 0,0},
    { BUTTON_CreateIndirect,    "Cancel",            GUI_ID_BUTTON1,          303,199,87, 31, 0,0},
    { BUTTON_CreateIndirect,    "ON",                GUI_ID_BUTTON2,          356,41, 80, 30, 0,0},
    { BUTTON_CreateIndirect,    "OFF",               GUI_ID_BUTTON3,          356,112,80, 30, 0,0},
    { BUTTON_CreateIndirect,    "COOL",              GUI_ID_BUTTON4,          37, 27, 80, 30, 0,0},
    { BUTTON_CreateIndirect,    "HOT",               GUI_ID_BUTTON5,          37, 87, 80, 30, 0,0},
    { BUTTON_CreateIndirect,    "WET",               GUI_ID_BUTTON6,          37, 147,80, 30, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER0,          133,150,219,27, 0,0},
    { TEXT_CreateIndirect,      "24",                GUI_ID_TEXT0,            172,41, 132,91, 0,0}
};



/*****************************************************************
**      FunctionName:void PaintDialog_B(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback_B --> WM_PAINT
*****************************************************************/

void PaintDialog_B(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;

}



/*****************************************************************
**      FunctionName:void InitDialog_B(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback_B --> WM_INIT_DIALOG
*****************************************************************/

void InitDialog_B(WM_MESSAGE * pMsg)
{
		char a[2],b;
    WM_HWIN hWin = pMsg->hWin;
    //
    //FRAMEWIN
    //
    FRAMEWIN_SetBarColor(hWin,1,0x2a2aa5);
    FRAMEWIN_SetFont(hWin,&GUI_FontComic24B_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_TOP|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMaxButton(hWin, FRAMEWIN_BUTTON_RIGHT, 1);
    FRAMEWIN_AddMinButton(hWin, FRAMEWIN_BUTTON_RIGHT, 2);
    FRAMEWIN_SetTitleHeight(hWin,24);
    //
    //GUI_ID_TEXT0
    //
	TEXT_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),0x00ff00);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_FontD80);
			b=AIR_B_VALUE;
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), (AIR_B_VALUE-16)*100/16);
		a[0]=(b/10)+0x30;
		a[1]=(b%10)+0x30;
		TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), a);
		AIR_B_VALUE=b;
}




/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback_B(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
			char a[2];
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog_B(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog_B(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_BUTTON0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
														Send_Flag_AIRB=1;
                            break;
                    }
                    break;
                case GUI_ID_BUTTON1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
												GUI_EndDialog(AIRB, 0);
														AIRB=NULL;
                            break;
                    }
                    break;
                case GUI_ID_BUTTON2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
														Send_Flag_AIRB=1;
														AIR_B_MODE|=0x80;
                            break;
                    }
                    break;
                case GUI_ID_BUTTON3:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
														Send_Flag_AIRB=1;
														AIR_B_MODE&=~0x80;
                            break;
                    }
                    break;
                case GUI_ID_BUTTON4:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
														Send_Flag_AIRB=1;
														AIR_B_MODE&=~0x70;
														AIR_B_MODE|=0x10;
                            break;
                    }
                    break;
                case GUI_ID_BUTTON5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
														Send_Flag_AIRB=1;
														AIR_B_MODE&=~0x70;
														AIR_B_MODE|=0x40;
                            break;
                    }
                    break;
                case GUI_ID_BUTTON6:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_RELEASED:
                            //OnButtonClicked(pMsg);
														Send_Flag_AIRB=1;
														AIR_B_MODE&=~0x70;
														AIR_B_MODE|=0x20;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												AIR_B_VALUE=(SLIDER_GetValue(WM_GetDialogItem(AIRB,GUI_ID_SLIDER0))*16/100)+16;
												a[0]=(AIR_B_VALUE/10)+0x30;
												a[1]=(AIR_B_VALUE%10)+0x30;
												TEXT_SetText(WM_GetDialogItem(AIRB,GUI_ID_TEXT0), a);
												
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
}


/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
WM_HWIN AIR_B(void) 
{ 
        return GUI_CreateDialogBox(_aDialogCreate_B, GUI_COUNTOF(_aDialogCreate_B), &_cbCallback_B, 0, 0, 0);
}

