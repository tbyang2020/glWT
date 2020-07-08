


//InterfaceMDI.h
#ifndef _INTERFACEMDI_H_
#define _INTERFACEMDI_H_    //#endif  // _INTERFACEMDI_H_ //

//结构仅在INTERFACEMDI.CPP中实现
#ifdef  _INTERFACEMDI_CPP_    //#endif  // _INTERFACEMDI_CPP_ //
#define INTERFACEMDI_EXTERN 
#else 
#define INTERFACEMDI_EXTERN extern 
#endif  // _INTERFACEMDI_CPP_ //



INTERFACEMDI_EXTERN HWND g_hGL;  



#include "res/resource.h"

#define MARGIN 5
#define XMARGIN 5
#define YMARGIN 5



#define WS_ISOLATE  (WS_POPUPWINDOW|WS_VISIBLE|WS_CLIPSIBLINGS|WS_DLGFRAME|WS_THICKFRAME|WS_OVERLAPPED   |WS_MAXIMIZEBOX)
#define WS_EMBED    (WS_CHILDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN|WS_BORDER|WS_OVERLAPPED  |WS_SIZEBOX) 

#define WS_EX_ISOLATE  (WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR) 
#define WS_EX_EMBED    (WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_TOOLWINDOW|WS_EX_WINDOWEDGE) 



#define HWND_MESSAGE ((HWND)-3) 


#define WNDCLS_FRAME     L"MDIFrameClass"     // 主界面窗口类,  WndProc. 
#define WNDCLS_CLIENT    L"MDIClientChild"    // ClientChild 窗口, ClientChildProc.  "MDICLIENT" 是系统类名, MDIClient 窗口 


// Client Child Window type  窗口显示特点  // Client ( Child) Window ID = DATA_TYPE   

enum DATA_TYPE{

DT_OG      = 0x101,    // Opengl 渲染 --- 确保窗口唯一, ID与 DATA_TYPE 相同  

ID_CLIENT  = 0x102,    // "MDICLIENT"  窗口  ID  
IDT_NEW,              
ID_OPEN, 


DT_DIB,        // 显示位图文件
DT_TEXT,       // 显示文本 
DT_GRAH,       // GDI 绘图 


};



// Client Child Window Data 

class CCW_DATA{  
public:

int DT_Type;     //  enum DT_..., 决定出口绘制方式   
void* pdata;     // 数据, 如 LPBYTE, DIB 指针等等, 与 DT_TYPE 有关.   


POINT pt={-1,-1};        // 鼠标最后点击的位置. Track cursor  


// 一下数据仅备用 ...... 



#if MORE_APPLICATION |0  

XFORM xf={1,0,  0,1,  0,0,};        // 6 个 int,  窗口私有的 World Transform. 记录鼠标移动、滚动. [1,0, 0,1, 0,0]   


#if IMAGE_PROCESS



RECT sel;        // selection, [0,0,0,0] 表示用户区:  WM_LBUTTONDOWN 影响左上角, 鼠标拖动+右键更改右下角. 
RECT bound;      // 指定颜色范围的确界. 
RECT hline;      // 指定颜色范围的水平直线. 
RECT vline;      // 指定颜色范围的竖直直线. 

LPVOID pSrc;     // 原始数据. 例如原始图像指针. 不负责申请、释放内存. 

#endif  // IMAGE_PROCESS 


public:

CCW_DATA(){ xf={1,0,  0,1,  0,0,};   pt={0,0}; pdata=0; pSrc=0;   
sel={0,0,0,0}; bound={0,0,0,0}; hline={0,0,0,0}; vline={0,0,0,0};
DT_Type=0; 
}   // 

~CCW_DATA(){  if(pdata){ free(pdata); pdata=0; } }  // virtual -- 子类可以 override 

#endif  // MORE_APPLICATION
 
};





typedef 
//INTERFACEMDI_EXTERN 
struct CHILDDATA  //文档--Client窗口的Child窗口,的数据//CreateData;
{
//数据[文件]类型,缺省为WINDOW_FILE,也可是由CREATEDATA_TYPES确定的已知类型; //Data Type; //Necessary;
//数据标识;
int iWindow;            // 窗口类型, 包括iDataType的含义; //在CreateView完成p的初始化之后赋值; //Important;
WORD szTitle[MAX_PATH];  //数据或文件的名称;  

LPVOID p; //数据[文件]内容,Buffer pointer; //To be read, or appointed; //必须指向一个全局内存区; 

//辅助成员变量;
int iFileType; //仅当iWindow=WINDOW_FILE时有效, 文件类型, 在CreateView中初始化;




/*
HWND hwnd;      //Handle of This Window;
HWND hcrtl[4];  //0,1; 2,3;

//controls required;

HWND hcontrol;  //Controls in the window, Not Necessary;
HWND hproperty; //Property Window, Not Necessary;
HWND hoption;   //Options Window, Not Necessary;
*/

}*PCHILDDATA, *LPCHILDDATA;


INTERFACEMDI_EXTERN HWND hClientWnd;
INTERFACEMDI_EXTERN HWND hArabic;
INTERFACEMDI_EXTERN HWND hEnglish;
INTERFACEMDI_EXTERN HWND hTriLinguo;
INTERFACEMDI_EXTERN HWND hToolTip;

INTERFACEMDI_EXTERN WORD szModulePath[MAX_PATH];







INTERFACEMDI_EXTERN bool bRichEditLoaded
#ifdef _INTERFACEMDI_CPP_
=false;
#else
;
#endif



 


////////////////////  [2006年3月13日HostFrameManipulations] ////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK ClientChildProc(HWND hWnd, UINT Message,WPARAM wParam,LPARAM lParam );
LRESULT CALLBACK ClientChild_GraphProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );

INT_PTR CALLBACK InputProc(HWND hdlg,UINT uMsg, WPARAM wParam,LPARAM lParam);
LRESULT APIENTRY InterceptClient(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);  







//// HWND CreateMDIClientWindow(HWND hParent);//To be edited!


LRESULT CALLBACK ClientChildProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);




//   [2006年3月13日ClientManipulations] //

void TrackMenu(HMENU hmenu,HWND hwnd);









int glTextOut(float x, float y, const wchar_t  *fmt, ...); 
int glTextOut(float x, float y, float z,  const wchar_t  *fmt, ...); 





inline    // 不影响 
int t_inline(){ static int ti=0; ti++;  return ti; }






#endif  // _INTERFACEMDI_H_ //