


//InterfaceMDI.h
#ifndef _INTERFACEMDI_H_
#define _INTERFACEMDI_H_    //#endif  // _INTERFACEMDI_H_ //

//�ṹ����INTERFACEMDI.CPP��ʵ��
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


#define WNDCLS_FRAME     L"MDIFrameClass"     // �����洰����,  WndProc. 
#define WNDCLS_CLIENT    L"MDIClientChild"    // ClientChild ����, ClientChildProc.  "MDICLIENT" ��ϵͳ����, MDIClient ���� 


// Client Child Window type  ������ʾ�ص�  // Client ( Child) Window ID = DATA_TYPE   

enum DATA_TYPE{

DT_OG      = 0x101,    // Opengl ��Ⱦ --- ȷ������Ψһ, ID�� DATA_TYPE ��ͬ  

ID_CLIENT  = 0x102,    // "MDICLIENT"  ����  ID  
IDT_NEW,              
ID_OPEN, 


DT_DIB,        // ��ʾλͼ�ļ�
DT_TEXT,       // ��ʾ�ı� 
DT_GRAH,       // GDI ��ͼ 


};



// Client Child Window Data 

class CCW_DATA{  
public:

int DT_Type;     //  enum DT_..., �������ڻ��Ʒ�ʽ   
void* pdata;     // ����, �� LPBYTE, DIB ָ��ȵ�, �� DT_TYPE �й�.   


POINT pt={-1,-1};        // ����������λ��. Track cursor  


// һ�����ݽ����� ...... 



#if MORE_APPLICATION |0  

XFORM xf={1,0,  0,1,  0,0,};        // 6 �� int,  ����˽�е� World Transform. ��¼����ƶ�������. [1,0, 0,1, 0,0]   


#if IMAGE_PROCESS



RECT sel;        // selection, [0,0,0,0] ��ʾ�û���:  WM_LBUTTONDOWN Ӱ�����Ͻ�, ����϶�+�Ҽ��������½�. 
RECT bound;      // ָ����ɫ��Χ��ȷ��. 
RECT hline;      // ָ����ɫ��Χ��ˮƽֱ��. 
RECT vline;      // ָ����ɫ��Χ����ֱֱ��. 

LPVOID pSrc;     // ԭʼ����. ����ԭʼͼ��ָ��. ���������롢�ͷ��ڴ�. 

#endif  // IMAGE_PROCESS 


public:

CCW_DATA(){ xf={1,0,  0,1,  0,0,};   pt={0,0}; pdata=0; pSrc=0;   
sel={0,0,0,0}; bound={0,0,0,0}; hline={0,0,0,0}; vline={0,0,0,0};
DT_Type=0; 
}   // 

~CCW_DATA(){  if(pdata){ free(pdata); pdata=0; } }  // virtual -- ������� override 

#endif  // MORE_APPLICATION
 
};





typedef 
//INTERFACEMDI_EXTERN 
struct CHILDDATA  //�ĵ�--Client���ڵ�Child����,������//CreateData;
{
//����[�ļ�]����,ȱʡΪWINDOW_FILE,Ҳ������CREATEDATA_TYPESȷ������֪����; //Data Type; //Necessary;
//���ݱ�ʶ;
int iWindow;            // ��������, ����iDataType�ĺ���; //��CreateView���p�ĳ�ʼ��֮��ֵ; //Important;
WORD szTitle[MAX_PATH];  //���ݻ��ļ�������;  

LPVOID p; //����[�ļ�]����,Buffer pointer; //To be read, or appointed; //����ָ��һ��ȫ���ڴ���; 

//������Ա����;
int iFileType; //����iWindow=WINDOW_FILEʱ��Ч, �ļ�����, ��CreateView�г�ʼ��;




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



 


////////////////////  [2006��3��13��HostFrameManipulations] ////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK ClientChildProc(HWND hWnd, UINT Message,WPARAM wParam,LPARAM lParam );
LRESULT CALLBACK ClientChild_GraphProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );

INT_PTR CALLBACK InputProc(HWND hdlg,UINT uMsg, WPARAM wParam,LPARAM lParam);
LRESULT APIENTRY InterceptClient(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);  







//// HWND CreateMDIClientWindow(HWND hParent);//To be edited!


LRESULT CALLBACK ClientChildProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);




//   [2006��3��13��ClientManipulations] //

void TrackMenu(HMENU hmenu,HWND hwnd);









int glTextOut(float x, float y, const wchar_t  *fmt, ...); 
int glTextOut(float x, float y, float z,  const wchar_t  *fmt, ...); 





inline    // ��Ӱ�� 
int t_inline(){ static int ti=0; ti++;  return ti; }






#endif  // _INTERFACEMDI_H_ //