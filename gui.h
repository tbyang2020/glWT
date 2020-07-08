

// GUI.h
#ifndef _GUI_H_
#define _GUI_H_    //#endif  // _GUI_H_ //

// 结构仅在 GUI.CPP 中实现
#ifdef  _GUI_CPP_    //#endif  // _GUI_CPP_ //
#define GUI_EXTERN 
#else 
#define GUI_EXTERN extern 
#endif  // _GUI_CPP_ //



#include <Windows.h>



#if M_TEST

GUI_EXTERN HWND hdlg_op
#ifdef  _GUI_CPP_  
=0L;
#else
;
#endif  

#endif  // M_TEST


enum PANEL_NODE {

    IDC_PNTOOLTIP =0x101, 


    IDC_PNIMAGE_BUTTON,
    IDC_PNIMAGE_COMBO,

    IDC_PNDATA_BUTTON,
    IDC_PNDATA_COMBO,

    IDC_PNCMDINPUT_EDIT,
    IDC_PNCMD_EDIT,

};





HWND AddPanel(HWND hWnd);


// bool GetFileName(LPWSTR szFile, bool bSave, HWND hWndL);  // InterfaceMDI.cpp 




bool AddTool(HWND hWnd, int uID, HWND hTooltip, LPSTR wtip);


int CreatePanel(HWND hWnd);

int UpdateCmd(HWND hWnd);

int MakeCmd(HWND hdlg); 















#endif  // _GUI_H_ //


