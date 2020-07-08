

#pragma warning(disable:4996)


#define  _GLFONT2021_CPP_

#include "glFont2021.h"

#include <Windows.h>
#include <stdio.h>        // vsprintf 

#include <gl\gl.h>
#include <gl\glu.h>


#include "InterfaceMDI/qTrace.h"
 


#include "glWT.h"



 
// WorldToScreen, 调用 gluProject, 把空间坐标转换为屏幕坐标. (x,y,z),空间坐标, xs=(x,y,1),屏幕坐标. 返回 xs.    

int ToScreen(const GLdouble x, const GLdouble y, const GLdouble z, GLdouble* xs, GLdouble* ys){
	if (!xs) return 0L;
	GLdouble md[16], mp[16], zs;    //  ModelView、Projection Matrices 
	GLint vp[4];                    // Viewport  

	glGetDoublev(GL_MODELVIEW_MATRIX, md);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);

return gluProject(x, y, z, md, mp, vp, xs, ys, &zs);  //  -- object coordinates to window coordinates.
}



// ScreenToWorld, 调用 gluUnProject, 把屏幕坐标转换为空间坐标. (x,y),屏幕坐标, z为深度值, xw=(x,y,1),空间坐标. 返回 xw.   

int ToWorld(const double x, const double y, const double z, double* xw, double* yw, double* zw){
	if (!xw) return 0L;
	GLdouble md[16], mp[16];     //  ModelView、Projection Matrices  
	GLint vp[4];                 // Viewport  

	glGetDoublev(GL_MODELVIEW_MATRIX, md);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);
	glGetIntegerv(GL_VIEWPORT, vp);

return gluUnProject(x, y, z, md, mp, vp, xw, yw, zw);    // -- window coordinates to object coordinates. 
}





















// x,y 为屏幕坐标--实际上相当于 Viewport   
int TextOutBm(float x, float y, LPCTSTR fmt , ...){  

if(!fmt) return 0; 

static TCHAR gsz[1024] = { 0 };   gsz[0] = 0;
if (fmt) { va_list ap;  va_start(ap, fmt);   vsprintf(gsz, fmt, ap);  va_end(ap); }




float c[4]={1,1,1,1};
float r=1;  

#if PARSE_CMD|1

char *p = gsz;    // int ip = -1;  // 截断点 


do {
    p = strchr(p, '/');    // 不检查 '//'
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

static HFONT m_hFont = CreateFont(-16, 0, 0, 0, 800, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, FF_MODERN, TEXT("华文楷体"));   // 默认字体  -12-2*12

if (! m_hFont) {  // 备份而已以供参考 
LOGFONT lf = { 16, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, { 0 }, };
lstrcpy(lf.lfFaceName, "楷体");   // 楷体 黑体  华文楷体  Times New Roman  
m_hFont = CreateFontIndirect(&lf);
}


HFONT hof = (HFONT)SelectObject(g_hDC, m_hFont);    // 选入字体	

#endif  //  USE_FONT







// HWND hWnd=WindowFromDC(g_hDC);  

int ic = strlen(gsz);
if (ic <= 0) return 0;

SIZE size;  ::GetTextExtentPoint32(g_hDC, gsz, ic, &size);  // 得到宽高 -- 与 mdc 情况不同,  与m_hFont 有关. 

#if  GETBITS_BACKUP|1

//	m_uTexture[0]=LoadTexture(TEXT("wall.bmp"));   CreateTexture( m_uTexture[1], TEXT("wall.bmp")); 
static HDC mdc=CreateCompatibleDC(g_hDC);    // 0--> wglGetCurrentDC(),  建立当前屏幕相关的 DC    0 = current screen

// HFONT hof = (HFONT)SelectObject(mdc, m_hFont);    // 选入字体		
// SIZE size;    ::GetTextExtentPoint32(mdc, m_szText, ic, &size);  // 得到宽高.  size.cx += 4;  // 好像宽度值太小. 必须是32的倍数! 

	HBITMAP hBitmap = CreateBitmap(size.cx, size.cy, 1, 1, 0L);   // 创建位图 	
	HBITMAP hobmp = (HBITMAP)SelectObject(mdc, hBitmap);   // 选入位图

	SetBkColor(mdc,0);  SetTextColor(mdc, 0xFFFFFF);      // SetBkMode(mDC,TRANSPARENT);  // // 设置文字背景色, 文字颜色		
	TextOut(mdc, 0, 0, gsz, ic);  // DC中输出文字 

	BITMAP bm; GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

	size.cx = (bm.bmWidth + 31) & (~31);  //  得到32的倍数		
	int iBf = size.cy * size.cx;

	BITMAPINFOHEADER bih = { sizeof(bih), bm.bmWidth, bm.bmHeight, 1, 1, BI_RGB, iBf, 0, 0, 0, 0, };
	struct { BITMAPINFOHEADER bih;  RGBQUAD col[2]; }bic;   // [in,out] 

	BITMAPINFO *binf = (BITMAPINFO *)&bic;
	binf->bmiHeader = bih;
	
static UCHAR* Bits = new UCHAR[iBf];  // 分配内存		
	::GetDIBits(mdc, hBitmap, 0, bm.bmHeight, Bits, binf, DIB_RGB_COLORS);  // 得到图像 

#endif  //  GETBITS_BACKUP








	glPushAttrib(GL_CURRENT_BIT);
	bool bL = glIsEnabled(GL_LIGHTING);  if (bL) glDisable(GL_LIGHTING);
	bool bT = glIsEnabled(GL_TEXTURE_2D);  if(bT) glDisable(GL_TEXTURE_2D);   // 必须, 否则文字四周颜色都受影响, 或者文字颜色不完全是所期望的,  或者不同区域文字亮度不同 


glColor3f(c[0],c[1],c[2]);  // √ 

// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


#if CHANGE_WT|1


// glColor3b(co & 0x7E, (co >> 8) & 0x7E, (co >> 16) & 0x7E);  // 127=0x7F
// glColor3b(c & 0x7F, (c >> 8) & 0x7F, (c >> 16) & 0x7F);  // √, 0x7E, 0xFF不行.   

// glColor3b(127, 127, 127);    // √, 128不行.   


//glMatrixMode(GL_MODELVIEW);   // GL_TEXTURE  // 
//glPushMatrix();
//glScalef(1., 2., 1.);  //  glLoadIdentity();  // 无法影响到字体大小 

GLdouble xw = x, yw = y, zw = 1;
ToWorld(x, y, 1, &xw, &yw, &zw);     
glRasterPos3d(xw, yw, zw);          // 设置光栅位置: GL_CURRENT_RASTER_POSITION_VALID                  

// glRasterPos 的参数是世界坐标
// 1、glWindowPos 函数允许你通过设置屏幕坐标来改变光栅坐标, 但需要引入 glew 库
//  2、通过 glUnProject 计算出屏幕坐标对应的世界坐标


// float xs[4]={x,y, 1,1}, xo[4]={x,y,1,1};   ToWorld(xs,xo);   // xs 好像相对于 Viewport 才可以 -- 要么 ToWorld 没处理好 xs (到底是屏幕还是窗口Screen), 要么  glRasterPos3d 映射没有考虑 Viewport 到窗口的转换!
// glRasterPos3d(xo[0], xo[1], xo[2]);    // 设置光栅位置: GL_CURRENT_RASTER_POSITION_VALID                  

#endif   // CHANGE_WT


glBitmap(size.cx, size.cy, 0, 0, 0, 0, Bits);    // 在RC中输出文字
    
	// glPopMatrix();

	// glDrawPixels(size.cx,size.cy,GL_GREEN, GL_UNSIGNED_BYTE,Bits);

	if (bL) glEnable(GL_LIGHTING);
	if (bT) glEnable(GL_TEXTURE_2D);  // ...... 判断才予以 Enable 
	glPopAttrib();

SelectObject(mdc, hobmp);   \
SelectObject(mdc, hof);

//::DeleteDC(mdc);    // mdc 不可以删除 ! 
	
// delete Bits;
return ic;
}





 

// 取得字符串的纹理 
UINT makeTexture(LPCTSTR szText, DWORD co){
 
static HFONT m_hFont=CreateFont(-16,0,0,0,800,0,0,0,GB2312_CHARSET,0,0,0,FF_MODERN,TEXT("华文楷体")); ;  


 HDC mdc = ::CreateCompatibleDC(0);    // 0--> wglGetCurrentDC(),  建立当前屏幕相关的 DC
 SelectObject(mdc,m_hFont);    // 选入字体  
 SIZE size;  ::GetTextExtentPoint32(mdc,szText,lstrlen(szText),&size);  // 得到宽高
 
 HBITMAP hBitmap=CreateBitmap(size.cx, size.cy, 1, 1, 0L);   // 创建位图  
 SelectObject(mdc,hBitmap);   // 选入位图
 SetTextColor(mdc,0x00FFFF);    SetBkColor(mdc,0);   // SetBkMode(mDC,TRANSPARENT);  // // 设置文字背景色, 文字颜色  
 TextOut(mdc, 0, 0, szText, lstrlen(szText));  // DC中输出文字

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

int biClrUsed=pbih->biClrUsed, bi=pbih->biBitCount; assert(biClrUsed==2);   // 进一步调用 GetDIBits会修改 pbi ！ pbih->biClrUsed会变为0
int w=pbih->biWidth, h=pbih->biHeight;
int wa= ( (w*bi+31)&~31 ) /bi;        //  行字节数.   ((w*bi+31)/32)*(32/8)      //  WIDTHBYTES(bits)  ((((bits) + 31)>>5)<<2)     // ((bits+31)/32)*(32/8)  //  ((bits+31)&~31)/8
int wb= ( (w*24+31)&~31 ) /24;        //  ( (24*w+31)/32 )*32 /24; //            //  24 位真彩图像数据的行也存在补齐问题,
int qa=( 4-(3*w)%4 )%4;               // *8 bits, q 需要补齐的字节数
//assert(qa==(wb-w)*3);
int bc=  wa*h/8;  // pbih->biSizeImage;  // biSizeImage 是  biWidth 模化到 32 后, biWidth*biHeight 的值. 
int bcc= wb*h/8*24;  //  bc*(24/bi); // //  for each (actual) pixel, 24 bits, 24/bi bytes.  

 

LPBYTE pbits = (LPBYTE)GlobalAlloc(GMEM_FIXED, bcc );  assert(pbits); // memory pointer
//pbih->biClrImportant=0;
if(!GetDIBits(mdc,hBitmap,0,(WORD)pbih->biHeight,pbits,pbi,DIB_RGB_COLORS)){ MessageBox(0,"GetDIBits",0,0);  GlobalFree((HGLOBAL)pbits); return false;}
// assert(  *(COLORREF*)&pbi->bmiColors[1]==0xFFFFFF  );


if(pbih->biBitCount==1){     // 1位, 2色位图, 每一个 Bit 代表一个像素  --- 没有办法使得指针(地址) 每次移动一个bit ！

RGBQUAD* ct= pbi->bmiColors;
struct RGB{BYTE r;BYTE g;BYTE b;};  // 3-BYTE data structure.

BYTE*p=pbits+bc, *q=pbits+bcc;  // 指向最后一个BYTE.  
 
// 需要遍历 pbits, 步长为 biBitCount位! 逐个 bit 遍历有点麻烦!

int i,pi=0,ip=0, ic=w;    // 从最后一个 byte 开始  

//TCHAR sz[128]; sprintf(sz,TEXT("qa=%d, wa=%d, wb=%d, w=%d, h=%d, bc=biSizeImage=%d, w*h/8=%d, wa*h/8=%d, wb*h/8=%d, bcc=%d, co[0]=%X, co[1]=%X,  %X | %X"), \
qa,wa, wb, w, h, pbih->biSizeImage,   w*h/8,  wa*h/8, wb*h/8,  bcc,   *(COLORREF*)&pbi->bmiColors[0],  *(COLORREF*)&pbi->bmiColors[1], pbits[0],pbits[1]);  \
MessageBox(0,sz,szText,0);

//int iq=w;
while(p>pbits){ p--;    // q-=3;   // 24位图像数据的行也存在补齐问题 ? 
pi=*p;
for(i=0; i<8; i++){

ic++;  if(ic>w){  if(ic>wa){ ic=1;  q-=0; } else{ continue; } }   // 移位控制: 裁去填补位.  如果w是4的倍数, 则q无需填补!

ip=pi&1;  pi>>=1;   // ib=~ib&1;  //         (*p&(1<<i)) ;    >>=

//iq++;  if(iq>w) {  iq=1; q-=qa; }        

q-=3;        // *(RGB*)q= *(RGB*)&ct[ip];
q[0]=ct[ip].rgbRed; q[1]=ct[ip].rgbGreen;  q[2]=ct[ip].rgbBlue;  // 上句颠倒 rgb为 bgr, 原因, RGBQUAD 是 gbr0. 
}  // for i

}  //  while(p>pbits) 


//assert(p==pbits && q==pbits);
}  // if(bih.biBitCount==1)

 

glPushAttrib(GL_CURRENT_BIT); 
 
 bool bL=glIsEnabled(GL_LIGHTING);  if(bL) glDisable(GL_LIGHTING);
 //glDisable(GL_TEXTURE_2D);  // 必须, 否则文字四周颜色都受影响, 或者文字颜色不完全是所期望的,  或者不同区域文字亮度不同
 glEnable(GL_TEXTURE_2D);


UINT iTex=0;

glGenTextures(1, &iTex);     // 生成纹理
glPixelStorei (GL_UNPACK_ALIGNMENT, 1);     // 设置像素格式
glBindTexture(GL_TEXTURE_2D, iTex);   // 捆绑纹理


TCHAR sz[128]; sprintf(sz,TEXT("wb=%d, w=%d, h=%d, biSizeImage=%d,  co[0]=%X, co[1]=%X,  %X | %X"), \
wb, w, h, pbih->biSizeImage, *(COLORREF*)&pbi->bmiColors[0],  *(COLORREF*)&pbi->bmiColors[1], pbits[0],pbits[1]);  \
MessageBox(0,sz,szText,0);


gluBuild2DMipmaps(GL_TEXTURE_2D, 3, wb,h, GL_RGB, GL_UNSIGNED_BYTE, pbits);  // GL_BGR_EXT


glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


// SaveDIB(bm,Bits,TEXT("test.bmp"));

// glDrawPixels(size.cx,size.cy,GL_GREEN, GL_UNSIGNED_BYTE,Bits);

 if(bL) glEnable(GL_LIGHTING); 
 //glEnable(GL_TEXTURE_2D);  // ...... 判断才予以 Enable
glPopAttrib(); 


LocalFree(pbi);    // Free memory.
GlobalFree((HGLOBAL)pbits);


// delete Bits;    

// SelectObject(mdc, oldBmp);  SelectObject(mdc,hof); 

 ::DeleteDC(mdc);
 

 assert(iTex>0);
 return iTex;
 
 }

 









 
