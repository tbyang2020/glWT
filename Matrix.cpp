

#pragma warning(disable:4996)



#define _MATRIX_CPP_ 

#include "Matrix.h"     // ����  COLUMN_MAJOR

#include <Windows.h> 

#include <math.h>    // floor  

#include "InterfaceMDI/qTrace.h"




#if MATRIX_OPERATIONS|1


// ��ͷ�ļ��ж���  COLUMN_MAJOR, �Լ���Ӧ��Ѱַָ��   //   ͨ���洢������, �� OpenGL ���������� ( column-major ) 






// #define DEBUG_STAGE  1   // ��Ժ��� triz ���õ���״̬ 


// triz, ���λ�����A, ͬʱ��¼���� P. ���� r(A).   D3DXMatrixInverse. ����P �����0, ������ʼ��Ϊ m��m ��λ����

// ��ε��� trizq, �� r ��Ҫ����, ���Ҫ���ú������ƾ�̬���� r 
int& getr(){ static int r; return r; }     void incr(){ getr()++; }    void setr(int v){ getr()=v; }  // triz ֮ǰ��Ҫ setr(0) 

template<class T>  // ע��! ����ǰӦ�� setr(0)  
int triz(T*A, int m, int n, T*P){  if(!A) return 0;  


#if DEBUG_STAGE|0

static int ic; ic++; 

FILE* fptr=fopen("triz.log","a+b");


fprintf(fptr,"\r\n�ݹ�� %d ��\r\nA%d��%d = \r\n", ic, m, n); 

const T* ptr=A;  // a(i,j)=a[p], p= i*n +j,  i=p\n, j=p modn   

for(int i=0;i<m;i++){

for(int j=0;j<n;j++){ fprintf(fptr,"%.1f\t", *LOCK(ptr,m,n,i,j));  }  // ��ʾi��
 
fprintf(fptr,"\r\n");
} // for i

fprintf(fptr,"\r\n\r\n");
fclose(fptr);  
 
#endif  // DEBUG_STAGE   




if(getr() >=m || getr()>=n){ 
#ifdef  DEBUG_STAGE
fptr=fopen("triz.log","a+b");
// ������� r �ﵽ m 
fprintf(fptr,"\r\n���� A%d��%d ���� = %d �Ѵﵽ���ֵ. \r\n", m, n, getr()); 
fclose(fptr); 
ShellExecute(0,"open","triz.log",0,0,SW_SHOW);
#endif  
return getr(); 
} // if(r>=m) return r;   // �ݹ������, ������� r ���Źؼ�����...  




T t, *p, *q, *s;  int i,j,k,u; 

int r=getr(); 

for(j=r; j<n; j++){   // �� j=r �п�ʼ. ��ʼ��Ȼ���ܻ����� (r,r) ֮��ȫ��Ϊ 0 ����.

p=LOCK(A,m,n,r,j);  // A+r*n+j;            // ָ��r��j��Ԫ�� A[r,j] 

for(i=r; i<m; i++){   // �ȱ��� A[r,j] �·�Ԫ��, ��Ҫʱ������, ȷ�� A[r,j]!=0.

if(*p!=0){   // break 

if(i!=r){  
q=LOCK(A,m,n,r,j);  // A+r*n+j; 
s=LOCK(A,m,n,i,j);  // A+i*n+j;         // ���� r��i��.  ���Դӵ� j �п�ʼ.  
for(k=j; k<n; k++) { t=*q; *q=*s; *s=t; INC(q,m,n); INC(s,m,n); } 
if(P) for(k=0; k<m; k++) { t=*LOCK(P,m,m, r,k);  *LOCK(P, m,m, r,k)= *LOCK(P,m,m,i,k);  *LOCK(P,m,m,i,k)=t; }  // ��¼1, ���� r��i�� ... ��������0�п�ʼ,�����Դ�j�п�ʼ, ����ע��P���� m��m  
}  // if(i!=r) 

t=*LOCK(A,m,n,r,j);    // A[r*n+j];  
q=LOCK(A,m,n,r,j);     // A+r*n+j;   // if(t!=1)  ......   
for(k=j; k<n; k++){ *q/=t; INC(q,m,n); }             // r �г��� A[r,j]. �ı���A[r,j]��ֵ.
if(P) for(k=0; k<m; k++) *LOCK(P,m,m,r,k)/=t;      // ��¼2, r �г��� A[r,j] ... ���ﲻ���Դ�j�п�ʼ, ����ע��P���� m��m  

break;  // from for i.
}  // if(*p!=0)

if(*LOCK(A,m,n,r,j)!=0) break;   // from for i. ��������һ��

NEXT_ROW(p, m,n);  // p+=n;
}  // for i


if(*LOCK(A,m,n,r,j)!=0){                         // �� A[r,j]=1 �� j-������Ԫ�ػ� 0, ���м�ȥ  r ��*A[r,j], ������ for j ѭ��.

for(k=0; k<m; k++) { if(k==r) continue;          // r �г���,
t=*LOCK(A,m,n,k,j);                              // A[k*n+j];   // �����м�ȥr�е��ʵ�����. 
for(u=j; u<n;u++)  *LOCK(A,m,n,k,u)-=*LOCK(A,m,n,r,u)*t;        //   A[k*n+u]-=A[r*n+u]*t;   // �� u=j�п�ʼ, ��Ϊr��j��֮ǰΪ0 

if(P) for(u=0; u<m;u++) *LOCK(P,m,m,k,u)-=*LOCK(P,m,m,r,u)*t;   // P[k*m+u]-=P[r*m+u]*t;    // ��¼3 ... �������� u=0 �п�ʼ, ע��P�� m��m ��, ������ A �� [m��n] ��ͬ

}  // for u

r++; incr(); break;  // from for j.  ���� break from for(j).
}  // if(A[r*n+j]!=0)

}  // for j





if(j==n){  // ��� break û�з���.
#if DEBUG_STAGE|0
fptr=fopen("triz.log","a+b");
// ������� r �ﵽ m 
fprintf(fptr,"\r\n���� A%d��%d ���� = %d. ���޸���� 0 ��. \r\n", m, n, getr()); 
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







// Am��s ������ Bs��n �õ� Cm��n ����. ���� C ��ָ��. C ���ڴ�ͨ�� delete[] �ͷ�.  row-major

// ע��, �����Ǿ��������, �ʺܶ�˷��Կ���  matr_x ���. 
//       ������ C ָ��Ϊ A �� B  

template <class T>
T*& matr_x(const T*A,  const T*B, T*& C, int m, int s, int n){
if(A==0L || B==0L || m<=0 || s<=0 || n<=0) return C;

if(C==A || C==B) { MessageBox(0,"������ռ�õ�ַ!","matr_x",0);  return C; }

if(C==0){ C=new T[m*n*sizeof(T)];  memset( C,0,m*n*sizeof(T) );  }

// qTrace("%d��%d",m,n);  show_matr(A,m,s,"A=");

for(int i=0; i<m; i++)
for(int j=0; j<n; j++){

*LOCK(C,m,n,i,j) =0;     // C[i*n+j]=0;   // C[i+j*m]=0; 
for(int k=0; k<s;k++)    // C[i+j*m] += A[i+k*m]*B[k+j*s];
*LOCK(C,m,n,i,j) += *LOCK(A, m,s, i,k) * *LOCK(B, s,n,k,j);    
}  // for ij   

return C;
}



// N-�׾�������. ��������ʽֵ, 0 ��ʾ������.  mat\[in]/[out], ������������, ��������. һ���� row-major, �� OpenGL ʹ�������� ( column-major ) 

// ��������Ƚ��鷳, ����, Ӧ�ó���Ӧ���洢ȫ�ֵ� Camera Matrix, Projection Matrix ���������. 

template < class T >
T matr_inv(T*mat, int N){

if( !mat || N<=0 ) return 0;

T *p=mat;

T*q=new T[N*N];   memset(q,0,N*N*sizeof(T));   // ��������, ��ʼ��Ϊ��λ��, ������Ϊ A ������ 
for(int i=0; i<N; i++)  q[i*N+i]=1;  // q[i+i*N]=1;           // �洢��ʽ��Ӱ�챾�д���  


#if OP_DIAGON|1

int k=0;  T t=0;  // �Խǻ�

for(int i=0; i<N; i++){     // �� i �� 
 

// ȷ�� (i,i)!=0  
k=i;  
while( k<N ) // {  if(p[k*N+i]!=0)break; k++; }     // { if(p[k+i*N]!=0)break; k++; } 
{ if(*LOCK(p,N,N,k,i) !=0 ) break;  k++; }


if(k==N){ qTrace("i=%d, No Solution!",i); break; }   // �����˻�, �˳�ѭ��


if(k>i){  // qTrace("k=%d, i=%d",k,i);  // ���� i��k�� 
for(int j=0; j<N; j++){
// t=p[k+j*N]; p[k+j*N]=p[i+j*N]; p[i+j*N]=t;     t=q[k+j*N]; q[k+j*N]=q[i+j*N]; q[i+j*N]=t; 
// t=p[k*N+j]; p[k*N+j]=p[i*N+j]; p[i*N+j]=t;     t=q[k*N+j]; q[k*N+j]=q[i*N+j]; q[i*N+j]=t; 
t=*LOCK(p,N,N,k,j);  *LOCK(p,N,N,k,j)=*LOCK(p,N,N,i,j); *LOCK(p,N,N,i,j)=t; 
t=*LOCK(q,N,N,k,j);  *LOCK(q,N,N,k,j)=*LOCK(q,N,N,i,j); *LOCK(q,N,N,i,j)=t; 
}  // for j
}  // if k>i  



// ���� i ��, �� i �л� 0, �� (i,i) ����  // t=1/p[i+i*N];  
for(int r=0; r<N; r++){  
if(r==i) continue; 

if(*LOCK(p,N,N,r,i)!=0){  // if(p[r*N+i]!=0){   // if(p[r+i*N]!=0){

t=*LOCK(p,N,N,r,i) / *LOCK(p,N,N,i,i);    // t=p[r*N+i]/p[i*N+i];  // t=p[r+i*N]/p[i+i*N];  // ע�� p[r+i*N] �ᱻ����, Ϊ 0 ! 

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


t=1;  // �Խ��ͻ���λ��
for (int i = 0; i < N; i++) {

t*= *LOCK(p,N,N,i,i);  // t*=p[i*N+i];  // t*=p[i+i*N];  // �ǵ�

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



show_matr(A,m,n, "������� A "); 



float*B=  new float[m*n];  //  (double*)malloc(m*n*sizeof(double));   //new ������? -- []������д�� ()  

memmove(B,A, sizeof(float)*m*n);   // for(int i=0; i<m*n; i++) B[i]=A[i];  


setr(0);
float* P = new float[m*m];  memset(P,0, m*m*sizeof(float));  for(int i=0; i<m; i++) P[i*m+i]=1; 

triz(A, m, n, P);  

show_matr(A,m,n, "���� A �ı�׼������"); 
show_matr(P,m,m, "���� A �Ľ��ݻ�����(�����) P"); 

float* C=0L; 
matr_x(P,B,C, m,m,n); 

show_matr(C,m,n, "��֤ PA ="); 


matr_inv(B,m); 
show_matr(B,m,n, "ͨ�� matr_inv ����������  ="); 


show_matr((const float*)0);

delete[] P; 
delete[] C;
delete[] B;  // free(B); 
}




void t_triz() {
 

#ifndef COLUMN_MAJOR 

// ��8����ҵ 2020/5/1  

float A[]={                \
1,  -1,   5,  -1,  1,  -3,     \
1,   1,  -2,   3,  4,   5,     \
3,  -1,   8,   1,  0,   0,     \
1,   1,  -9,   7,  0,   0,     \
0,   0,   0,   0,  0,   0,     \
};           int m=5, n=6;


#else 


// ��8����ҵ 2020/5/1    clolumn-major 

float  A[]={                  \
 1,     1,   3,   1,  0,      \
-1,     1,  -1,   1,  0,      \
 5,    -2,   8,  -9,  0,      \
-1,     3,   1,   7,  0,      \
 1,     4,   0,   0,  0,      \
-3,     5,   0,   0,  0,      \
};              int m=5, n=6;

#endif   



show_matr(A,m,n, "������� A "); 



float*B=  new float[m*n];  //  (double*)malloc(m*n*sizeof(double));   //new ������? -- []������д�� ()  

memmove(B,A, sizeof(float)*m*n);   // for(int i=0; i<m*n; i++) B[i]=A[i];  


setr(0);
float* P = new float[m*m];  memset(P,0, m*m*sizeof(float));  for(int i=0; i<m; i++) P[i*m+i]=1; 

triz(A, m, n, P);  


show_matr(A,m,n, "���λ� A="); 

show_matr(P,m,m, "�����б任 P="); 



float*C=0L; matr_x(P,B,C, m,m,n); 

show_matr(C,m,n, "���� PA="); 

show_matr((const float*)0);

delete[] P; 
delete[] C;
delete[] B;  // free(B); 
}




#if SHOW_MATRICES|1




// �������������. nΪ���������ĸ���, 1-based  
// template <class T>
double cofr(int*a, int n, int* po=0, int* qo=0){
if(n<1) return 0;  // 0-based index 

// ������������ֵ.    ע�� n-- 
// s0=a(n),  sk = a(n-k)+1/s(k-1)   0<k<=n 
// p0=a(n), q(0)=1.    pk = a(n-k)p(k-1)+q(k-1), qk =p(k-1).  0<k<=n 

int p=a[n-1], q=1, t;      // �ǳ��ջ����, �������ļ��������������!  


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





// ��������, �������� a/b   �ƽ� double r, ֱ������㹻С.  ��������������. 
 
// Ҳ�������Ա���ƥ��--ȡǰ���������, �����С�� -- �����ĸ��[1,10000] �ķ���. 

// n-�ױƽ������, ���ع���Ϊ 1/2^(n/2).  10^-12 �� 2^(-n). n �� 12/0.3010 �� 40, ���� n��������ֵΪ 80.    

// float ��ָ��λ��8λ, �� double ��ָ��λ�� 11 λ
// float �ķ�ΧΪ -2^128 ~ +2^128, Ҳ�� -3.40E+38 ~ +3.40E+38
// double�ķ�ΧΪ -2^1024 ~ +2^1024, Ҳ�� -1.79E+308 ~ +1.79E+308 

// �ú����ڲ�ʹ�� float �������׳���, ԭ�����!  ���� float, ���� er=1E-6,  �ƺ����Խ������.

int frac(double v, int* a, int *b, double er=1E-6 ){   // ���� double v, ��������  er=1E-12


// ������ 
// r0=r 
// ak=[r(k-1)],  rk=1/(r(k-1)-ak) 


#if CUT_TAIL|0

double u=v;  
const int d=4;   // 12, 24, 38
u*=pow(10,d);  u=floor(u);  u/=pow(10,d);  
v=u; 
 
#endif  // CUT_TAIL  


const int BLOCK= 128;   // �ڴ�����  
int iBlock = BLOCK;
int* as=(int*) malloc(sizeof(int)*iBlock);  


double r=v, s, t;              

int p,q;           // s=p/q  

int n=0;           // �������е�ʵ�ʳ���  

do{
as[n]=r;           // �ض�ȡ��   n=0 ʱΪ 1-�� 
r -= as[n];  

if(r==0){ n++; break; }   // �ƽ���� 
r=1/r; 
 
n++;     if(n>iBlock){ iBlock+=BLOCK;  as=(int*) realloc(as,sizeof(int)*iBlock); }  // if 

t=cofr(as, n);  
t-=v;
if(t<0) t=-t; 


if(n>80) { qTrace("v=%.12f\r\nΪ�˴ﵽ�ƽ����� 10^(-12), ��������Ҫ n=%d > 80 �����ϵ�������!", v, n); break; }  // ����, ���������� 80. ���ھ��� 10^(-12), n ��������ֵΪ 80.   


}while(t>er);



cofr(as,n,&p,&q); 

if(a) *a=p; 
if(b) *b=q;  

return n;
}  // frac  



// ���� frac, ��С��λ����.  

void t_frac(){
float v=0.202380985022;  // 2.718281828459;   // 1/3.;  // 3.14159265358979323846;    // 3.1415926 5358979 323 846    // 2.71828 1828459 04523536 028747 13527 
int a, b;
int n=frac(v,&a,&b);

char szt[128];
sprintf(szt, "%.12f �� %d/%d = %.12f\r\n���������� %d\r\n", v, a, b, (double)a/b, n);
MessageBox(0, szt,"t_frac",0); 
}







#endif  // SHOW_MATRICES




#endif  // MATRIX_OPERATIONS





#if MORE_OPERS    // see Colobot/math/matrix.h��geometry.h 



#endif  //  MORE_OPERS