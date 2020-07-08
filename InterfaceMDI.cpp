



//  InterfaceMDI.cpp �ṩ���ĵ�����, ʵ�ֶ��ĵ������������  // 2006��3��13�� --  2018/7/15
  

#pragma warning(disable:4996)



#define _INTERFACEMDI_CPP_

#include <Windows.h>
#include <math.h>  //for sin ... 

#include "res/resource.h"

#include "InterfaceMDI.h"
#include "qTrace.h"






#define  MDI_3DTEST   




#include <gl/GL.h>  
#include <gl/GLU.h>

#pragma comment(lib,"Opengl32.lib")            // __imp__glBegin, __imp__glBindTexture, __imp__glClearColor ... glDevice.cpp 
#pragma comment(lib, "glu32.lib")              // _gluLookAt, _gluPerspective, _gluUnProject ... glInterface.cpp  


#ifdef  MDI_3DTEST 

#include "glFont2021/glFont2021.h"

#include "Matrix.h"
#include "glWT.h"

#else  


#include "Colobot/engine/App.h"




#endif  // MDI_3DTEST






#define BORDER 12 

#define WM_DRAW  (WM_USER+0x101)   // �Ա㲻�ػ� "MDICLIENT" �Ĵ��ڹ���  


HWND&getWnd(){static HWND g_hWnd; return g_hWnd; }   // setWnd() ...  



#if WIN32_BASICS|1




bool GetFileName(LPWSTR szFile, bool bSave=FALSE, HWND hWnd=0L){ 

wchar_t*pFilter= new wchar_t[MAX_PATH]; 

swprintf(pFilter, 
L"All Files (*.*)|*.*|"
// L"All Files (*.*)|*.*|����(*.exe;*.fon)|*.exe;*.fon|Text Files(*.txt;*.doc;*.rtf)|*.txt;*.doc;*.rtf|Icon Files (*.ICO)|*.ICO|Bitmap Files (*.bmp)|*.bmp|" 
// Text Files(*.txt;*.doc);*.rtf|*.txt;*.doc;*.rtf|Icon Files (*.ICO)|*.ICO|All Files (*.*)|*.*|
// Text Files(*.txt;*.doc;*.rtf)|*.txt;*.doc;*.rtf|
// Icon Files (*.ICO)|*.ICO|
// Bitmap Files (*.bmp)|*.bmp|
);

wchar_t*p=pFilter;  while(*p!=0){ if(*p=='|') *p=0; p++; }   // �滻'|'Ϊ'\0';
  
//                     0            1   2     3      4 5 6    7       8   9  10  11    12 
OPENFILENAMEW ofn = {sizeof(ofn), hWnd, 0,  pFilter, 0,0,0,  szFile, 256, 0, 0,   0L,   L"���ļ�", 
  OFN_FILEMUSTEXIST,   // |OFN_ENABLESIZING |OFN_EXPLORER |OFN_SHOWHELP  |OFN_SHAREAWARE,  
  0,0,0,0,0,0,0,0,0};  // 9 �� 0 

bool bok = false; 

*szFile = 0;  // ����  
// bool bok=GetOpenFileNameW(&ofn);

if(bSave){
  ofn.Flags|=OFN_OVERWRITEPROMPT;
 // ofn.lpstrDefExt =NULL;  // �������չ�� 
  ofn.lpstrTitle =L"�����ļ�";
  bok=GetSaveFileNameW(&ofn);
}else{
  ofn.Flags|= OFN_FILEMUSTEXIST|OFN_SHAREAWARE;  // OFN_ALLOWMULTISELECT; //
//  ofn.lpstrTitle =L"���ļ�";  
  bok=GetOpenFileNameW(&ofn);
}


delete[] pFilter;

return bok;
}


bool GetFileName(LPTSTR szFile, bool bSave=FALSE, LPCTSTR pFilter=0L, HWND hWnd=0L){ 

char*szFilter= new char[MAX_PATH]; 
if(pFilter) strcpy(szFilter,pFilter);  
else 
sprintf(szFilter, 
"All Files (*.*)|*.*|"
// L"All Files (*.*)|*.*|����(*.exe;*.fon)|*.exe;*.fon|Text Files(*.txt;*.doc;*.rtf)|*.txt;*.doc;*.rtf|Icon Files (*.ICO)|*.ICO|Bitmap Files (*.bmp)|*.bmp|" 
// Text Files(*.txt;*.doc);*.rtf|*.txt;*.doc;*.rtf|Icon Files (*.ICO)|*.ICO|All Files (*.*)|*.*|
// Text Files(*.txt;*.doc;*.rtf)|*.txt;*.doc;*.rtf|
// Icon Files (*.ICO)|*.ICO|
// Bitmap Files (*.bmp)|*.bmp|
);

char*p=szFilter;  while(*p!=0){ if(*p=='|') *p=0; p++; }   // �滻'|'Ϊ'\0';
  
//                     0            1   2     3      4 5 6    7       8   9  10  11    12 
OPENFILENAME ofn = {sizeof(ofn), hWnd, 0,  pFilter, 0,0,0,  szFile, 256, 0, 0,   0L,   "���ļ�", 
  OFN_FILEMUSTEXIST,   // |OFN_ENABLESIZING |OFN_EXPLORER |OFN_SHOWHELP  |OFN_SHAREAWARE,  
  0,0,0,0,0,0,0,0,0};  // 9 �� 0 

bool bok = false; 

*szFile = 0;  // ����  

if(bSave){
  ofn.Flags|=OFN_OVERWRITEPROMPT;
 // ofn.lpstrDefExt =NULL;  // �������չ�� 
  ofn.lpstrTitle ="�����ļ�";
  bok=GetSaveFileName(&ofn);
}else{
  ofn.Flags|= OFN_FILEMUSTEXIST|OFN_SHAREAWARE;  // OFN_ALLOWMULTISELECT; //
//  ofn.lpstrTitle =L"���ļ�";  
  bok=GetOpenFileName(&ofn);
}


delete[] pFilter;

return bok;
}




// TrackMenu, ��ʾ�˵�. �������� 
void TrackMenu(HMENU hmenu,HWND hdlg){ 
POINT pt; GetCursorPos(&pt);  
// TPMPARAMS arp;arp.cbSize=sizeof(TPMPARAMS);  arp.rcExclude.left=0;  arp.rcExclude.top=0; arp.rcExclude.right=80;  arp.rcExclude.bottom=80;
TPMPARAMS tpm={sizeof(tpm),}; 
tpm.rcExclude.left=0,tpm.rcExclude.top=0,tpm.rcExclude.right=300,tpm.rcExclude.bottom=300; // screen area not to overlap       
UINT uFlag=TPM_VCENTERALIGN  //|TPM_TOPALIGN
      |TPM_LEFTBUTTON        //|TPM_NOANIMATION|TPM_VERPOSANIMATION //Win98,Win2000
      |TPM_HORIZONTAL;       //|TPM_NONOTIFY//|TPM_RETURNCMD //��ֱ����Ӧ���ѡ��
TrackPopupMenuEx(hmenu,uFlag,pt.x,pt.y,hdlg,&tpm);
}


// ָ������͸����.  int da Ϊ͸�������� ��1, ��2, ...  
void SetTransparence(HWND hWnd, int da){  // =-1

  static int ao = 255. *80/100;   // %80 ��͸��  %20 ͸��  
  //if(0 <= a && a < 256) ao = a; else ao++;

  ao += da;  

  if(ao < 25) ao = 25;    // ���� 10% ��͸�� 
  if(ao > 255) ao = 255;  // ���ײ�͸��
  
  SetLayeredWindowAttributes(hWnd, 0, ao, LWA_ALPHA); 
}


// ���� Scroll Pos 
void drawScroll(HDC hdc){

HWND hWnd = WindowFromDC(hdc); 
int cx = GetSystemMetrics(SM_CXVSCROLL); 
int cy = GetSystemMetrics(SM_CYVSCROLL); 
int wPos = GetScrollPos(hWnd, SB_VERT);

char szt[128]; 
int ic=sprintf(szt,"%d",wPos); 
RECT rc; GetClientRect(hWnd,&rc); 
SetBkMode(hdc,TRANSPARENT); 

int x=rc.right-cx-16, y= cy+ (rc.bottom-2*cy)*(wPos/255.); 
DWORD c = GetPixel(hdc, x, y);  
c = 0xFFFFFF - c;  
SetTextColor(hdc,c); 
TextOut(hdc,x,y, szt,ic); 

//ReleaseDC(hWnd,hdc); 
}




// ����Ի���  InputBox   �ο� InputBox(prompt[, title][, deft][, xpos][, ypos][, helpfile, context])

INT_PTR CALLBACK DialogProc(HWND hdlg,UINT uMsg, WPARAM wParam,LPARAM lParam){

static struct INPUT{HWND hdlg; char*deft; char*title; char*prompt;}*Input=0; 


switch (uMsg)  {

case WM_INITDIALOG:{
Input=(INPUT*)lParam; // ={0,deft, title, prompt }; 

if (!Input) { 
MessageBox(0,"Dialog Parameter Error!",0,0); 
// DestroyWindow(hdlg); return -1; //    
EndDialog(hdlg,false); 
}

if (!Input->deft) { 
MessageBox(0,"Input Function Error!",0,0); 
EndDialog(hdlg,-1); 
}

Input->hdlg=hdlg; 
SendDlgItemMessage(hdlg, IDC_COMBO1, WM_SETTEXT, 0, (long)Input->deft);
SendDlgItemMessage(hdlg, IDC_COMBO1, CB_ADDSTRING, 0, (long)Input->deft);
SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, 0, 0);

// SetWindowText(GetDlgItem(hdlg,IDC_COMBO1),g_szt); 
if(Input->title) SendMessage(hdlg,WM_SETTEXT,0,(long)Input->title); 
if(Input->prompt) SendDlgItemMessage(hdlg, IDC_STATIC1, WM_SETTEXT, 0, (long)Input->prompt);

SendDlgItemMessage(hdlg, IDC_COMBO1,CB_SETCURSEL,0,0);

}break;  //SetClassLong  // HINSTANCE hinst=GetModuleHandle(0); 
    

case WM_DESTROY:
// MessageBox(0,"WM_DESTROY",0,0);
break; 




case WM_COMMAND: 
switch(LOWORD(wParam)){ 

case IDOK:{ 
int is=SendDlgItemMessage(hdlg, IDC_COMBO1,CB_GETCURSEL,0,0);
if (is==-1) SendDlgItemMessage(hdlg, IDC_COMBO1,WM_GETTEXT,1024-1, (long)Input->deft);
else SendDlgItemMessage(hdlg, IDC_COMBO1,CB_GETLBTEXT, is, (long)Input->deft);

//EndDialog(hdlg,GetDlgItemInt(hdlg,IDC_COMBO1,0,0));    
EndDialog(hdlg,1);    

}break; 

case IDCANCEL:  
*Input->deft = 0;  *(Input->deft+1)=0; 
//EndDialog(hdlg,-1);  
EndDialog(hdlg,0); 
// DestroyWindow(hdlg);   
// SendMessage(GetParent(hdlg),WM_APP+0x01,0,0);
break;
     
//defaul:break;
}//switch(wmId)
break; // WM_COMMAND

default:break;//switch (uMsg)
}
return FALSE;  // message  not processed 
}

char* InputBox(const char* deft, const char*title, const char*prompt){  // =0=0
static char szInput[1024]; 
if (deft) strcpy(szInput,deft); 
else *szInput=0; 

struct {HWND hdlg; const char*deft; const char*title; const char*prompt;}INPUT={0,szInput, title, prompt }; 

DialogBoxParam(0,(LPCTSTR)IDD_DIALOG1,0L,DialogProc,(long)&INPUT); 

return szInput; 
}





#endif  // WIN32_BASICS




#if OPENGL_RELATED |1


// ���� ViewPort��ͶӰ���󡢹۲����.  bMenu=true, ���ڲ����� gView. 

void gView(int W, int H, bool bMenu = false){
if (W==0 || H == 0) W =1,  H = 1;   // Prevent A Divide By Zero error

float x=0, y=0, w=W, h=H; 

#if MINI_WINDOW|0    // û���ô�. Opengl �ӹ���������. 

int B=4; 

x=W/3+B, y=B, w=2*W/3 -2*B, h=2*H/3-2*B;

#endif  // MINI_WINDOW


glViewport(x,y,w,h);  // Ĭ��

//glViewport(0, h/2, w/2, h/2);  // û��Ҫ����, ��Ϊ gl ��Ⱦ����ռ�������û���. 


glMatrixMode(GL_PROJECTION); glLoadIdentity();  //  ����Ĭ��  ? 

	//if(bMenu) glOrtho(-1, 1,   -1, 1, -1, 1);       // ����ϵͳĬ�ϵ� -- ���Բ����� glView.  ��ȷ����ƶ�������! 
	//if(bMenu) glOrtho(1, -1,   1, -1, -1, 1);      //  [ 1, -1;  1, -1;  -1, 1] 
	// if(bMenu) glOrtho(0, w, 0, h, -1, 1);               // ������������  
	
if (bMenu) glOrtho(-w / 2., w / 2., -h / 2., h / 2., -w, w);     // ���Ǻ�������--����߶�  
else   gluPerspective(60, (float)w / h, 0.1, 4000.0f);    // VR�ռ�߶�  
//     gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1, 2000.0f);  // 500.0f  // ���ǲ��� 3ds�ļ�ʱ������ // FOV  // Aspect Ratio	
                      			 


// glMatrixMode(GL_MODELVIEW);  // �����λ�ò�������Ķ�! ...... ����  gluPerspective �ƺ�������! ---��������� 3ds.cpp ĳ��û��ǡ������ MatrixMode !  

//return;	// �����������ָ��, ��ôȫ���л��ᵼ�����������! 


// glMatrixMode(GL_MODELVIEW); glLoadIdentity();  // Ĭ��.  postmultiplies ���ҳ�,�ú����ӳ� the current matrix.      

gluLookAt(2,1,10.5,  0,0,0,   0,1,0);   // ����ͼ, gl��z-���������. ���ֱ��. 

// gluLookAt(1,1,1, 0,0,0,  0,1,0);    // �򵥸���ͼ  
// gluLookAt(4,1,4, 0,0,0,  0,1,0);     // ����ͼ   

// gluLookAt( 6, 2, 4,     0, 0, 0,    0, 0, 1);  // ����λ��: y-����, �泯z-����   

	//gluLookAt(1,1,1, 0,0,0,  0,0,1);  // �չ�һЩcsģ��: z-����, �泯x-����   
	//gluLookAt(50, 50, 10, 0, 0, 0, 0, 0, 1);  // �չ�һЩ�Ƚϴ��csģ��: z-����, �泯x-����   

}  // glView  



// ��ʼ�� gl ����. glz - opengl application.  

bool gSetup(HWND hWnd){
	
// HWND hWnd=WindowFromDC(hdc);   	 

HDC hdc = GetDC(hWnd);   //  if (!setPixelFormat(hdc)) PostQuitMessage (0): ......   

PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1, PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER, //  | PFD_SUPPORT_OPENGL
		PFD_TYPE_RGBA, 16, 0, };    // RGB and Alpha pixel type. Depthbits is ignored for RGBA. //  pfd.dwLayerMask = PFD_MAIN_PLANE;

// pfd.cAccumBits = pfd.cStencilBits = 0;  // No special bitplanes needed; We desire no stencil bits

int pf;  if ((pf = ChoosePixelFormat(hdc, &pfd)) == false){ eInfo("ChoosePixelFormat"); return false; }
if (SetPixelFormat(hdc, pf, &pfd) == false) { eInfo("SetPixelFormat");  return false; }

// gl���� ......   
HGLRC hrc = wglCreateContext(hdc);  wglMakeCurrent(hdc, hrc);

//  �ӿڡ�ͶӰ����� ...... 
// int w=0, h=0;	RECT rc; GetClientRect(hWnd, &rc);  w=rc.right; h=rc.bottom;    


// gView(w, h, false);   // ��  glzView �ڲ����� Camera, ȫ���л��ᵼ�����������! ����취�Ǽ�¼һ��ȫ�����! 

//RECT rc; GetClientRect(hWnd, &rc);  float w=rc.right, h=rc.bottom;   w/=50;  h/=50; 
// glOrtho(-w/2, w/2, -h/2, h/2, -100,100);

glMatrixMode(GL_MODELVIEW);   
gluPerspective(60, 1.0,  1, 1000.0f);   
gluLookAt(2,1,10.5,  0,0,0,   0,1,0);


//  glӦ�� ..................  

// glEnable(GL_TEXTURE_2D);  glEnable(GL_DEPTH_TEST);  //  ����"Terrain.raw"�ļ�,��Ϊ�߳�����

// glDisable(GL_CULL_FACE); 

// glShadeModel(GL_SMOOTH);

//CreateTexture(g_Texture[0], "wall.bmp");    \
CreateTexture(g_Texture[1], "wall1.bmp");

//g_Texture[0]=LoadTexture( "wall.bmp");   \
g_Texture[1]=LoadTexture( "wall1.bmp");

//g_Texture[1]=ReadTexture( "wall1.bmp");


// glLineWidth(2);
// glPointSize(10);  // Ĭ��=1. դ�񻯵��ֱ��, ��λ����������


return true;
}



// getFPS, ����֡����, ��ÿ 1 ������ʾ֡�ĸ���.
int getFPS(){
	//	int g_FPS = 24;   // ֡Ƶ��, ÿ��֡��. ͨ��Ϊ 40 ֡. 
	//	float g_SPF = 1000. / g_FPS;  // ֡����, �����֡ʱ����, ������. ͨ��Ϊ 25 ms. 

	static int iFPS = 0, FPS = 0, t = 0, T = GetTickCount();  // iFPS=ʵ��֡����=iClips,  FPSʵ��֡Ƶ��,��g_FPS����.   
	iFPS++;  // ÿֻ֡�ܵ���һ��! 
	t = GetTickCount();  // ms 

	if (t - T >= 1000) {  // �ﵽһ��  //	if (dt>g_SPF)   
		T = t;  // ms 
		FPS = iFPS; iFPS = 0;
	}  // if 

	return FPS;
}















// glTextOut, wglUseFontBitmapsW ��ʾ�ı�. x,y Ϊ��ʼ��ʾ��λ�õ���Ļ����   

// �����ʽ��ָ�� /c /s ... ��ɫ����С  ָ���ʶ��Ҫ����. ��һ��ָ��֮����ַ�������ʾ. 

int TextOutFW(float x, float y, const wchar_t  *fmt, ...) {
    if (!fmt) return 0;


    static wchar_t  gsz[1024];  gsz[0] = 0;
    va_list ap;  va_start(ap, fmt);   vswprintf(gsz, fmt, ap);  va_end(ap);

    float c[4] = { 0 };
    float s = 1;  // �����С����. 

#if PARSE_CMD|1

    wchar_t*p = gsz;


    do {

        p = wcschr(p, L'/');    // ����� '//'
        if (!p) break;

        p++;

        switch (*p) {
        case L'r':  c[0] = wcstol(p + 1, 0, 16);  break;
        case L'g':  c[1] = wcstol(p + 1, 0, 16);  break;
        case L'b':  c[2] = wcstol(p + 1, 0, 16);  break;
        case L'a':  c[3] = wcstol(p + 1, 0, 16);  break;
        case L's':  s = wcstod(p + 1, 0L); break;

        default: break;
        }

        if (*p == 0) break;

    } while (p);


    p = wcschr(gsz, L'/');
    if (p) *p = 0;

#endif  // PARSE_CMD



    int ic = wcslen(gsz);

    if (ic <= 0) return 0;




    HDC hdc = wglGetCurrentDC(); // SetTextColor(hdc,0xFFFFFF);  SetBkColor(hdc,0); 

#if USE_FONT|1  // ���� 

    static HFONT g_hFont = 0L;  // Ĭ������ GB2312_CHARSET, "����" ,    48, 0, 0, 0, FW_NORMAL,  // 800, 

    if (!g_hFont) {

        // g_hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);  // DEFAULT_GUI_FONT��ANSI_VAR_FONT ��������

        LOGFONT lf = { 16, 0, 0, 0, FW_THIN, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, { 0 }, };
        lstrcpy(lf.lfFaceName, "Times New Roman");   // ���� ����  ���Ŀ���  Times New Roman  
        g_hFont = CreateFontIndirect(&lf);
    }

    HFONT hfo = (HFONT)SelectObject(hdc, g_hFont);

#endif    // USE_FONT






    float ct[4] = { 0 };  glGetFloatv(GL_CURRENT_COLOR, ct);  // ����Ӱ����ɫ! 
    glColor4fv(c);

//    glPushAttrib(GL_LIST_BIT);  // Pushes The Display List Bits ���Ҫ���ֵĻ� 


    bool bL = glIsEnabled(GL_LIGHTING);  if (bL) glDisable(GL_LIGHTING);
    bool bT = glIsEnabled(GL_TEXTURE_2D);  if (bT) glDisable(GL_TEXTURE_2D);



    int base = 0;    // ic = wcslen(gsz);
    wchar_t dwc;




// �� 3-ά TextOutFW ��Ƚ��������3�д���. 

float z=0; 

//float xs[4]={x,y,1,1}, xo[4];
//ToWorld(xs,xo);   x=xo[0], y=xo[1], z=xo[4]; 



GLdouble xw = x, yw = y, zw = 1;
ToWorld(x, y, .3, &xw, &yw, &zw);
x=xw, y=yw, z=zw;




#if USE_BITMAP|1    // ͨ����դ��λ

    glRasterPos3f(x, y, z);    // ���ù�դλ��: GL_CURRENT_RASTER_POSITION_VALID      

    for (int i = 0; i < ic; i++) {

        dwc = gsz[i];  // Lead bytes are unique to double-byte character sets. See also IsDBCSLeadByteEx

        base = glGenLists(1);  // CreateFontBmp ...... 

        wglUseFontBitmapsW(hdc, dwc, 1, base); 		// ��ʾ 2-ά����Ч���Ϻ�!  

        glCallList(base);  //  glCallList/s ����ľ���!  

        glDeleteLists(base, 1);   // ��;�޷�ɾ�� display list set. 

    }  //  for 



#else             // ��������  -- ��ʾ��λ���ֲ����� 


    // glMatrixMode(GL_PROJECTION); glPushMatrix();

    glMatrixMode(GL_MODELVIEW); glPushMatrix();
    // glMatrixMode(GL_TEXTURE); glPushMatrix();
float r=1.0; 
    glTranslatef(x*r, y*r, z*r);
    glScalef(s, s, s / 2.);




    for (int i = 0; i < ic; i++) {

        dwc = gsz[i];  // Lead bytes are unique to double-byte character sets. See also IsDBCSLeadByteEx

        // if (IsDBCSLeadByte(gsz[i])) { dwc = (DWORD)((gsz[i] << 8) | gsz[i + 1]); k = 2;  }
        // mbtowc(&dwc, (char*)gsz+i,2); 

        base = glGenLists(1);  // CreateFontBmp ...... 


        // wglUseFontBitmapsW(hdc, dwc, 1, base); 		// ����Bitmap, ��Ϊ�������ĳЩ��ʾ����˸ 

        wglUseFontOutlinesW(hdc, dwc, 1, base, 0, 0.1f, WGL_FONT_POLYGONS, 0L);  // WGL_FONT_LINES   // WGL_FONT_LINES Ҳ����ľ���!  

        // glListBase(base - dwc);  glCallLists(base, GL_UNSIGNED_SHORT, &dwc);  // ����һ�е�Ч: glCallList(base); 

        glCallList(base);  //  glCallList/s ����ľ���!  

        glDeleteLists(base, 1);   // ��;�޷�ɾ�� display list set. 

    }  //  for 





    glMatrixMode(GL_MODELVIEW); glPopMatrix();
    // glMatrixMode(GL_PROJECTION); glPopMatrix();   
    //glMatrixMode(GL_TEXTURE); glPopMatrix();   


#endif   // USE_BITMAP 


//   SelectObject(hdc, hfo);

//    glPopAttrib();

    glColor4fv(ct);     // TextOut(g_hDC,0,0,gsz,strlen(gsz));

    if (bL) glEnable(GL_LIGHTING);
    if (bT) glEnable(GL_TEXTURE_2D);

    return ic;
}


 


// glTextOut, glzDrawText, ������������ʾ�ı�.  x, y, z=0 Ϊ��ʼ��ʾ��λ�õ���������.  

// ���� wglUseFontBitmapsW��wglUseFontOutlinesW ���ı���ʾ�ĺ÷���. 

// �ο� nehe.gamedev.net, Jeff Molofee 2000: 

// �����ʽ��ָ�� /c /s ... ��ɫ����С 

int TextOutFW(float x, float y, float z,  const wchar_t  *fmt, ...) {
if (!fmt) return 0;
   

static wchar_t  gsz[1024];  gsz[0] = 0;
va_list ap;  va_start(ap, fmt);   vswprintf(gsz, fmt, ap);  va_end(ap);

float c[4] ={0};
float s = 1;  // �����С����. 
  
#if PARSE_CMD|1

wchar_t*p=gsz;


do {

    p = wcschr(p, L'/');    // ����� '//'
    if (!p) break;

    p++;

    switch (*p) {
    case L'r':  c[0] = wcstol(p + 1, 0, 16);  break;
    case L'g':  c[1] = wcstol(p + 1, 0, 16);  break;
    case L'b':  c[2] = wcstol(p + 1, 0, 16);  break;
    case L'a':  c[3] = wcstol(p + 1, 0, 16);  break;
    case L's':  s = wcstod(p + 1, 0L); break;

    default: break;
    }

    if (*p == 0) break;

} while (p);


p = wcschr(gsz, L'/'); 
if(p) *p=0; 

#endif  // PARSE_CMD



int ic=wcslen(gsz);

if(ic<=0) return 0; 




HDC hdc = wglGetCurrentDC(); // SetTextColor(hdc,0xFFFFFF);  SetBkColor(hdc,0); 

#if USE_FONT|1  // ���� 

static HFONT g_hFont = 0L;  // Ĭ������ GB2312_CHARSET, "����" ,    48, 0, 0, 0, FW_NORMAL,  // 800, 

if (!g_hFont) {

    // g_hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);  // DEFAULT_GUI_FONT��ANSI_VAR_FONT ��������

    LOGFONT lf = { 16, 0, 0, 0, FW_THIN, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, { 0 }, };
    lstrcpy(lf.lfFaceName, "Times New Roman");   // ���� ����  ���Ŀ���  Times New Roman  
    g_hFont = CreateFontIndirect(&lf);
}

HFONT hfo = (HFONT)SelectObject(hdc, g_hFont);

#endif    // USE_FONT






float ct[4] = { 0 };  glGetFloatv(GL_CURRENT_COLOR, ct);  // ����Ӱ����ɫ! 
glColor4fv(c);    

glPushAttrib(GL_LIST_BIT);  // Pushes The Display List Bits ���Ҫ���ֵĻ� 
  

bool bL = glIsEnabled(GL_LIGHTING);  if (bL) glDisable(GL_LIGHTING);
bool bT = glIsEnabled(GL_TEXTURE_2D);  if (bT) glDisable(GL_TEXTURE_2D);



int base = 0;    // ic = wcslen(gsz);
wchar_t dwc;




#if USE_BITMAP|1    // ͨ����դ��λ

glRasterPos3f(x, y, z);

for (int i = 0; i < ic; i++) {

dwc = gsz[i];  // Lead bytes are unique to double-byte character sets. See also IsDBCSLeadByteEx

base = glGenLists(1);  // CreateFontBmp ...... 

wglUseFontBitmapsW(hdc, dwc, 1, base); 		// ��ʾ 2-ά����Ч���Ϻ�!  

glCallList(base);  //  glCallList/s ����ľ���!  

glDeleteLists(base, 1);   // ��;�޷�ɾ�� display list set. 

}  //  for 



#else             // ��������


// glMatrixMode(GL_PROJECTION); glPushMatrix();

glMatrixMode(GL_MODELVIEW); glPushMatrix();
// glMatrixMode(GL_TEXTURE); glPushMatrix();

glTranslatef(x, y, z);
glScalef(s, s, s / 2.);




for (int i = 0; i < ic; i++) {

dwc = gsz[i];  // Lead bytes are unique to double-byte character sets. See also IsDBCSLeadByteEx

// if (IsDBCSLeadByte(gsz[i])) { dwc = (DWORD)((gsz[i] << 8) | gsz[i + 1]); k = 2;  }
// mbtowc(&dwc, (char*)gsz+i,2); 

base = glGenLists(1);  // CreateFontBmp ...... 


// wglUseFontBitmapsW(hdc, dwc, 1, base); 		// ����Bitmap, ��Ϊ�������ĳЩ��ʾ����˸ 

wglUseFontOutlinesW(hdc, dwc, 1, base, 0, 0.1f, WGL_FONT_POLYGONS, 0L);  // WGL_FONT_LINES   // WGL_FONT_LINES Ҳ����ľ���!  

// glListBase(base - dwc);  glCallLists(base, GL_UNSIGNED_SHORT, &dwc);  // ����һ�е�Ч: glCallList(base); 

glCallList(base);  //  glCallList/s ����ľ���!  

glDeleteLists(base, 1);   // ��;�޷�ɾ�� display list set. 

}  //  for 





glMatrixMode(GL_MODELVIEW); glPopMatrix();   
// glMatrixMode(GL_PROJECTION); glPopMatrix();   
//glMatrixMode(GL_TEXTURE); glPopMatrix();   


#endif   // USE_BITMAP 


//   SelectObject(hdc, hfo);

glPopAttrib();   

glColor4fv(ct);     // TextOut(g_hDC,0,0,gsz,strlen(gsz));

if (bL) glEnable(GL_LIGHTING);    
if (bT) glEnable(GL_TEXTURE_2D);

return ic;
}













// w Ϊ�����߶ΰ볤��, dw Ϊ��ͷβ�����ռ�볤�ȵı���. w=1, dw=0.04  
void drawAxis(float w=1, float dw=0.03) {  // =1,  =0.04 


glPushAttrib(GL_CURRENT_BIT);   // ������ܻ�Ӱ���������ģ�ͻ���

bool bT = glIsEnabled(GL_TEXTURE_2D);  if (bT) glDisable(GL_TEXTURE_2D);  // ����Ӱ����ɫ 

//glPushMatrix(); 


    dw *= w;
    float dp = w -  1.618* dw ;  // dp= w - ��ͷ����

float cr=0.9; 

    glBegin(GL_LINES);

    glColor3f(0, 0, cr);  // x��    
    glVertex3f(-w, 0, 0);  glVertex3f(w, 0, 0);  //  glVertex2i
    glVertex3f(w, 0, 0);  glVertex3f(dp, dw / 2, 0);
    glVertex3f(w, 0, 0);  glVertex3f(dp, -dw / 2, 0);

glVertex3f(dp, -dw / 2, 0); glVertex3f(dp, dw / 2, 0);

    glColor3f(0, cr, 0);  // y�� 
    glVertex3f(0, w, 0);   glVertex3f(0, -w, 0);
    glVertex3f(0, w, 0);   glVertex3f(-dw / 2, dp, 0);
    glVertex3f(0, w, 0);   glVertex3f(dw / 2, dp, 0);

glVertex3f(-dw / 2, dp, 0); glVertex3f(dw / 2, dp, 0);

    glColor3f(cr, 0, 0);  // z�� 
    glVertex3f(0, 0, w);  glVertex3f(0, 0, -w);
    glVertex3f(0, 0, w);  glVertex3f(-dw / 2, 0, dp);
    glVertex3f(0, 0, w);  glVertex3f(dw / 2, 0, dp);

glVertex3f(-dw / 2, 0, dp); glVertex3f(dw / 2, 0, dp);
    glEnd();

 w+=0.08; 
#if MARK_AXIS|1

    TextOutFW(0, 0, 0, L"ԭ��o/c0x3F3F3F/s0.3");  // 7F/2=3F

    TextOutFW(w, 0, 0, L"x/c0x3F0000/s0.3/b1");  // 7F/2=3F
    TextOutFW(0, w, 0, L"y/c0x003F00/s0.3/g1");
    TextOutFW(0, 0, w, L"z/c0x00005F/s0.3/r1");

  
#endif  
  
//glPopMatrix();

if (bT)glEnable(GL_TEXTURE_2D);
glPopAttrib();
}





// glzRender, glRenderScene, function renders the entire scene.

void gRender(){  // HWND hWnd=0L ע��ָ����ȷ���� 

#if RENDER_SCENE |1

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	 // Clear The Screen And The Depth Buffer
glClearColor(0.5, 0.5, 0.5, 0);   // (0,0,0,1) Ĭ��.  (0.2, .2, .8, 1);  (0.4, .4, .8, 1);


float w=3; 
drawAxis(w);

    
// RenderText();   

static float quad[4*3*sizeof(float)]={0,0,0,     1.5, 0, 0,     1.5, 1.5, 0,       0,1.5,0, };   // �� xoy ��, �� wglUseFontOutlinesW Ĭ�����һ�� 

static float r=0; r+=3.1415926/180;     

glMatrixMode(GL_MODELVIEW);  // �����п���Ӱ��ͶӰ����  

// glRotatef(0.5,   0.2, 1, 0); 


// HDC hdc=wglGetCurrentDC(); HWND hWnd=WindowFromDC(hdc);  RECT rc;GetClientRect(hWnd,&rc); 
int vp[4]; glGetIntegerv(GL_VIEWPORT,vp);      // qTrace("%d��%d,  %d��%d, (%d,%d)  ", rc.right, rc.bottom,  vp[2],vp[3], vp[0], vp[1]);
float x=vp[2]/50., y=vp[3]*0.75;  
// x=rc.right/80.;  y=rc.bottom*0.95;  

// TextOutBm( x, y, "��������  FontTexture. FPS=%d/r1/g.8/b%.1f", getFPS(), sin(r)); 

TextOutFW(x, y, L"��������  FontTexture. FPS=%d./b1/g%.1f/s1", getFPS(), sin(r));

// TextOutFW(-0.5, 3.8,0.2,  L"��������  FontTexture. FPS=%d./b1/g%.1f/s0.5", getFPS(), sin(r)); 


// renderText(quad,L"  ABC����/r%0.1f", sin(r));
// renderText(quad,L"����  ABC/r%0.1f", sin(r));


// gDrawText(-(rc.right-rc.left)/5., -(rc.bottom-rc.top)/5., L"BillBoard���� FPS = %3d ����֡���� 40/r.8",  getFPS()); 

#endif  // RENDER_SCENE

//  SwapBuffers(wglGetCurrentDC());  // �� WinMain / MainLoop �е��� 				// g_hDC, Swap the backbuffers to the foreground
}






#endif  // OPENGL_RELATED 


#if MDI_SPECIAL|1


// CreateMDIWindow  is similar to WM_MDICREATE, except that the function can create an MDI child window in a different thread, while the message cannot. 
HWND CreateMDIWnd( HWND hWnd, int iType, void*data=0, const wchar_t*szTitle=0 ){

HWND hClient=GetWindow(hWnd,GW_CHILD); 
RECT rc; GetClientRect(hWnd,&rc);

CCW_DATA*pc=new CCW_DATA;  pc->DT_Type=iType; pc->pdata=data; 

MDICREATESTRUCTW mdics;   ZeroMemory(&mdics,sizeof(mdics));
mdics.szClass=WNDCLS_CLIENT;  
mdics.szTitle=szTitle; 
mdics.x=rc.left, mdics.y=rc.top;  mdics.cx=rc.right-rc.left, mdics.cy=rc.bottom-rc.top; 
int B=16; 
mdics.x=(rc.right-rc.left)/3, mdics.y=rc.top+B;  mdics.cx=2*(rc.right-rc.left)/3-2*B, mdics.cy=(rc.bottom-rc.top)-4*B; 


mdics.lParam=(long)pc; 

// mdics.hOwner=GetModuleHandle(0); 


mdics.style= WS_CHILD|WS_VISIBLE |WS_OVERLAPPEDWINDOW   ;  //  |WS_SYSMENU  |WS_CAPTION   |WS_SIZEBOX WS_VISIBLE;


HWND  hNew=CreateMDIWindowW(WNDCLS_CLIENT,szTitle,mdics.style,  mdics.x,mdics.y, mdics.cx,mdics.cy,    hClient,0,  (LPARAM)pc);     \
if(hNew==NULL) eInfo("CreateMDIWindowW"); 

// HWND hNew= (HWND)SendMessageW(hClient,WM_MDICREATE,0,(LPARAM)&mdics);  // Ҳ���� 

// HWND hNew=(HWND)SendDlgItemMessageW(hWnd, ID_CLIENT,WM_MDICREATE,0,(LPARAM)&mdics);  // ��Ч 
// SetWindowLong(hNew,GWL_ID, iType); 


//  �����´���
// SendMessage(hClient, WM_MDIACTIVATE,(long)hNew,0);
// SendMessage(hClient, WM_MDIMAXIMIZE,(long)hNew,0);


// ���Ŵ���
// SendMessage(hClient, WM_MDITILE, (WPARAM)MDITILE_HORIZONTAL, 0);
//SendMessage(hParent, WM_MDICASCADE, (WPARAM)0, 0); //M DITILE_VERTICAL,MDITILE_HORIZONTAL
//SendMessage(hClientWnd, WM_MDIICONARRANGE, 0, 0 ); //Arrange the child windows.

// ShowWindow(hNew,SW_SHOWMAXIMIZED); 

 

return hNew;
}



 






#endif  // MDI_SPECIAL 


WNDPROC OldClientProc;  // MDICLIENT ������Ҫ��������, ����λ����Ҫ����

LRESULT APIENTRY InterceptClient(HWND hWnd,UINT uMsg,WPARAM wP,LPARAM lP) {

switch(uMsg){

case WM_CREATE: {  // MoveWindow û��Ч��   

//WORD wTesmi[]= L"\x628\x650\x633\x640\x640\x640\x640\x640\x640\x640\x640\x640\x640\x640\x640\x640\x645\x650\x20\x623\x644\x644\x647\x650\x20\x623\x644\x631\x651\x64E\x62D\x652\x645\x64E\x627\x646\x20\x650\x20\x623\x644\x631\x651\x64E\x62D\x650\x64A\x640\x640\x640\x640\x645\x650";
// MessageBoxW(NULL,wTesmi,L"readfile",0);

int BR=0;  
RECT rc; GetClientRect(hWnd,&rc); 
rc.left=rc.right*2/3; 
MoveWindow(hWnd, rc.left, rc.top,rc.right-rc.left, rc.bottom-rc.top, true); 
return 0;  

}break;




case WM_PAINT:{ 

PAINTSTRUCT ps;
HDC hdc= BeginPaint(hWnd,&ps);
RECT re; GetClientRect(hWnd,&re);
DrawTextW(hdc, L"MDICLIENT����",wcslen(L"MDICLIENT����"),&re,DT_RTLREADING);
   
//StretchBlt(hdc,0,0,re.right,re.bottom, hMemDc,  0,0,re.right/2,re.bottom, SRCCOPY);
//SelectObject(hdc,hBitmap);

EndPaint(hWnd,&ps);	
}break;  

//deft:break;//switch(uMsg)
}

return CallWindowProc(OldClientProc,hWnd,uMsg, wP, lP); // not reached!
} 


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
switch(message) {


case WM_CREATE:  { 

#if CREATE_MDICLIENT|1

CLIENTCREATESTRUCT ccs; 
ccs.hWindowMenu  =  GetMenu(hWnd); 
//ccs.idFirstChild = 0x10;

int BR=4; 

hClientWnd=  // CreateMDIClientWindow(hWnd);  
CreateWindowExW(WS_EX_TRANSPARENT,  //WS_EX_CLIENTEDGE, //   |WS_EX_LAYERED  ,  // 
L"MDICLIENT", L"MDICLIENT_AREA",   
WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_SIZEBOX |MDIS_ALLCHILDSTYLES ,  //  |WS_SIZEBOX|MDIS_ALLCHILDSTYLES  |WS_CLIPCHILDREN,  //  |WS_VSCROLL|WS_HSCROLL
0, 0, 0,0,     hWnd,(HMENU)ID_CLIENT,0,(LPWSTR)&ccs);  
if(!hClientWnd)qTrace("CreateWindowEx");

//OldClientProc=(WNDPROC)SetWindowLongW(hClientWnd,GWL_WNDPROC,(LPARAM)InterceptClient); \
if(!OldClientProc) qTrace("InterceptClient");			

SetClassLong(hClientWnd,GCL_HBRBACKGROUND,(long)GetStockObject(WHITE_BRUSH)); //GCL_HICON    LTGRAY_BRUSH

//if(0==SetWindowLong(hClientWnd,GWL_EXSTYLE, WS_EX_LAYERED))qTrace("SetWindowLong");   \
SetLayeredWindowAttributes(hClientWnd, 0, 50, LWA_ALPHA);  

//\\hWorkspace=CreateView(hClientWnd,WINDOW_EDIT,L"Workspace",0,0,0); //,(void*)L"Workspace",WINDOW_BLANK

#endif  // CREATE_MDICLIENT


g_hGL=CreateMDIWnd(hWnd,DT_OG,  0L,  L"OpenGL"); 
//PostMessage(hWnd,WM_DRAW,0,0);
}break; 


case WM_DESTROY:{ 
    
// KillTimer(hClientWnd,0x01);

//// DestroyWindow(hCommand);
//// UnregisterClassW(L"Commands",GetModuleHandle(NULL));

// DestroyMenu(hMenu);
// DeleteObject(hbrWhite);
 
PostQuitMessage(0);
}break;




case WM_PAINT:{   // Frame ��Ϣ 

PAINTSTRUCT ps; 

HDC hdc = BeginPaint(hWnd, &ps);
char*p=(char*)"Tesseractѡ��";          // �������� -> C/C++ ->���� -> ����ģʽ��-> ��
SetBkMode(hdc, TRANSPARENT);
TextOut(hdc, 0, 0, p,strlen(p));

EndPaint(hWnd,&ps);
}break; 



case WM_SIZE: PostMessage(hWnd,WM_DRAW,0,0);  break; 

//case WM_MOVE:
case WM_DRAW:{  // WM_SIZE ��ô����? 

HWND hdlg=FindWindowEx(hWnd,0,"#32770",0);
if(!hdlg) break; 

RECT rc; GetClientRect(hWnd, &rc);
RECT rg; GetWindowRect(hdlg, &rg);

int BR=0; 
int x = rc.left + (rg.right - rg.left) + BR;
int y = rc.top ;  // + BR
int w = rc.right - x + BR;
int h = rc.bottom - rc.top + BR;

MoveWindow(hClientWnd, x, y, w,h, true); 
}break;



case WM_COMMAND:{  // OnCommand(hWnd,wParam,lParam);


#if PARSE_CMD|1

//HWND hWndActive=(HWND)SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDIGETACTIVE,0,0);  \
if(IsWindow(hWndActive))  SendMessage(hWndActive,WM_COMMAND,wParam,lParam);


WORD wId=LOWORD(wParam), wNd=HIWORD(wParam);  

switch (wId){

case IDF_EXIT: DestroyWindow(hWnd); break; 
  
case IDH_ABOUT: MessageBox(0,"���ĵ�����","3D-Engine",MB_OK); break;


case IDF_NEW:{

MDICREATESTRUCTW mdics;   ZeroMemory(&mdics,sizeof(mdics));
// mdics.hOwner=GetModuleHandle(0); 
mdics.style=WS_CHILD|WS_VISIBLE |WS_OVERLAPPEDWINDOW;   //|WS_SIZEBOX|WS_CAPTION|WS_MAXIMIZEBOX;
mdics.lParam=0; 
mdics.szClass=WNDCLS_CLIENT;  mdics.szTitle=L"Untitled";   // mdics.x=mdics.y=mdics.cx=mdics.cy=CW_USEdeft; 

HWND hNew=(HWND)SendDlgItemMessageW(hWnd, ID_CLIENT,WM_MDICREATE,0,(LPARAM)&mdics);
SetWindowLong(hNew,GWL_ID, IDT_NEW); 

}break; 

case IDF_OPEN:{

wchar_t szFile[256];
bool bok=GetFileName(szFile, 0);
if(!bok) break; 

//\\LPBYTE pDIB = dib_read(szFile);  // malloc  
//\\if(!pDIB){  qTrace("dib_read");  break; } 

MDICREATESTRUCTW mdics; ZeroMemory(&mdics,sizeof(mdics));
mdics.szClass=WNDCLS_CLIENT;  
mdics.style= WS_CHILD|WS_VISIBLE|WS_VSCROLL;   
//mdics.hOwner=GetModuleHandle(0); 

mdics.szTitle=wcsrchr(szFile, L'\\');  

//CCW_DATA* pa= (CCW_DATA*)malloc(sizeof(CCW_DATA));    \
pa->xf = (XFORM*)malloc(sizeof(XFORM));  *pa->xf={1,0,  0,1,  0,0}; \
pa->pt = (POINT*)malloc(sizeof(POINT));  *pa->pt={0,0};  \
pa->pdata=pDIB; 

CCW_DATA* ccd = new CCW_DATA;  
//\\ccd->pdata=pDIB; 
mdics.lParam=(long)ccd;   

HWND hOpen= (HWND) SendMessageW(GetDlgItem(hWnd,ID_CLIENT),WM_MDICREATE,0,(LPARAM)&mdics);

// static int g_OpenID = ID_OPEN -1;   g_OpenID++;  
SetWindowLong(hOpen,GWL_ID, ID_OPEN); 

// SetMenu(hOpen, GetMenu(hWnd));  // ��Ч 

}break;  // IDF_OPEN 



case IDT_TEST: {

t_glWT(); 

}break;


case IDT_MATRIX:{  
// t_inv(); 
t_triz(); 

}break; 

case IDT_WT:{

t_unProject(); 

}break;


#if MDI_VIEW |1   // ���ڲ���  

     
  case IDW_CASCADE:
    SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDICASCADE,0,0);
  break;
  case IDW_TILEV:
    //SendDlgItemMessage(hWnd,ITEM_CLIENT,WM_COMMAND,wParam,lParam);
    SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDITILE,MDITILE_VERTICAL,0);
  break;
  case IDW_TILEH:
    SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDITILE,MDITILE_HORIZONTAL,0);
  break;
  case IDW_CLOSE:
    {	
	HWND hactive=(HWND)SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDIGETACTIVE,0,0);
    SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDIDESTROY,(WPARAM)hactive,0);
	}
  break;
  case IDW_CLOSEALL:
    {
    HWND hactive=NULL; int ic=-1;
    do{
    SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDIDESTROY,(WPARAM)hactive,0);
    hactive=(HWND)SendDlgItemMessage(hWnd,ID_CLIENT,WM_MDIGETACTIVE,0,0);
	ic++; if(ic>100) break;
	}while(hactive!=NULL);	

	}
  break;

#endif  // MDI_VIEW



default:break;
}  // switch (LOWORD(wParam))


   

// return 0;  // Cannot return 0!  must return DefFrameProc(hWnd,hClientWnd,message,wParam,lParam);
 

#endif  // PARSE_CMD

}  break;  // WM_COMMAND 

   







//case WM_KEYDOWN: //һ������MDIclient�Ӵ��ھͲ����д���Ϣ?��DefFrameProcW �й�.
//if(wParam==VK_ESCAPE) DestroyWindow(hWnd);
//break;

case WM_HOTKEY: break; 


 
case WM_NOTIFY:
//switch(((LPNMHDR)lParam)->code){
// case NM_DBLCLK: OnTreeViewNM_DBLCLK(hWnd,message,lParam);  break;    
//default: break;	  
//}//switch(((LPNMHDR)lParam)->code)
break; //case WM_NOTIFY:



case WM_MOUSEMOVE:  // if(bToolTipVisible) //SendMessage(hToolTip,TTM_TRACKPOSITION,0,lParam);//MAKELPARAM(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)));
break;
  
  
  

  
default: break;
}  // switch(message) 

//DefWindowProc(hWnd, message, wParam, lParam);
return DefFrameProcW(hWnd,hClientWnd,message,wParam,lParam);    
}


// 2006��3��13�� ClientManipulations 

bool bEmbed=false;


LRESULT CALLBACK ClientChildProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam ){


// static int gx=-1, gy=-1;    // �����������໥����, ����Ҫת�Ƶ� GWL_USERDATA 

switch(Message){

case WM_CREATE: {  // OnCreateView(hWnd,lParam);

// SetGraphicsMode(GetDC(hWnd),GM_ADVANCED);  // SetWorldTransform ���� 

SetScrollRange(hWnd, SB_VERT|SB_HORZ,0,0xFF, TRUE);  // SB_BOTH.  Ĭ�� [0,100]  
//  PostMessage(hWnd,WM_USER+0x01,0,0);  //��ҪSend...;  	



// 1. ���ղ���һ����ʼ�� VIEW DATA.
CREATESTRUCTW*pcs=(CREATESTRUCTW*)lParam; if(!pcs) break;  // If the window being created is an MDI window, this member contains a pointer to an MDICREATESTRUCT structure. 
MDICREATESTRUCTW*pmcs=(MDICREATESTRUCTW*)pcs->lpCreateParams; if(!pmcs) break;
// LPBYTE pcd=(LPBYTE)pmcs->lParam; if(!pcd) return 0; //lpcwd->lpIR = (LPICONRESOURCE)(((MDICREATESTRUCT *)(((CREATESTRUCT *)lParam)->lpCreateParams))->lParam);//pcd = (CHILDDATA*)malloc(sizeof(CHILDDATA));



// 2. ָ�������û�����Ϊ VIEW DATA.  

CCW_DATA*pa=(CCW_DATA*)pmcs->lParam; if(!pa) break; 

SetWindowLongPtr(hWnd,GWLP_USERDATA,pmcs->lParam); // pmcs->lParam  p needs to be global! // why not pcd? ����������/��Χ�й�.

static bool g_bOG=false; 

if(pa->DT_Type==DT_OG){  // ��ʼ�� OpenGL 
if(g_bOG){MessageBox(0,"OpenGL ������һ������",0,0); break; }

gSetup(hWnd);    

g_bOG=true; 
}



}break; //WM_CREATE: not reached;
 

case WM_CLOSE:{ qTrace("WM_CLOSE"); }break;


case WM_DESTROY:{  // destroy CHILDDATA;//����Ӵ������ݴ洢��;
CCW_DATA* pa=(CCW_DATA*)GetWindowLong(hWnd,GWL_USERDATA);  
if (pa == NULL) break;  

if(pa->DT_Type==DT_OG){


}  // ���� OpenGL  


delete(pa);
//return 0;  // ������Ĵ��� ID
}break; 
  

case WM_PAINT:{  // break;   

PAINTSTRUCT  ps;
HDC hdc=BeginPaint(hWnd,&ps); 
 
// TextOut(hdc, 0,0,"����ab", 4);

EndPaint(hWnd, &ps); 

}break;   // WM_PAINT


case WM_COMMAND:{  // �������ڴ���, lParam ���岻��ȷ��

switch (LOWORD(wParam)){

case IDE_GRAY:{ 
char* r=InputBox("1", "ָ����ת����ʽ", "0-B, 1-G, 2-R, 3-W, 4-Meam, 5-Max, 6-Min");
}break; 



case IDP_EXPAND:{
CCW_DATA*pcd = (CCW_DATA*)GetWindowLong(hWnd,GWL_USERDATA); 
if (!pcd) break;
//\\POINT* pt = LocPixel(hWnd); 
//\\dipr_Exec(hWnd,(DIPR_PROC)dipr_expand,  pt->x, pt->y); 
}break; 



default: break;  
}  // switch (LOWORD(wParam))

}break;   // WM_COMMAND 








////  case WM_TIMER:     OnClientChildTimer(hWnd);  break;

//case WM_MOVE:
case WM_SIZE:{  // break;  // ����ܿ�! 

#if WRONG_SCHEME
CREATESTRUCTW*pcs=(CREATESTRUCTW*)lParam; if(!pcs) break;  // If the window being created is an MDI window, this member contains a pointer to an MDICREATESTRUCT structure. 
MDICREATESTRUCTW*pmcs=(MDICREATESTRUCTW*)pcs->lpCreateParams; if(!pmcs) break;
CCW_DATA*pa=(CCW_DATA*)pmcs->lParam; if(!pa) break; 
#endif  // WRONG_SCHEME

CCW_DATA*pa = (CCW_DATA*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
if(!pa) break;  // assert(pa);  // WM_SIZE ��������� WM_CREATE ֮ǰ����! 

if(pa->DT_Type!=DT_OG) break; 
if (SIZE_MINIMIZED == wParam) break; 
// int w=LOWORD(lParam), h= HIWORD(lParam); if (w<20 || h<20){qTrace("%d��%d",w,h);}

glViewport(0,0,LOWORD(lParam), HIWORD(lParam)); 

}break;  // ȫ������!  // glMatrixMode(GL_MODELVIEW);  glLoadIdentity();  gluLookAt(3, 1, 2, 0, 0, 0, 0, 1, 0);






case WM_KEYDOWN:  
if(wParam==VK_ESCAPE){   break;  // DestroyWindow(hWnd); //---�����Menu���ƻ���. 
// SendMessage(hClientWnd,WM_MDIDESTROY,(WPARAM)hWnd,0); 
ShowWindow(hWnd, SW_MINIMIZE); 
}  // if  VK_ESCAPE 
break;

  






  
#if OPENGL_MOUSE|1

case WM_MOUSEWHEEL: {

CCW_DATA*pa = (CCW_DATA*)GetWindowLong(hWnd,GWL_USERDATA); 
if (!pa) break;
if(pa->DT_Type!=DT_OG) break; 

#if MULTI_DOC|1
POINT pt={LOWORD(lParam), HIWORD(lParam)};   // �������Ļ
HWND hClient=GetParent(hWnd); 
// ScreenToClient(hClient,&pt);              // pt �������Ļ
RECT rc; GetWindowRect(hWnd,&rc);
if(!PtInRect(&rc,pt)) break; 

#endif  // 


float dz = (short)HIWORD(wParam);  //   /120.;   // nMouseWheelDelta. WHEEL_DELTA=120

float r = 1;  // * WHEEL_DELTA=120  // LOWORD �Ǹ�����Ϣ MK_CONTROL �ȵ�. (short)����! 

if(dz<0) r/=0.9;
else if(dz>0) r*=.9;


glMatrixMode(GL_MODELVIEW);   // �����������������. --- �����������Ļ��������? 

glScalef(r, r, r);  

return true;
}break;  // case WM_MOUSEWHEEL .


case WM_MOUSEMOVE: {

if (!(MK_LBUTTON&wParam)  && !(MK_RBUTTON&wParam)  ) break;   // ͬʱ�������, ƽ��, �Ҽ���ת

int x = LOWORD(lParam), y = HIWORD(lParam);


CCW_DATA*pa = (CCW_DATA*)GetWindowLong(hWnd,GWL_USERDATA); 
if (!pa) break;
if(pa->DT_Type!=DT_OG){ 
pa->pt.x = x; pa->pt.y = y;    // OGL ���ڽ��������λ�ü�¼ 
break;   
}

if (pa->pt.x == -1 &&  pa->pt.y == -1) { pa->pt.x = x; pa->pt.y = y; break; }

float r = 0.1;   

int dx = x - pa->pt.x, dy = y - pa->pt.y;  


glMatrixMode(GL_MODELVIEW); 

if( MK_RBUTTON&wParam ){  // �Ҽ�����, ����ת������ 

// if(dx<0) dx=-dx;    if(dy<0) dy=-dy; 

glRotatef(r*dy, 1,0,0); 
glRotatef(r*dx, 0,1,0); 

//   pa->pt.x = x; pa->pt.y = y;   break;
}



if( MK_LBUTTON&wParam ){  // �������, ƽ�������  

#if MOVE_CAMERA |0 

float m[16];  glGetFloatv(GL_MODELVIEW_MATRIX, m);            
float vX[3]={m[0+0*4],m[0+1*4],m[0+2*4],};               // vRight  0-��     (i,j) = i+j*4  
float vY[3]={m[1+0*4],m[1+1*4],m[1+2*4],};               // vUp     1-��    
float vZ[3]={m[2+0*4],m[2+1*4],m[2+2*4],};               // vLook   2-��    

if(dx<dy )  glRotatef(r*yd,vX[0],vX[1],vX[2]); 
else        glRotatef(r*xd,vY[0],vY[1],vY[2]); 

#else

r=0.01; 
glTranslatef( dx*r, -dy*r, 0);  

#endif  // MOVE_CAMERA
     
}    // if MK_LBUTTON



pa->pt.x = x; pa->pt.y = y;

}break;


//case WM_NCMOUSELEAVE:   while(ShowCursor(true)<0)ShowCursor(true);   break; 

case WM_RBUTTONUP:
case WM_LBUTTONUP:{      

CCW_DATA* pa=(CCW_DATA*)GetWindowLong(hWnd,GWL_USERDATA);  
if (!pa) break;  // if(pa->DT_Type!=DT_OG) break; 

pa->pt = {LOWORD(lParam), HIWORD(lParam)}; 

}break; 



case WM_RBUTTONDOWN:
case WM_LBUTTONDOWN:{ 
// InvalidateRect(hWnd,0,1); 

CCW_DATA*pa = (CCW_DATA*)GetWindowLong(hWnd,GWL_USERDATA); 
if (!pa) break;

pa->pt = {LOWORD(lParam), HIWORD(lParam)}; 


}break; 
 

#endif  // OPENGL_MOUSE




// case WM_HSCROLL: // SB_LINEUP, The user clicks the top scroll arrow.
case WM_VSCROLL: {  // MSDN: Scroll Bars [Win32];

WORD wId=LOWORD(wParam), wPos=HIWORD(wParam);
switch(wId) {

case SB_THUMBTRACK:{  // IDP_EDGE 
//SendMessage(hWnd,SBM_SETPOS,wPos,true);
SetScrollPos(hWnd,SB_HORZ|SB_VERT,wPos,true);


InvalidateRect(hWnd,0,1);    // �� WM_PAINT �л��� Scroll Pos 

CCW_DATA*pa = (CCW_DATA*)GetWindowLong(hWnd, GWL_USERDATA); 
if (!pa) break;   
//  if (!pa->pSrc) break; 


//\\dipr_Exec(hWnd,(DIPR_PROC)dipr_sp,wPos,0); 



}break;



case SB_THUMBPOSITION: break;  // SendMessage((HWND)lParam,SBM_SETPOS,50,true); //lParam=0L;



default: break;
}//switch(wId);         
return 0;
}break;

  
default: break;  // switch(Message)
}

return DefMDIChildProcW(hWnd,Message,wParam,lParam );
}



























int PASCAL WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int){

// t_inv(); return 0; 

// t_unProject(); return 0;  // Opengl��ʼ��֮��ſ��Բ��� 

// t_Vector(); return 0; 
// t_frac(); 
// return 0; 

// ���� inline ����    qTrace("%d, %d,%d",t_inline(), t_inline(), t_inline());


// 1. ע�ᴰ����.
WNDCLASSEXW wcex= { sizeof(WNDCLASSEXW),CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS, WndProc, 0L, 0L, 
hInst, LoadIcon(0,IDI_APPLICATION),   //   IDI_WINLOGO 
LoadCursor(NULL,IDC_ARROW), 
(HBRUSH)(CTLCOLOR_DLG +1), (LPCWSTR)IDR_MENU1,
WNDCLS_FRAME, NULL };  
if(!RegisterClassExW(&wcex)) {qTrace("RegisterWindowEx,1"); return 0;}


wcex.lpszClassName=WNDCLS_CLIENT;
wcex.lpfnWndProc=ClientChildProc;
wcex.lpszMenuName=0;  // (LPCWSTR)IDR_MENU1; // an MDI child window cannot have its own menu 
if(!RegisterClassExW(&wcex)) {eInfo("RegisterWindowEx,2"); UnregisterClassW(WNDCLS_FRAME,hInst);return FALSE;}



// 2. ��������.
RECT re; GetClientRect(GetDesktopWindow(),&re);  

// int w=re.right-re.left, h=re.bottom-re.top;    int x=w/2, y=h/2; w=2*w/3, h=2*h/3;    x-=w/2, y-=3*h/4;   y=16;  // y-=H/2   

SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&re,0);
int B = GetSystemMetrics(SM_CXBORDER); 

int w=re.right-re.left, h=re.bottom-re.top;   int x=B, y=B; w-=2*B; h-=2*B;  


HWND hWnd=  CreateWindowExW(0,WNDCLS_FRAME,L"Main MDI Window",
  WS_OVERLAPPEDWINDOW|WS_VISIBLE,  // |WS_VSCROLL|WS_HSCROLL,  // WS_EX_APPWINDOW|WS_EX_CLIENTEDGE,
  x,y,w,h, 0L, 0L,hInst,NULL);
if(!hWnd){ UnregisterClassW(WNDCLS_FRAME,hInst);  UnregisterClassW(WNDCLS_CLIENT,hInst); return 0; }


// 3. ע���ȼ� Ctrl+Q, Exit. F1, Help; 
ATOM key_F1, key_CTRL_Q;
key_CTRL_Q = GlobalAddAtom(TEXT("_Hotkey_CTRL_Q"));   
// key_F1 = GlobalAddAtom(TEXT("_Hotkey_F1"));   

//if(!RegisterHotKey(hThisWnd,key_F1,0, VK_F1)) {eInfo(TEXT("RegisterHotKey"));return(0);} 
//If hWnd=NULL, WM_HOTKEY messages are posted to the message queue of the calling thread and must be processed in the message loop. 
RegisterHotKey(NULL,key_CTRL_Q,MOD_CONTROL, 'Q'); 


//hToolBar=InitWindowEx(hInst,L"MDIToolBarClass",L"ToolBar",hThisWnd);
//if(!hToolBar){UnregisterClass("MDIToolBarClass",hInst); UnregisterClass("MDIFrameClass",hInst); UnregisterClass("MDIClientChild",hInst);return FALSE;}
//ShowWindow(hToolBar,SW_SHOW);

//SetTimer(hThisWnd,0x00,1000,NULL); ////KillTimer(hThisWnd, 0x00);

// 4. ��ѭ��.

////  HACCEL hacct=LoadAccelerators(hInst,(LPCTSTR)IDR_ACCELERATOR1);


MSG msg={0};  

#ifdef MDI_3DTEST

// �����ﴴ�� 3D ����  




while (msg.message != WM_QUIT) {  // WM_QUIT = 0x0012


// br=TranslateAccelerator(hThisWnd,hacct,&msg); if(br) continue;  

// br=TranslateMDISysAccel(hClientWnd,&msg); if(br) continue;

// if (!IsDialogMessage(hdlg_op, &msg)) { TranslateMessage(&msg); DispatchMessage(&msg); }




    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {

        if (msg.message == WM_QUIT)  break;

        TranslateMessage(&msg);  DispatchMessage(&msg); // continue; 
    }

// if (g_bPause) { WaitMessage(); continue; }  // ��ͣ 


    // ����֡���� --  Delay(25);   
    static int t = 0, SPF = 16, T = GetTickCount();    // �� SPF=40=1000/25, �� FPS=25. ע�� Loop Ƶ������Ļˢ���ʵ�����, ǰ��ԶԶ���ں���.   
    t = GetTickCount();  if (t - T < SPF) { Sleep(0); continue; }

    T = t;
    


gRender();  // hWnd ���� DT_OG ����!
SwapBuffers( wglGetCurrentDC() );     

Sleep(0);
}  // while




#else

CApplication* app= GetAppPtr();    \
app->Ready(); app->Go();

#endif  // RUN_APP


qLog(0);




//5. ��������;
//KillTimer(hThisWnd, 0x00);
//DestroyWindow(hMainDlg); DestroyWindow(hAddDlg);//EndDialog;

if(bRichEditLoaded) {
////bRichEditLoaded=FreeLibrary(hRichEditDll); //���������ڴ˴�����WM_DESTROY��,I don't know why!
////if(!bRichEditLoaded) MessageBox(NULL,"FreeLibrary","error",0);
}


UnregisterHotKey(NULL, key_CTRL_Q); // ע���˳��ȼ� Ctrl+Q
//UnregisterHotKey(hThisWnd, key_F1); // ɾ���ȼ� F1.

UnregisterClassW(WNDCLS_CLIENT,hInst);
UnregisterClassW(WNDCLS_FRAME,hInst);
return msg.wParam;
}











