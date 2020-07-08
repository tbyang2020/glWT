

#define _QDBG_CPP_  // 否则 g_szDbgBuffer 不被初始化 


//#define _XKEYCHECK_H  
#pragma warning(disable:4996)  // 避免提示  sprintf 替换为 sprintf_s 等等. 或者提前 #define _CRT_SECURE_NO_DEPRECATE 

#include <Windows.h>
#include <stdio.h>

#include "qTrace.h"  // #define ...




#if GET_MAINWND|1

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){ 
	DWORD cPId = *((DWORD*)lParam); 
	DWORD PId = 0; 
 
	GetWindowThreadProcessId(hwnd, &PId); 
	if(PId == cPId && GetParent(hwnd) == NULL)
	{ 
		*((HWND *)lParam) = hwnd;
		return FALSE;  // stop  
	} 
	return TRUE;  // continue 
} 
 
 
HWND GetMainWnd() { 
	DWORD cPId = GetCurrentProcessId();
	if(!EnumWindows(EnumWindowsProc, (LPARAM)&cPId)) return (HWND)cPId;  // 但是 EnumWindows 失败时也返回 false! 
	return NULL; 
} 


void t_GetMainWnd(){

  HWND hWnd = GetMainWnd();

  if(!hWnd) { MessageBox(0, "No window!", 0, 0); return; }

  char szt[128]; GetWindowText(hWnd, szt, 128);
  MessageBox(0, szt, "t_GetMainWnd", MB_OK);

}

#endif  // GET_MAINWND



// show_msg, 命令开关标志 "//id:257", 表示 id=257. 默认 id=IDC_EDIT1=0x0101. Win32程序中替代 printf  
//
// 线程安全, 可以确保内容被显示时不会被其他线程修改. 
//
 



//#if !defined(_CONSOLE) 

// _qLog, qdbg_log, 记录/显示 log 文件.   
void _qLog(const char*fmt, ...) {   

static int ic; ic++; 


#if USE_LOG|1

static FILE*g_fp=0;   


  if(!fmt){ 	//if(!g_fp) g_fp = fopen("qdbg_log.txt", "w"); else{     
    if(g_fp)
    {        
    fclose(g_fp); g_fp = 0L;  // 关闭 qLog.txt 文件, 显示其内容.  
	  if((int)ShellExecute(0, "open", "qdbg.log", 0, 0, SW_SHOW)<=32) eInfo("ShellExecute");       
    
    Sleep(1000); // 1秒后删除 qLog.txt, ......  
    DeleteFile("qdbg.log");  // 消除痕迹 
    }  // if...else

    return;      
  } 	// if(!fmt)    
	

// 记录调试信息 
if (!g_fp)g_fp = fopen("qdbg.log", "w");   

static char po[1024];   
va_list v; va_start(v, fmt); vsprintf(po, fmt, v); va_end(v);
fprintf(g_fp, "\r\n%d\t ", ic);  
fprintf(g_fp, po);  
fprintf(g_fp, "\r\n");  
fflush(g_fp); 

#endif  // USE_LOG 
}  // _qLog 



//#endif  // #if !defined(_CONSOLE) 





// chkTime, 检查模块执行时间. fn为模块(函数)名称, bStart=1 则时间间隔清零. 
// 必须在开始(bStart=true)、结束(bStart=false)各调用一次, 中间不可调用.





DWORD qmsg(const char*fmt, ...)  {
	static char szt[1024];  char *p = szt; *p = 0;
	DWORD er = GetLastError();  // if (er == 0) return er;

	if (fmt){ va_list v; va_start(v, fmt); p+=vsprintf(p, fmt, v); va_end(v); }

	if (!(er == 0)){  // || er == 0x274c 过滤掉常见错误: 274c=超时. 2745=对方放弃连接. 2736=无效套接字.  
		LPSTR psz; FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0, (LPSTR)&psz, 0, 0);
		p += sprintf(p, "[Error %X]\t%s\r\n ", er,psz);  LocalFree(psz);
	}  // if( !(..) ) 

	MessageBoxA(0, szt, "qmsg", 0);
	return er;
}

DWORD wmsg(const WORD*fmt, ...)  {
	static WCHAR szt[1024];  LPWSTR p = (LPWSTR)szt; *p = 0;
	DWORD er = GetLastError();  // if (er == 0) return er;

	if (fmt){ va_list v; va_start(v, fmt);  p+=vswprintf((wchar_t*)p, (const wchar_t*)fmt, v); va_end(v); }

	if (!(er == 0)){    // || er == 0x274c 过滤掉常见错误: 274c=超时. 2745=对方放弃连接. 2736=无效套接字.  
		LPWSTR psz; FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0, (LPWSTR)&psz, 0, 0);
		p += swprintf(p, L"[Error %X]\t%s\r\n ", er, psz);  LocalFree(psz);
	}  // if( !(..) ) 

#if defined(_CONSOLE)   // Win32 Console Application
wprintf(L"%s\r\n", szt);
#else
MessageBoxW(0, szt, L"wdbg", 0);
#endif

return er;
}












void dbg_Log( const char*fmt, ... )  {  

	if (fmt){ va_list v; va_start(v, fmt); vprintf(fmt, v); va_end(v); }
	 
	static int g_tik = GetTickCount();    \
	static int ctik;  ctik = GetTickCount();    \
	int dt= ctik- g_tik;  g_tik = ctik; \
	printf("间隔 %d 毫秒.\r\n", dt);  \
} 



DWORD qdbg(const char*fmt, ...)  {    static char szt[1024];  char *p = szt; *p = 0;

	DWORD er = GetLastError();  // if (er == 0) return er;

	p += sprintf(p, "[%4X] ", er);

	if ( !(er == 0) ){   // || er == 0x274c 过滤掉常见错误: 274c=超时. 2745=对方放弃连接. 2736=无效套接字.  
		LPSTR psz; FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0, (LPSTR)&psz, 0, 0);
		p += sprintf(p, "\t%s\r\n ", psz);  LocalFree(psz);
	}  // if( !(..) ) 

	if (fmt){ va_list v; va_start(v, fmt); vsprintf(p, fmt, v); va_end(v); }

#ifdef M_FPTR   // log file pointer  FILE*,  #define  M_FPTR  g_fptr 
	fputs(szt, M_FPTR);
#elif defined(_CONSOLE)   // Win32 Console Application
  printf("%s\r\n", szt);
#else
  MessageBox(0,szt,"qdbg",0);
#endif
	
return er;
}






DWORD wdbg(const WORD*fmt, ...)  {
	static WCHAR szt[1024];  LPWSTR p = (LPWSTR)szt; *p = 0;

	DWORD er = GetLastError();  // if (er == 0) return er;

	p += swprintf(p, L"[%4X] ", er);

	if (!(er == 0)){   // || er == 0x274c 过滤掉常见错误: 274c=超时. 2745=对方放弃连接. 2736=无效套接字.  
		LPWSTR psz; FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, er, 0, (LPWSTR)&psz, 0, 0);
		p += swprintf(p, L"\t%s\r\n ", psz);  LocalFree(psz);
	}  // if( !(..) ) 

	if (fmt){ va_list v; va_start(v, fmt);  vswprintf((wchar_t*)p, (const wchar_t*)fmt, v); va_end(v); }

#ifdef M_FPTR   // log file pointer  FILE*,  #define  M_FPTR  g_fptr 
	fputws(szt, M_FPTR);
#elif defined(_CONSOLE)   // Win32 Console Application
	wprintf(L"%s\r\n", szt);
#else
	MessageBoxW(0, szt, L"wdbg", 0);
#endif

	return er;
}
































