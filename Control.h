

// CONTROL.h
#ifndef _CONTROL_H_
#define _CONTROL_H_    //#endif  // _CONTROL_H_ //

// 结构仅在 CONTROL.CPP 中实现
#ifdef  _CONTROL_CPP_    //#endif  // _CONTROL_CPP_ //
#define CONTROL_EXTERN 
#else 
#define CONTROL_EXTERN extern 
#endif  // _CONTROL_CPP_ //


#include <Windows.h>

HWND CreateControl(HWND hWnd, LPCTSTR lpClassName="Edit",  const int id =1 , RECT* rp=0L , int sty=0 );      



#if WC_LISTVIEW_MGR|1 

int AddListViewColumns(HWND hListView, const wchar_t* szHeadings);
int AddListViewItems(HWND hListView, const wchar_t* szItem);





#endif  // WC_LISTVIEW_MGR




#if TOOLTIPS_CLASS_MGR|1

bool AddTool(HWND hWnd, int uID, HWND hTooltip, LPSTR wtip);

void UpdateTip(HWND hWnd, UINT uID, HWND hTooltip, LPWSTR newTip); 
void UpdateTip(HWND hWnd, UINT uID, HWND hTooltip, char* newTip); 



#endif  // TOOLTIPS_CLASS_MGR




#endif  // _CONTROL_H_ //


