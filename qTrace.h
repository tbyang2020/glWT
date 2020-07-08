
 


#define DBG_LOG 

#ifndef _QDBG_H_
#define _QDBG_H_    // #endif  // _QDBG_H_ //

// 结构仅在 QDBG.CPP 中实现
#ifdef  _QDBG_CPP_    // #define _QDBG_CPP_ //
#define QDBG_EXTERN 
#else 
#define QDBG_EXTERN extern 
#endif  // _QDB_CPP_ //

//#define RC_INVOKED  //  ? winnt.h:  #error "No Target Architecture"

#include <Windows.h>      // Windows.h 与 engine.cpp::interface 冲突! 
// #undef DrawText        // Winuser.h:  #define DrawText  DrawTextA, 这样  void CText::DrawText(...)  会受影响 
// #undef GetCharWidth    // Colobot:: GetCharWidth 与 Winuser.h 定义冲突
// #undef LoadImage       // Colobot:: LoadImage 冲突


#include <assert.h>

#include <stdio.h>

#include <winsock.h>


QDBG_EXTERN  CRITICAL_SECTION g_spinlock;   // WM_CREAT 中第一时间初始化. 很多 .cpp 都要使用. 




//#include "InterfaceMDI/utf8.h"

#ifndef  _CONSOLE
#define _WINDOWS
#endif

#define qError  eInfo

#define qdbg_log qLog
#define _qdbg_log qLog



//#define IDC_EDIT1 0x0101  // 确保定义一致, 参见 InterfaceNet.h 
//#define IDC_EDIT2 0x0102


//int show_msg(const char *fmt, ...);   // #define printf  show_msg  


#ifdef _WINDOWS
//#define printf  show_msg  // 不要全文搜索替换 printf. VS2013的替换似乎会更改只读文件. 
#endif  



inline void showMsg(const char*msg, const char*t=0){
    
#ifdef _CONSOLE 

	if(t){ printf(t);	printf(": "); } 
	printf(msg);

#else

#ifdef _WINDOWS
 MessageBox(0, msg, t, 0); 
#else
	int y = 0; 
	if(t){ TextOut(GetDC(0), 0, y, t, strlen(t)); y += 24;	}
	TextOut(GetDC(0),0,y, msg, strlen(msg)); 
#endif

#endif

}


#define eInfo(a)  { DWORD er=GetLastError(); LPTSTR psz;  \
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,0,er,0,(LPTSTR)&psz,0,0);  \
	showMsg(psz,a); LocalFree(psz); }    // static 





inline int eInfoNet(char* a)  {
  int er = WSAGetLastError(); LPTSTR psz;  \
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0, (LPTSTR)&psz, 0, 0);  \
    MessageBox(0, psz, a, 0); LocalFree(psz); return er; }


HWND GetMainWnd();
void t_GetMainWnd(); 





// sTrace, 简单追踪 

#if(_MSC_VER>1200)  // VC6 不支持变参数宏 VC6 = 1200, VS2013 = 1800 , sTrace 只好仅保留一个可变参数  

#define qTrace( a, ... ) { static char g_DBG[1024];    int ip=sprintf(g_szDBG, a, ##__VA_ARGS__ );    \
	const char*p = strrchr(__FILE__, '\\');  if(!p)p = __FILE__;  \
	sprintf(g_DBG + ip, "\t[Line %d in File %s,%s]\r\n", __LINE__, p, __FUNCTION__);   \
showMsg(g_DBG, a);   \
}



//#include "InterfaceMDI/utf8.h"

// 某些源文件,例如 brain.cpp, 编码是 Unicode(UTF-8 无签名) - 代码页 65001 
//inline const char*u82a(const char* sz){  if(!_isu8(sz)) return sz;  return U8ToMultiByte(sz, 0); }



// qTrace 比 sTrace 多出了 GetLastError 的信息.
#define qTrace( ao, ... ) {    \
const char*qTrace_a = ao;  /*u82a(ao);*/    \
static char g_szDBG[1024];  DWORD er=GetLastError(); LPSTR psz=0;  \
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0,(LPTSTR)&psz, 0, 0);  \
	int qTrace_ip =0;  if(qTrace_a) qTrace_ip=sprintf(g_szDBG, qTrace_a, ##__VA_ARGS__);    \
	qTrace_ip += sprintf(g_szDBG+qTrace_ip,"\r\n[ERR %d] %s\r\n",er,psz);  \
	const char*p = strrchr(__FILE__, '\\');  if(!p)p = __FILE__;  \
	sprintf(g_szDBG + qTrace_ip, "\t[Line %d in File %s,%s]\r\n", __LINE__, p, __FUNCTION__);   \
	showMsg(g_szDBG,qTrace_a);    LocalFree(psz);  \
}




// qTrace 比 sTrace 多出了 GetLastError 的信息.  // const wchar_t*p = wcsrchr(__FILE__, '\\'); 

#define qTraceW( ao, ... ) {      \
const wchar_t*qTrace_a = ao;  /*u82a(ao);*/    \
static wchar_t g_szDBG[1024];  DWORD er=GetLastError(); LPWSTR psz=0;  \
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0,(LPWSTR)&psz, 0, 0);    \
  int qTrace_ip =0;  if(qTrace_a) qTrace_ip=swprintf(g_szDBG, qTrace_a, ##__VA_ARGS__);    \
	qTrace_ip += swprintf(g_szDBG+qTrace_ip,L"\r\n[ERR %d] %s\r\n",er,psz);  \
	swprintf(g_szDBG + qTrace_ip, L"\t[ %S  Line %d in File %S]\r\n", __FUNCTION__, __LINE__, __FILE__ );   \
MessageBoxW(0,g_szDBG,qTrace_a,MB_OK);    LocalFree(psz);  \
}












#else  //! _MSC_VER>1200


#define qTrace( a, ti ) {    int ip=sprintf(g_szDbgBuffer, a, ti );    \
	const char*p=strrchr(__FILE__,'\\');  if(!p)p=__FILE__;  \
	sprintf(g_szDbgBuffer+ip, "    \t[Line %d in File %s,%s]\r\n", __LINE__, p, __FUNCTION__);   \
	showMsg(g_szDbgBuffer,a);   \
}



#define qTrace( a, b) {    printf( a, b );    \
	const char*p=strrchr(__FILE__,'\\');  if(!p)p=__FILE__;  \
	printf("\t[Line %d in File %s,%s]\r\n", __LINE__, p, __FUNCTION__);   \
} 



#endif    // #if(_MSC_VER>1200) 





// qdbg_log, 记录/显示 log 文件. qdbg_log(): 关闭log文件, 显示其内容.  
void _qLog(const char*fmt = 0, ...);


//  _qdbg_log,  qdbg_log 添加位置信息.  注意: #define中不可以使用 va_list v; va_start(v, fmt); vsprintf(g_szDbgBuffer, fmt, v); va_end(v); 

// 记录 .scene 文件错误 

#define qLog(fmt,...){ if(!fmt){_qLog(); }else{  const char*QLOG_PTR=strrchr(__FILE__,'\\'); if(!QLOG_PTR) QLOG_PTR=__FILE__; else QLOG_PTR++;   \
static char g_szLog[1024]; 	*g_szLog = 0; sprintf(g_szLog, fmt, ##__VA_ARGS__);   \
_qLog("Ln%d\t%s\t%s\t\t%s\r\n",  __LINE__, QLOG_PTR, __FUNCTION__, g_szLog);  }        \
}   // __TIME__ 只是文件编译时间 






DWORD qmsg(const char*fmt, ...); 
DWORD wmsg(const WORD*fmt, ...);




// __STDC_VERSION__ ,    C99, C++11  // http://hi.baidu.com/pragmatist/item/591a8a4ccbd643086dc2f035 : 
// ##符号在逗号和参数名之间时不做连字符, 而用作变参宏 
// gcc的"变参宏" 在 utf-8 编码的文件内调用时, 会导致中文乱码! 为了代码简单, 这里没处理.
// vs2013怎样规定cpp源文件编码规则? 属性里好像没有. -- 属性应当可以. Disable Language Extensions -- 在[File/Advanced Save Options] !  

// 如果 Linker: Console (/SUBSYSTEM:CONSOLE) 则应当手动定义 _CONSOLE  

// FILE* g_fp;    // Windows (/SUBSYSTEM:WINDOWS), 程序开始时  结束时打开 "qdbg.txt"


 

#define Qdbg_log(a,...) { qTrace( a, ##__VA_ARGS__); }  



// chkTime, 检查模块执行时间. fn为模块(函数)名称, bStart=1 则时间间隔清零. 
// 必须在开始(bStart=true)、结束(bStart=false)各调用一次, 中间不可调用.


// 在宏定义中使用静态变量: 
inline int& getTime(){ static int to; return to; }  //  = GetTickCount(); int t = GetTickCount(); int dt = t - to; to = t;  return dt; }
inline void setTime(int t){ getTime() = t; }
 
#define  chkTime( fn, bStart) {  int t = GetTickCount(); int dt=t-getTime(); setTime(t);  if (bStart) dt = 0;  \
	qTrace("%s \tTime spent %d ms\t", (fn), dt);   \
}



// trace, VC6.0 中无法使用如此方便的定义 ... trace 这个名字太俗了, 不便查找! 
#define trace  qTrace
#define track  qTrace
#define pursue(a,b)  
#define trace_event(a,b)  
#define brk(a,b)  
#define dbg_Loc(a,b)  



// The compiler recognizes six predefined ANSI C macros, ie.  Predefined Macros: 
// __DATE__, __TIME__,  __TIMESTAMP__, __FILE__, __LINE__,      __STDC__
// 注意, __FILE__ 是 const char* 

 
#endif  // _QDBG_H_ //





