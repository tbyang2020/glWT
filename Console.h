

// CONSOLE.h
#ifndef _CONSOLE_H_
#define _CONSOLE_H_    //#endif  // _CONSOLE_H_ //

// 结构仅在 CONSOLE.CPP 中实现
#ifdef  _CONSOLE_CPP_    //#endif  // _CONSOLE_CPP_ //
#define CONSOLE_EXTERN 
#else 
#define CONSOLE_EXTERN extern 
#endif  // _CONSOLE_CPP_ //



 
 int CreateConsole(); 




#endif  // _CONSOLE_H_ //


