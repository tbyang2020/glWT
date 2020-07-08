

#pragma warning(disable:4996)



#define _MATRIX_CPP_ 

#include "Matrix.h"     // 定义  COLUMN_MAJOR

#include <Windows.h> 

#include <math.h>    // floor  

#include "InterfaceMDI/qTrace.h"




#if MATRIX_OPERATIONS|1


// 在头文件中定义  COLUMN_MAJOR, 以及相应的寻址指针   //   通常存储行向量, 但 OpenGL 储行列向量 ( column-major ) 






// #define DEBUG_STAGE  1   // 针对函数 triz 设置调试状态 


// triz, 梯形化矩阵A, 同时记录过程 P. 返回 r(A).   D3DXMatrixInverse. 参数P 如果非0, 则必须初始化为 m×m 单位矩阵

// 多次调用 trizq, 秩 r 需要清零, 因此要利用函数控制静态变量 r 
int& getr(){ static int r; return r; }     void incr(){ getr()++; }    void setr(int v){ getr()=v; }  // triz 之前先要 setr(0) 

template<class T>  // 注意! 调用前应当 setr(0)  
int triz(T*A, int m, int n, T*P){  if(!A) return 0;  


#if DEBUG_STAGE|0

static int ic; ic++; 

FILE* fptr=fopen("triz.log","a+b");


fprintf(fptr,"\r\n递归第 %d 层\r\nA%d×%d = \r\n", ic, m, n); 

const T* ptr=A;  // a(i,j)=a[p], p= i*n +j,  i=p\n, j=p modn   

for(int i=0;i<m;i++){

for(int j=0;j<n;j++){ fprintf(fptr,"%.1f\t", *LOCK(ptr,m,n,i,j));  }  // 显示i行
 
fprintf(fptr,"\r\n");
} // for i

fprintf(fptr,"\r\n\r\n");
fclose(fptr);  
 
#endif  // DEBUG_STAGE   




if(getr() >=m || getr()>=n){ 
#ifdef  DEBUG_STAGE
fptr=fopen("triz.log","a+b");
// 矩阵的秩 r 达到 m 
fprintf(fptr,"\r\n矩阵 A%d×%d 的秩 = %d 已达到最大值. \r\n", m, n, getr()); 
fclose(fptr); 
ShellExecute(0,"open","triz.log",0,0,SW_SHOW);
#endif  
return getr(); 
} // if(r>=m) return r;   // 递归过程中, 矩阵的秩 r 起着关键作用...  




T t, *p, *q, *s;  int i,j,k,u; 

int r=getr(); 

for(j=r; j<n; j++){   // 从 j=r 列开始. 开始仍然可能会遇到 (r,r) 之下全部为 0 的列.

p=LOCK(A,m,n,r,j);  // A+r*n+j;            // 指向r行j列元素 A[r,j] 

for(i=r; i<m; i++){   // 先遍历 A[r,j] 下方元素, 必要时交换行, 确保 A[r,j]!=0.

if(*p!=0){   // break 

if(i!=r){  
q=LOCK(A,m,n,r,j);  // A+r*n+j; 
s=LOCK(A,m,n,i,j);  // A+i*n+j;         // 交换 r、i行.  可以从第 j 列开始.  
for(k=j; k<n; k++) { t=*q; *q=*s; *s=t; INC(q,m,n); INC(s,m,n); } 
if(P) for(k=0; k<m; k++) { t=*LOCK(P,m,m, r,k);  *LOCK(P, m,m, r,k)= *LOCK(P,m,m,i,k);  *LOCK(P,m,m,i,k)=t; }  // 记录1, 交换 r、i行 ... 这里必须从0列开始,不可以从j列开始, 并且注意P阶是 m×m  
}  // if(i!=r) 

t=*LOCK(A,m,n,r,j);    // A[r*n+j];  
q=LOCK(A,m,n,r,j);     // A+r*n+j;   // if(t!=1)  ......   
for(k=j; k<n; k++){ *q/=t; INC(q,m,n); }             // r 行除以 A[r,j]. 改变了A[r,j]的值.
if(P) for(k=0; k<m; k++) *LOCK(P,m,m,r,k)/=t;      // 记录2, r 行除以 A[r,j] ... 这里不可以从j列开始, 并且注意P阶是 m×m  

break;  // from for i.
}  // if(*p!=0)

if(*LOCK(A,m,n,r,j)!=0) break;   // from for i. 无需检查下一列

NEXT_ROW(p, m,n);  // p+=n;
}  // for i


if(*LOCK(A,m,n,r,j)!=0){                         // 用 A[r,j]=1 将 j-列其他元素化 0, 各行减去  r 行*A[r,j], 且跳出 for j 循环.

for(k=0; k<m; k++) { if(k==r) continue;          // r 行除外,
t=*LOCK(A,m,n,k,j);                              // A[k*n+j];   // 其他行减去r行的适当倍数. 
for(u=j; u<n;u++)  *LOCK(A,m,n,k,u)-=*LOCK(A,m,n,r,u)*t;        //   A[k*n+u]-=A[r*n+u]*t;   // 从 u=j列开始, 因为r行j列之前为0 

if(P) for(u=0; u<m;u++) *LOCK(P,m,m,k,u)-=*LOCK(P,m,m,r,u)*t;   // P[k*m+u]-=P[r*m+u]*t;    // 记录3 ... 这里必须从 u=0 列开始, 注意P是 m×m 阶, 可能与 A 的 [m×n] 不同

}  // for u

r++; incr(); break;  // from for j.  必须 break from for(j).
}  // if(A[r*n+j]!=0)

}  // for j





if(j==n){  // 情况 break 没有发生.
#if DEBUG_STAGE|0
fptr=fopen("triz.log","a+b");
// 矩阵的秩 r 达到 m 
fprintf(fptr,"\r\n矩阵 A%d×%d 的秩 = %d. 已无更多非 0 行. \r\n", m, n, getr()); 
fclose(fptr); 
ShellExecute(0,"open","triz.log",0,0,SW_SHOW);
#endif  
return r; 
}






//static int ic=0; ic++;   \
printf("A%d=(Rank=%d)\n", ic,r);  show_matrix(A,m, n,3);  printf("\n");  \
//printf("P%d=\n", ic);  show_matrix(P,m, m);  printf("\n");   

return triz(A,m,n,P);
}







// Am×s 矩阵与 Bs×n 得到 Cm×n 矩阵. 返回 C 的指针. C 的内存通过 delete[] 释放.  row-major

// 注意, 向量是矩阵的特例, 故很多乘法皆可由  matr_x 完成. 
//       不允许 C 指定为 A 或 B  

template <class T>
T*& matr_x(const T*A,  const T*B, T*& C, int m, int s, int n){
if(A==0L || B==0L || m<=0 || s<=0 || n<=0) return C;

if(C==A || C==B) { MessageBox(0,"不允许占用地址!","matr_x",0);  return C; }

if(C==0){ C=new T[m*n*sizeof(T)];  memset( C,0,m*n*sizeof(T) );  }

// qTrace("%d×%d",m,n);  show_matr(A,m,s,"A=");

for(int i=0; i<m; i++)
for(int j=0; j<n; j++){

*LOCK(C,m,n,i,j) =0;     // C[i*n+j]=0;   // C[i+j*m]=0; 
for(int k=0; k<s;k++)    // C[i+j*m] += A[i+k*m]*B[k+j*s];
*LOCK(C,m,n,i,j) += *LOCK(A, m,s, i,k) * *LOCK(B, s,n,k,j);    
}  // for ij   

return C;
}



// N-阶矩阵求逆. 返回行列式值, 0 表示不可逆.  mat\[in]/[out], 输入待求逆矩阵, 输出逆矩阵. 一般是 row-major, 但 OpenGL 使用列向量 ( column-major ) 

// 逆矩阵计算比较麻烦, 所以, 应用程序应当存储全局的 Camera Matrix, Projection Matrix 及其逆矩阵. 

template < class T >
T matr_inv(T*mat, int N){

if( !mat || N<=0 ) return 0;

T *p=mat;

T*q=new T[N*N];   memset(q,0,N*N*sizeof(T));   // 辅助矩阵, 初始化为单位阵, 将被化为 A 的逆阵 
for(int i=0; i<N; i++)  q[i*N+i]=1;  // q[i+i*N]=1;           // 存储方式不影响本行代码  


#if OP_DIAGON|1

int k=0;  T t=0;  // 对角化

for(int i=0; i<N; i++){     // 第 i 行 
 

// 确保 (i,i)!=0  
k=i;  
while( k<N ) // {  if(p[k*N+i]!=0)break; k++; }     // { if(p[k+i*N]!=0)break; k++; } 
{ if(*LOCK(p,N,N,k,i) !=0 ) break;  k++; }


if(k==N){ qTrace("i=%d, No Solution!",i); break; }   // 矩阵退化, 退出循环


if(k>i){  // qTrace("k=%d, i=%d",k,i);  // 交换 i、k行 
for(int j=0; j<N; j++){
// t=p[k+j*N]; p[k+j*N]=p[i+j*N]; p[i+j*N]=t;     t=q[k+j*N]; q[k+j*N]=q[i+j*N]; q[i+j*N]=t; 
// t=p[k*N+j]; p[k*N+j]=p[i*N+j]; p[i*N+j]=t;     t=q[k*N+j]; q[k*N+j]=q[i*N+j]; q[i*N+j]=t; 
t=*LOCK(p,N,N,k,j);  *LOCK(p,N,N,k,j)=*LOCK(p,N,N,i,j); *LOCK(p,N,N,i,j)=t; 
t=*LOCK(q,N,N,k,j);  *LOCK(q,N,N,k,j)=*LOCK(q,N,N,i,j); *LOCK(q,N,N,i,j)=t; 
}  // for j
}  // if k>i  



// 利用 i 行, 把 i 列化 0, 但 (i,i) 不变  // t=1/p[i+i*N];  
for(int r=0; r<N; r++){  
if(r==i) continue; 

if(*LOCK(p,N,N,r,i)!=0){  // if(p[r*N+i]!=0){   // if(p[r+i*N]!=0){

t=*LOCK(p,N,N,r,i) / *LOCK(p,N,N,i,i);    // t=p[r*N+i]/p[i*N+i];  // t=p[r+i*N]/p[i+i*N];  // 注意 p[r+i*N] 会被更改, 为 0 ! 

for(int c=0; c<N; c++){

// p[r+c*N] -= p[i+c*N]*t;    q[r+c*N] -= q[i+c*N]*t;  
// p[r*N+c] -= p[i*N+c]*t;    q[r*N+c] -= q[i*N+c]*t;  

*LOCK(p,N,N,r,c) -= *LOCK(p,N,N,i,c)*t; 
*LOCK(q,N,N,r,c) -= *LOCK(q,N,N,i,c)*t;  

}  // for c

}  // if !=0 

}  // for r

}  // for i 
 

#endif  // OP_DIAGON 


t=1;  // 对角型化单位阵
for (int i = 0; i < N; i++) {

t*= *LOCK(p,N,N,i,i);  // t*=p[i*N+i];  // t*=p[i+i*N];  // 角点

for(int j=0; j<N; j++)   *LOCK(q,N,N,i,j) /= *LOCK(p,N,N,i,i);  // q[i*N+j] /= p[i*N+i];    // q[i+j*N] /= p[i+i*N];  

*LOCK(p,N,N,i,i)=1;   // p[i*N+i]=1;   // p[i+i*N]=1;   
}  // for i 


memcpy(mat,q, N*N*sizeof(T));
delete[] q; 

return t; 
}





// #define float double

void t_inv() {
 

#ifndef COLUMN_MAJOR 

 
//float A[]={     \
1,  -1,   0,     \
1,   2,   0,     \
0,   0,   1,     \
};   int m=3, n=3;

 


float A[]={     \
1,   11,   -21,   31,   \
2,   12,    22,  -32,   \
3,   13,   -23,   33,   \
4,   14,    24,   34,   \
};   int m=4, n=4;

 


#else 



// float A[]={     \
 1,   1,   0,     \
-1,   2,   0,     \
 0,   0,   1,     \
};   int m=3, n=3;


float A[]={     \
  1,     2,     3,    4,   \
 11,    12,    13,   14,   \
-21,    22,   -23,   24,   \
 31,   -32,    33,   34,   \
};   int m=4, n=4;

 

#endif   



show_matr(A,m,n, "输入矩阵 A "); 



float*B=  new float[m*n];  //  (double*)malloc(m*n*sizeof(double));   //new 有问题? -- []不可以写作 ()  

memmove(B,A, sizeof(float)*m*n);   // for(int i=0; i<m*n; i++) B[i]=A[i];  


setr(0);
float* P = new float[m*m];  memset(P,0, m*m*sizeof(float));  for(int i=0; i<m; i++) P[i*m+i]=1; 

triz(A, m, n, P);  

show_matr(A,m,n, "矩阵 A 的标准阶梯型"); 
show_matr(P,m,m, "矩阵 A 的阶梯化矩阵(逆矩阵) P"); 

float* C=0L; 
matr_x(P,B,C, m,m,n); 

show_matr(C,m,n, "验证 PA ="); 


matr_inv(B,m); 
show_matr(B,m,n, "通过 matr_inv 计算的逆矩阵  ="); 


show_matr((const float*)0);

delete[] P; 
delete[] C;
delete[] B;  // free(B); 
}




void t_triz() {
 

#ifndef COLUMN_MAJOR 

// 第8周作业 2020/5/1  

float A[]={                \
1,  -1,   5,  -1,  1,  -3,     \
1,   1,  -2,   3,  4,   5,     \
3,  -1,   8,   1,  0,   0,     \
1,   1,  -9,   7,  0,   0,     \
0,   0,   0,   0,  0,   0,     \
};           int m=5, n=6;


#else 


// 第8周作业 2020/5/1    clolumn-major 

float  A[]={                  \
 1,     1,   3,   1,  0,      \
-1,     1,  -1,   1,  0,      \
 5,    -2,   8,  -9,  0,      \
-1,     3,   1,   7,  0,      \
 1,     4,   0,   0,  0,      \
-3,     5,   0,   0,  0,      \
};              int m=5, n=6;

#endif   



show_matr(A,m,n, "输入矩阵 A "); 



float*B=  new float[m*n];  //  (double*)malloc(m*n*sizeof(double));   //new 有问题? -- []不可以写作 ()  

memmove(B,A, sizeof(float)*m*n);   // for(int i=0; i<m*n; i++) B[i]=A[i];  


setr(0);
float* P = new float[m*m];  memset(P,0, m*m*sizeof(float));  for(int i=0; i<m; i++) P[i*m+i]=1; 

triz(A, m, n, P);  


show_matr(A,m,n, "梯形化 A="); 

show_matr(P,m,m, "初等行变换 P="); 



float*C=0L; matr_x(P,B,C, m,m,n); 

show_matr(C,m,n, "检验 PA="); 

show_matr((const float*)0);

delete[] P; 
delete[] C;
delete[] B;  // free(B); 
}




#if SHOW_MATRICES|1




// 反向计算连分数. n为连分整数的个数, 1-based  
// template <class T>
double cofr(int*a, int n, int* po=0, int* qo=0){
if(n<1) return 0;  // 0-based index 

// 计算连分数的值.    注意 n-- 
// s0=a(n),  sk = a(n-k)+1/s(k-1)   0<k<=n 
// p0=a(n), q(0)=1.    pk = a(n-k)p(k-1)+q(k-1), qk =p(k-1).  0<k<=n 

int p=a[n-1], q=1, t;      // 非常恼火的是, 连分数的计算必须重新来过!  


for(int i=1; i<n; i++){
t=p;
p=a[n-i-1]*p+q;  q=t;
}
 
if(po) *po=p;
if(qo) *qo=q; 
double r = 1.*p/q;  


#if DEBUG_STAGE|0

char szt[256];
int ip=sprintf(szt,"[");
for(int i=0; i<n; i++){ 
ip+=sprintf(szt+ip,"%d  ", a[i]); 
}  // for i 
ip+=sprintf(szt+ip,"]\r\n %d/%d=%f", p,q, r); 
MessageBox(0,szt,"cofr",0);

#endif  // DEBUG_STAGE




return r;
}

void t_cofr(){
int a[3]={1,2,3, };
int p, q; 
float v=cofr(a,3,&p,&q);

qTrace("%f=%d/%d", v,p,q);
}





// 辅助函数, 用连分数 a/b   逼近 double r, 直到误差足够小.  返回连分数长度. 
 
// 也可以试试暴力匹配--取前面的有理数, 差别最小的 -- 例如分母∈[1,10000] 的分数. 

// n-阶逼近的误差, 保守估计为 1/2^(n/2).  10^-12 ≈ 2^(-n). n ≈ 12/0.3010 ≈ 40, 所以 n的最大估计值为 80.    

// float 的指数位有8位, 而 double 的指数位有 11 位
// float 的范围为 -2^128 ~ +2^128, 也即 -3.40E+38 ~ +3.40E+38
// double的范围为 -2^1024 ~ +2^1024, 也即 -1.79E+308 ~ +1.79E+308 

// 该函数内部使用 float 方程容易出错, 原因不清楚!  对于 float, 设置 er=1E-6,  似乎可以解决问题.

int frac(double v, int* a, int *b, double er=1E-6 ){   // 对于 double v, 可以设置  er=1E-12


// 连分数 
// r0=r 
// ak=[r(k-1)],  rk=1/(r(k-1)-ak) 


#if CUT_TAIL|0

double u=v;  
const int d=4;   // 12, 24, 38
u*=pow(10,d);  u=floor(u);  u/=pow(10,d);  
v=u; 
 
#endif  // CUT_TAIL  


const int BLOCK= 128;   // 内存粒度  
int iBlock = BLOCK;
int* as=(int*) malloc(sizeof(int)*iBlock);  


double r=v, s, t;              

int p,q;           // s=p/q  

int n=0;           // 连整数列的实际长度  

do{
as[n]=r;           // 截断取整   n=0 时为 1-阶 
r -= as[n];  

if(r==0){ n++; break; }   // 逼近完成 
r=1/r; 
 
n++;     if(n>iBlock){ iBlock+=BLOCK;  as=(int*) realloc(as,sizeof(int)*iBlock); }  // if 

t=cofr(as, n);  
t-=v;
if(t<0) t=-t; 


if(n>80) { qTrace("v=%.12f\r\n为了达到逼近精度 10^(-12), 不可能需要 n=%d > 80 阶以上的连分数!", v, n); break; }  // 测试, 限制最大阶数 80. 对于精度 10^(-12), n 的最大估计值为 80.   


}while(t>er);



cofr(as,n,&p,&q); 

if(a) *a=p; 
if(b) *b=q;  

return n;
}  // frac  



// 测试 frac, 化小数位分数.  

void t_frac(){
float v=0.202380985022;  // 2.718281828459;   // 1/3.;  // 3.14159265358979323846;    // 3.1415926 5358979 323 846    // 2.71828 1828459 04523536 028747 13527 
int a, b;
int n=frac(v,&a,&b);

char szt[128];
sprintf(szt, "%.12f ≈ %d/%d = %.12f\r\n连分数长度 %d\r\n", v, a, b, (double)a/b, n);
MessageBox(0, szt,"t_frac",0); 
}







#endif  // SHOW_MATRICES




#endif  // MATRIX_OPERATIONS





#if MORE_OPERS    // see Colobot/math/matrix.h、geometry.h 



#endif  //  MORE_OPERS