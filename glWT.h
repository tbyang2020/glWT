

// GLWT.h
#ifndef _GLWT_H_
#define _GLWT_H_      // #endif  // _GLWT_H_ //

// 结构仅在 GLWT.CPP 中实现
#ifdef  _GLWT_CPP_    // #define _GLWT_CPP_ //
#define GLWT_EXTERN 
#else 
#define GLWT_EXTERN extern 
#endif  // _GLWT_CPP_ //


#include <Windows.h>


#define   PI   3.14159265358979   // 3.1415926  5358979  323846  




float* ToWorld(const float*xs, float*xo);  
float* ToScreen(const float*xo, float*xs); 

void t_glWT();

void t_unProject(); 


#endif  // _GLWT_H_ //


