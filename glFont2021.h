


// GLFONT2021.h
#ifndef _GLFONT2021_H_
#define _GLFONT2021_H_    // #endif  // _GLFONT2021_H_ //

#include <Windows.h>

// 结构仅在GLFONT2021.CPP中实现
#ifdef  _GLFONT2021_CPP_    // #define  _GLFONT2021_CPP_   
#define GLFONT2021_EXTERN 
#else 
#define GLFONT2021_EXTERN extern 
#endif  // _GLFONT2021_CPP_ //



int ToWorld(const double x, const double y, const double z, double* xw, double* yw, double* zw);

 int TextOutBm(float x, float y, LPCTSTR fmt , ...); 
 


#endif  // _GLFONT2021_H_ //