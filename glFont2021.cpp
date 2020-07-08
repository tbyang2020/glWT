

#pragma warning(disable:4996)


#define  _GLFONT2021_CPP_

#include "glFont2021.h"

#include <Windows.h>
#include <stdio.h>        // vsprintf 

#include <gl\gl.h>
#include <gl\glu.h>


#include "InterfaceMDI/qTrace.h"
 


#include "glWT.h"



 
// WorldToScreen, ���� gluProject, �ѿռ�����ת��Ϊ��Ļ����. (x,y,z),�ռ�����, xs=(x,y,1),��Ļ����. ���� xs.    

int ToScreen(const GLdouble x, const GLdouble y, const GLdouble z, GLdouble* xs, GLdouble* ys){
	if (!xs) return 0L;
	GLdouble md[16], mp[16], zs;    //  ModelView��Projection Matrices 
	GLint vp[4];                    // Viewport  

	glGetDoublev(GL_MODELVIEW_MATRIX, md);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);

return gluProject(x, y, z, md, mp, vp, xs, ys, &zs);  //  -- object coordinates to window coordinates.
}



// ScreenToWorld, ���� gluUnProject, ����Ļ����ת��Ϊ�ռ�����. (x,y),��Ļ����, zΪ���ֵ, xw=(x,y,1),�ռ�����. ���� xw.   

int ToWorld(const double x, const double y, const double z, double* xw, double* yw, double* zw){
	if (!xw) return 0L;
	GLdouble md[16], mp[16];     //  ModelView��Projection Matrices  
	GLint vp[4];                 // Viewport  

	glGetDoublev(GL_MODELVIEW_MATRIX, md);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);

return gluUnProject(x, y, z, md, mp, vp, xw, yw, zw);    // -- window coordinates to object coordinates. 
}





















// x,y Ϊ��Ļ����--ʵ�����൱�� Viewport   
int TextOutBm(float x, float y, LPCTSTR fmt , ...){  

if(!fmt) return 0; 

static TCHAR gsz[1024] = { 0 };   gsz[0] = 0;
if (fmt) { va_list ap;  va_start(ap, fmt);   vsprintf(gsz, fmt, ap);  va_end(ap); }




float c[4]={1,1,1,1};
float r=1;  

#if PARSE_CMD|1

char *p = gsz;    // int ip = -1;  // �ضϵ� 


do {
    p = strchr(p, '/');    // ����� '//'
    if (!p) break;
 
    p++;   

    switch (*p) {
    case 'r':  c[0] = atof(p + 1);  break;    // wcstol(p + 1, 0, 16)
    case 'g':  c[1] = atof(p + 1);  break;
    case 'b':  c[2] = atof(p + 1);  break;
    case 'a':  c[3] = atof(p + 1);  break;
    case 's':     r = atof(p + 1);  break;

    default: break;
    }

    if (*p == 0) break;

} while (p);

p = strchr(gsz, '/');  if (p) *p = 0;

#endif  //  PARSE_CMD




HDC g_hDC = wglGetCurrentDC();


#if USE_FONT|1

static HFONT m_hFont = CreateFont(-16, 0, 0, 0, 800, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, FF_MODERN, TEXT("���Ŀ���"));   // Ĭ������  -12-2*12

if (! m_hFont) {  // ���ݶ����Թ��ο� 
LOGFONT lf = { 16, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, { 0 }, };
lstrcpy(lf.lfFaceName, "����");   // ���� ����  ���Ŀ���  Times New Roman  
m_hFont = CreateFontIndirect(&lf);
}


HFONT hof = (HFONT)SelectObject(g_hDC, m_hFont);    // ѡ������	

#endif  //  USE_FONT







// HWND hWnd=WindowFromDC(g_hDC);  

int ic = strlen(gsz);
if (ic <= 0) return 0;

SIZE size;  ::GetTextExtentPoint32(g_hDC, gsz, ic, &size);  // �õ���� -- �� mdc �����ͬ,  ��m_hFont �й�. 

#if  GETBITS_BACKUP|1

//	m_uTexture[0]=LoadTexture(TEXT("wall.bmp"));   CreateTexture( m_uTexture[1], TEXT("wall.bmp")); 
static HDC mdc=CreateCompatibleDC(g_hDC);    // 0--> wglGetCurrentDC(),  ������ǰ��Ļ��ص� DC    0 = current screen

// HFONT hof = (HFONT)SelectObject(mdc, m_hFont);    // ѡ������		
// SIZE size;    ::GetTextExtentPoint32(mdc, m_szText, ic, &size);  // �õ����.  size.cx += 4;  // ������ֵ̫С. ������32�ı���! 

	HBITMAP hBitmap = CreateBitmap(size.cx, size.cy, 1, 1, 0L);   // ����λͼ 	
	HBITMAP hobmp = (HBITMAP)SelectObject(mdc, hBitmap);   // ѡ��λͼ

	SetBkColor(mdc,0);  SetTextColor(mdc, 0xFFFFFF);      // SetBkMode(mDC,TRANSPARENT);  // // �������ֱ���ɫ, ������ɫ		
	TextOut(mdc, 0, 0, gsz, ic);  // DC��������� 

	BITMAP bm; GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

	size.cx = (bm.bmWidth + 31) & (~31);  //  �õ�32�ı���		
	int iBf = size.cy * size.cx;

	BITMAPINFOHEADER bih = { sizeof(bih), bm.bmWidth, bm.bmHeight, 1, 1, BI_RGB, iBf, 0, 0, 0, 0, };
	struct { BITMAPINFOHEADER bih;  RGBQUAD col[2]; }bic;   // [in,out] 

	BITMAPINFO *binf = (BITMAPINFO *)&bic;
	binf->bmiHeader = bih;
	
static UCHAR* Bits = new UCHAR[iBf];  // �����ڴ�		
	::GetDIBits(mdc, hBitmap, 0, bm.bmHeight, Bits, binf, DIB_RGB_COLORS);  // �õ�ͼ�� 

#endif  //  GETBITS_BACKUP








	glPushAttrib(GL_CURRENT_BIT);
	bool bL = glIsEnabled(GL_LIGHTING);  if (bL) glDisable(GL_LIGHTING);
	bool bT = glIsEnabled(GL_TEXTURE_2D);  if(bT) glDisable(GL_TEXTURE_2D);   // ����, ��������������ɫ����Ӱ��, ����������ɫ����ȫ����������,  ���߲�ͬ�����������Ȳ�ͬ 


glColor3f(c[0],c[1],c[2]);  // �� 

// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


#if CHANGE_WT|1


// glColor3b(co & 0x7E, (co >> 8) & 0x7E, (co >> 16) & 0x7E);  // 127=0x7F
// glColor3b(c & 0x7F, (c >> 8) & 0x7F, (c >> 16) & 0x7F);  // ��, 0x7E, 0xFF����.   

// glColor3b(127, 127, 127);    // ��, 128����.   


//glMatrixMode(GL_MODELVIEW);   // GL_TEXTURE  // 
//glPushMatrix();
//glScalef(1., 2., 1.);  //  glLoadIdentity();  // �޷�Ӱ�쵽�����С 

GLdouble xw = x, yw = y, zw = 1;
ToWorld(x, y, 1, &xw, &yw, &zw);     
glRasterPos3d(xw, yw, zw);          // ���ù�դλ��: GL_CURRENT_RASTER_POSITION_VALID                  

// glRasterPos �Ĳ�������������
// 1��glWindowPos ����������ͨ��������Ļ�������ı��դ����, ����Ҫ���� glew ��
//  2��ͨ�� glUnProject �������Ļ�����Ӧ����������


// float xs[4]={x,y, 1,1}, xo[4]={x,y,1,1};   ToWorld(xs,xo);   // xs ��������� Viewport �ſ��� -- Ҫô ToWorld û����� xs (��������Ļ���Ǵ���Screen), Ҫô  glRasterPos3d ӳ��û�п��� Viewport �����ڵ�ת��!
// glRasterPos3d(xo[0], xo[1], xo[2]);    // ���ù�դλ��: GL_CURRENT_RASTER_POSITION_VALID                  

#endif   // CHANGE_WT


glBitmap(size.cx, size.cy, 0, 0, 0, 0, Bits);    // ��RC���������
    
	// glPopMatrix();

	// glDrawPixels(size.cx,size.cy,GL_GREEN, GL_UNSIGNED_BYTE,Bits);

	if (bL) glEnable(GL_LIGHTING);
	if (bT) glEnable(GL_TEXTURE_2D);  // ...... �жϲ����� Enable 
	glPopAttrib();

SelectObject(mdc, hobmp);   \
SelectObject(mdc, hof);

//::DeleteDC(mdc);    // mdc ������ɾ�� ! 
	
// delete Bits;
return ic;
}





 

// ȡ���ַ��������� 
UINT makeTexture(LPCTSTR szText, DWORD co){
 
static HFONT m_hFont=CreateFont(-16,0,0,0,800,0,0,0,GB2312_CHARSET,0,0,0,FF_MODERN,TEXT("���Ŀ���")); ;  


 HDC mdc = ::CreateCompatibleDC(0);    // 0--> wglGetCurrentDC(),  ������ǰ��Ļ��ص� DC
 SelectObject(mdc,m_hFont);    // ѡ������  
 SIZE size;  ::GetTextExtentPoint32(mdc,szText,lstrlen(szText),&size);  // �õ����
 
 HBITMAP hBitmap=CreateBitmap(size.cx, size.cy, 1, 1, 0L);   // ����λͼ  
 SelectObject(mdc,hBitmap);   // ѡ��λͼ
 SetTextColor(mdc,0x00FFFF);    SetBkColor(mdc,0);   // SetBkMode(mDC,TRANSPARENT);  // // �������ֱ���ɫ, ������ɫ  
 TextOut(mdc, 0, 0, szText, lstrlen(szText));  // DC���������

//CreateBMPFile(TEXT("m_test.bmp"),hBitmap,mdc); 

//\\ SaveDDB(hBitmap, mdc,TEXT("m_test.bmp"));

//SnapShot(WindowFromDC(wglGetCurrentDC()),0,  TEXT("m_test.bmp"));

 
BITMAP bm;  if(!GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm)) MessageBox(0,"GetObject",0,0);//errhandler("GetObject");
assert(bm.bmBitsPixel==1);

//DIBSECTION bs={0};  if(!GetObject(hBitmap, sizeof(bs), (LPSTR)&bs)) MessageBox(0,"GetObject",0,0);//errhandler("GetObject"); 
//assert(bs.dsBmih.biBitCount==1);

struct { BITMAPINFOHEADER bih;  RGBQUAD col[2]; }bic={0};   // [in,out]
PBITMAPINFO pbi=(PBITMAPINFO)&bic;
PBITMAPINFOHEADER  pbih = (PBITMAPINFOHEADER) pbi;

memset(pbi,0,sizeof(bic));   // GetDIBits, The bitmap identified by the hBitmap parameter must not be selected into a device context when the application calls this function.
pbih->biSize=sizeof(BITMAPINFOHEADER);  pbih->biBitCount=0;    // Query ......


if(!GetDIBits(mdc,hBitmap,0,bm.bmHeight,0,pbi,DIB_RGB_COLORS)){ MessageBox(0,"GetDIBits",0,0);  return false;}

int biClrUsed=pbih->biClrUsed, bi=pbih->biBitCount; assert(biClrUsed==2);   // ��һ������ GetDIBits���޸� pbi �� pbih->biClrUsed���Ϊ0
int w=pbih->biWidth, h=pbih->biHeight;
int wa= ( (w*bi+31)&~31 ) /bi;        //  ���ֽ���.   ((w*bi+31)/32)*(32/8)      //  WIDTHBYTES(bits)  ((((bits) + 31)>>5)<<2)     // ((bits+31)/32)*(32/8)  //  ((bits+31)&~31)/8
int wb= ( (w*24+31)&~31 ) /24;        //  ( (24*w+31)/32 )*32 /24; //            //  24 λ���ͼ�����ݵ���Ҳ���ڲ�������,
int qa=( 4-(3*w)%4 )%4;               // *8 bits, q ��Ҫ������ֽ���
//assert(qa==(wb-w)*3);
int bc=  wa*h/8;  // pbih->biSizeImage;  // biSizeImage ��  biWidth ģ���� 32 ��, biWidth*biHeight ��ֵ. 
int bcc= wb*h/8*24;  //  bc*(24/bi); // //  for each (actual) pixel, 24 bits, 24/bi bytes.  

 

LPBYTE pbits = (LPBYTE)GlobalAlloc(GMEM_FIXED, bcc );  assert(pbits); // memory pointer
//pbih->biClrImportant=0;
if(!GetDIBits(mdc,hBitmap,0,(WORD)pbih->biHeight,pbits,pbi,DIB_RGB_COLORS)){ MessageBox(0,"GetDIBits",0,0);  GlobalFree((HGLOBAL)pbits); return false;}
// assert(  *(COLORREF*)&pbi->bmiColors[1]==0xFFFFFF  );


if(pbih->biBitCount==1){     // 1λ, 2ɫλͼ, ÿһ�� Bit ����һ������  --- û�а취ʹ��ָ��(��ַ) ÿ���ƶ�һ��bit ��

RGBQUAD* ct= pbi->bmiColors;
struct RGB{BYTE r;BYTE g;BYTE b;};  // 3-BYTE data structure.

BYTE*p=pbits+bc, *q=pbits+bcc;  // ָ�����һ��BYTE.  
 
// ��Ҫ���� pbits, ����Ϊ biBitCountλ! ��� bit �����е��鷳!

int i,pi=0,ip=0, ic=w;    // �����һ�� byte ��ʼ  

//TCHAR sz[128]; sprintf(sz,TEXT("qa=%d, wa=%d, wb=%d, w=%d, h=%d, bc=biSizeImage=%d, w*h/8=%d, wa*h/8=%d, wb*h/8=%d, bcc=%d, co[0]=%X, co[1]=%X,  %X | %X"), \
qa,wa, wb, w, h, pbih->biSizeImage,   w*h/8,  wa*h/8, wb*h/8,  bcc,   *(COLORREF*)&pbi->bmiColors[0],  *(COLORREF*)&pbi->bmiColors[1], pbits[0],pbits[1]);  \
MessageBox(0,sz,szText,0);

//int iq=w;
while(p>pbits){ p--;    // q-=3;   // 24λͼ�����ݵ���Ҳ���ڲ������� ? 
pi=*p;
for(i=0; i<8; i++){

ic++;  if(ic>w){  if(ic>wa){ ic=1;  q-=0; } else{ continue; } }   // ��λ����: ��ȥ�λ.  ���w��4�ı���, ��q�����!

ip=pi&1;  pi>>=1;   // ib=~ib&1;  //         (*p&(1<<i)) ;    >>=

//iq++;  if(iq>w) {  iq=1; q-=qa; }        

q-=3;        // *(RGB*)q= *(RGB*)&ct[ip];
q[0]=ct[ip].rgbRed; q[1]=ct[ip].rgbGreen;  q[2]=ct[ip].rgbBlue;  // �Ͼ�ߵ� rgbΪ bgr, ԭ��, RGBQUAD �� gbr0. 
}  // for i

}  //  while(p>pbits) 


//assert(p==pbits && q==pbits);
}  // if(bih.biBitCount==1)

 

glPushAttrib(GL_CURRENT_BIT); 
 
 bool bL=glIsEnabled(GL_LIGHTING);  if(bL) glDisable(GL_LIGHTING);
 //glDisable(GL_TEXTURE_2D);  // ����, ��������������ɫ����Ӱ��, ����������ɫ����ȫ����������,  ���߲�ͬ�����������Ȳ�ͬ
 glEnable(GL_TEXTURE_2D);


UINT iTex=0;

glGenTextures(1, &iTex);     // ��������
glPixelStorei (GL_UNPACK_ALIGNMENT, 1);     // �������ظ�ʽ
glBindTexture(GL_TEXTURE_2D, iTex);   // ��������


TCHAR sz[128]; sprintf(sz,TEXT("wb=%d, w=%d, h=%d, biSizeImage=%d,  co[0]=%X, co[1]=%X,  %X | %X"), \
wb, w, h, pbih->biSizeImage, *(COLORREF*)&pbi->bmiColors[0],  *(COLORREF*)&pbi->bmiColors[1], pbits[0],pbits[1]);  \
MessageBox(0,sz,szText,0);


gluBuild2DMipmaps(GL_TEXTURE_2D, 3, wb,h, GL_RGB, GL_UNSIGNED_BYTE, pbits);  // GL_BGR_EXT


glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


// SaveDIB(bm,Bits,TEXT("test.bmp"));

// glDrawPixels(size.cx,size.cy,GL_GREEN, GL_UNSIGNED_BYTE,Bits);

 if(bL) glEnable(GL_LIGHTING); 
 //glEnable(GL_TEXTURE_2D);  // ...... �жϲ����� Enable
glPopAttrib(); 


LocalFree(pbi);    // Free memory.
GlobalFree((HGLOBAL)pbits);


// delete Bits;    

// SelectObject(mdc, oldBmp);  SelectObject(mdc,hof); 

 ::DeleteDC(mdc);
 

 assert(iTex>0);
 return iTex;
 
 }

 









 
