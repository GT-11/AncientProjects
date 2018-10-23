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





//EventsFunctionList
//EndofEventsFunctionList
extern char Light_VALUE[6];
extern char Send_Flag_Light[6];
extern WM_HWIN _Light;
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

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "Light",             0,                       0,  0,  550,350,FRAMEWIN_CF_MOVEABLE,0},
    { BUTTON_CreateIndirect,    "OK",                GUI_ID_BUTTON0,          197,274,150,34, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER0,          107,12, 420,30, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER1,          107,52, 420,30, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER2,          107,92, 420,30, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER3,          107,132,420,30, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER4,          107,172,420,30, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER5,          107,212,420,30, 0,0},
    { TEXT_CreateIndirect,      "A",                 GUI_ID_TEXT0,            57, 16, 20, 26, 0,0},
    { TEXT_CreateIndirect,      "B",                 GUI_ID_TEXT1,            57, 56, 20, 26, 0,0},
    { TEXT_CreateIndirect,      "C",                 GUI_ID_TEXT2,            57, 96, 20, 26, 0,0},
    { TEXT_CreateIndirect,      "D",                 GUI_ID_TEXT3,            57, 136,20, 26, 0,0},
    { TEXT_CreateIndirect,      "E",                 GUI_ID_TEXT4,            57, 176,20, 26, 0,0},
    { TEXT_CreateIndirect,      "F",                 GUI_ID_TEXT5,            57, 216,20, 26, 0,0}
};



/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

void PaintDialog(WM_MESSAGE * pMsg)
{
//    WM_HWIN hWin = pMsg->hWin;

}



/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/

void InitDialog(WM_MESSAGE * pMsg)
{
	char b[6];
    WM_HWIN hWin = pMsg->hWin;
    //
    //FRAMEWIN
    //
    FRAMEWIN_SetFont(hWin,&GUI_FontComic24B_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMaxButton(hWin, FRAMEWIN_BUTTON_RIGHT, 1);
    FRAMEWIN_AddMinButton(hWin, FRAMEWIN_BUTTON_RIGHT, 2);
    FRAMEWIN_SetTitleHeight(hWin,24);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1),&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2),&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3),&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT4),&GUI_Font24B_ASCII);
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT5),&GUI_Font24B_ASCII);
		b[0]=Light_VALUE[0];
		b[1]=Light_VALUE[1];
		b[2]=Light_VALUE[2];
		b[3]=Light_VALUE[3];
		b[4]=Light_VALUE[4];
		b[5]=Light_VALUE[5];
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0), Light_VALUE[0]);
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER1), Light_VALUE[1]);
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER2), Light_VALUE[2]);
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER3), Light_VALUE[3]);
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER4), Light_VALUE[4]);
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER5), Light_VALUE[5]);
		Light_VALUE[0]=b[0];
		Light_VALUE[1]=b[1];
		Light_VALUE[2]=b[2];
		Light_VALUE[3]=b[3];
		Light_VALUE[4]=b[4];
		Light_VALUE[5]=b[5];
}




/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
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
														GUI_EndDialog(_Light, 0);
														_Light=NULL;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												Light_VALUE[0]=SLIDER_GetValue(WM_GetDialogItem(_Light,GUI_ID_SLIDER0));
												Send_Flag_Light[0]=1;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER1:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												Light_VALUE[1]=SLIDER_GetValue(WM_GetDialogItem(_Light,GUI_ID_SLIDER1));
												Send_Flag_Light[1]=1;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER2:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												Light_VALUE[2]=SLIDER_GetValue(WM_GetDialogItem(_Light,GUI_ID_SLIDER2));
												Send_Flag_Light[2]=1;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER3:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												Light_VALUE[3]=SLIDER_GetValue(WM_GetDialogItem(_Light,GUI_ID_SLIDER3));
												Send_Flag_Light[3]=1;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER4:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												Light_VALUE[4]=SLIDER_GetValue(WM_GetDialogItem(_Light,GUI_ID_SLIDER4));
												Send_Flag_Light[4]=1;
                            break;
                    }
                    break;
                case GUI_ID_SLIDER5:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                            //OnSliderValueChanged(pMsg);
												Light_VALUE[5]=SLIDER_GetValue(WM_GetDialogItem(_Light,GUI_ID_SLIDER5));
												Send_Flag_Light[5]=1;
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


WM_HWIN Light(void) 
{ 
        return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

