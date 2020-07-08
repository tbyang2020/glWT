

// MATRIX.h   参见 Colobot/math/matrix.h、geometry.h、vector.h 

#ifndef _MATRIX_H_
#define _MATRIX_H_      // #endif  // _MATRIX_H_ //

// 结构仅在 MATRIX.CPP 中实现
#ifdef  _MATRIX_CPP_    // #define _MATRIX_CPP_ //
#define MATRIX_EXTERN 
#else 
#define MATRIX_EXTERN extern 
#endif  // _MATRIX_CPP_ //


#include <Windows.h>
#include <math.h>
#include <assert.h>

#include "InterfaceMDI/qTrace.h"




#define  COLUMN_MAJOR   //   通常存储行向量, 但 OpenGL 储行列向量 ( column-major ) 

#ifdef  COLUMN_MAJOR

#define  ROW_PTR(A, m, n, i)     ((A)+(i))              // 指向 Am×n 的 i行   
#define  COL_PTR(A, m, n, j)     ((A)+(j)*(m))          // 指向 Am×n 的 j列  

#define  LOCK(A, m, n, i ,j)     ((A)+(i)+(j)*(m))      // 指向 Am×n 的 i行j列元素 A[i,j] 

#define  NEXT_ROW(ptr, m,n)        (ptr)++                // 下一行 
#define  NEXT_COL(ptr, m,n)        (ptr)+=(m)             // 下一列. n 为列数. 
#define  INC(ptr, m,n)             (ptr)+=(m)             // 同行的下一列元素. n 为列数.  与 NEXT_COL 相同                   


#else    //  ROW_MAJOR

#define  ROW_PTR(A, m, n, i)     ((A)+(i)*(n))         // 指向 Am×n 的 i行   
#define  COL_PTR(A, m, n, j)     ((A)+(j))             // 指向 Am×n 的 j列  

#define  LOCK(A, m, n, i ,j)     ((A)+(i)*(n)+(j))     // 指向 Am×n 的 i行j列元素 A[i,j] 

#define  NEXT_ROW(ptr, m,n)        (ptr)+=(n)             // 下一行 
#define  NEXT_COL(ptr, m,n)        (ptr)++                // 下一列. n 为列数. 
#define  INC(ptr, m,n)             (ptr)++                // 同行的下一列元素. n 为列数.  与 NEXT_COL 相同              



#endif   //  COLUMN_MAJOR






#define  ERR  1E-6   // TOLERANCE, 精度设置  10^-6,     

inline 
bool EQU(float a, float b, float tolerance = ERR) { return fabs(a - b) < tolerance; }  //! Compares \a a and \a b within \a tolerance

inline 
bool IS_ZERO(float a, float tolerance = ERR) { return EQU(a, 0.0f, tolerance); }  //! Compares \a a to zero within \a tolerance





// 矩阵、向量封装  

#if VECTOR_MATRIX|1





// \struct Matrix math/matrix.h   * \brief 4x4 matrix
// The internal representation is a 16-value table in column-major order, thus:

// \verbatim  逐字描述为
// m[0 ] m[4 ] m[8 ] m[12]
// m[1 ] m[5 ] m[9 ] m[13]
// m[2 ] m[6 ] m[10] m[14]
// m[3 ] m[7 ] m[11] m[15]
// \endverbatim

// This representation is native to OpenGL; DirectX requires transposing the matrix.
 
//  The order of multiplication of matrix and vector is also OpenGL-native (see the function MatrixVectorMultiply).

// All methods are made inline to maximize optimization. --- 关键字 inline 在类内部没有必要

// Unit tests for the structure and related functions are in module: math/test/matrix_test.cpp.  --- 直接在这里测试 
 
typedef class Matrix{   // Matrix values in column-major order
private:
float* a = 0L;  int m = 4, n = 4;
	
public:
Matrix(float*_a=0, int m=0, int n=0) {   //! Creates the indentity matrix
	m = n = 4;
	a = (float*)malloc( m*n * sizeof(float) );  
	memset(a, 0, m*n * sizeof(float));
	for (int i = 0; i < m; i++) a[i + i*m] = 1;   // 按 column-major 定位, 不影响 
}  

~Matrix() {  free(a);  }

	
//! Creates the matrix from 1D array \a m matrix values in column-major order 
explicit Matrix(const float(&_m)[16]) { for (int i = 0; i < m*n; ++i) a[i] = _m[i];	}

	
//! Creates the matrix from 2D array. The array's first index is row, second is column.  \param m array with values
explicit Matrix(const float(&_m)[4][4])	{
for (int c = 0; c < 4; ++c)
for (int r = 0; r < 4; ++r)  // a[r+c*4] = _m[r][c];  
*LOCK(a, m, n, r, c) = _m[r][c]; 
}



//! Sets value in given row and col
void Set(int row, int col, float value) // { a[(col - 1) * 4 + (row - 1)] = value; }
{    *LOCK(a, m, n, row-1, col-1) = value; }
	
//! Returns the value in given row and col
float Get(int row, int col) // { return a[(col - 1) * 4 + (row - 1)]; }
{ return   *LOCK(a, m, n, row - 1, col - 1); }



//! Loads the zero matrix	
void LoadZero() {  for (int i = 0; i < 16; ++i) a[i] = 0.0f; }

//! Loads the identity matrix
void LoadIdentity() {  LoadZero();  for(int i=0; i<m; i++) a[i+i*m]=1; }



operator float* () { return (float*)this; }  // cast operator 
operator const float* () { return (const float*)this; }  // cast to const float* 




#if TOBE_CONTINUED



	//! Transposes the matrix
	inline void Transpose()
	{
		/* (2,1) <-> (1,2) */ Swap(m[1], m[4]);
		/* (3,1) <-> (1,3) */ Swap(m[2], m[8]);
		/* (4,1) <-> (1,4) */ Swap(m[3], m[12]);
		/* (3,2) <-> (2,3) */ Swap(m[6], m[9]);
		/* (4,2) <-> (2,4) */ Swap(m[7], m[13]);
		/* (4,3) <-> (3,4) */ Swap(m[11], m[14]);
	}

	//! Calculates the determinant of the matrix
	/** \returns the determinant */
	inline float Det() const
	{
		float result = 0.0f;
		for (int i = 0; i < 4; ++i)
		{
			result += m[i] * Cofactor(i, 0);
		}
		return result;
	}

	//! Calculates the cofactor of the matrix
	/**
	 * \param r  row (0 to 3)
	 * \param c  column (0 to 3)
	 * \returns  the cofactor
	 */
	inline float Cofactor(int r, int c) const
	{
		assert(r >= 0 && r <= 3);
		assert(c >= 0 && c <= 3);

		float result = 0.0f;

		/* That looks horrible, I know. But it's fast :) */

		switch (4 * r + c)
		{
			// r=0, c=0
			/* 05 09 13
			   06 10 14
			   07 11 15 */
		case 0:
			result = +m[5] * (m[10] * m[15] - m[14] * m[11])
				- m[9] * (m[6] * m[15] - m[14] * m[7])
				+ m[13] * (m[6] * m[11] - m[10] * m[7]);
			break;

			// r=0, c=1
			/* 01 09 13
			   02 10 14
			   03 11 15 */
		case 1:
			result = -m[1] * (m[10] * m[15] - m[14] * m[11])
				+ m[9] * (m[2] * m[15] - m[14] * m[3])
				- m[13] * (m[2] * m[11] - m[10] * m[3]);
			break;

			// r=0, c=2
			/* 01 05 13
			  02 06 14
			  03 07 15 */
		case 2:
			result = +m[1] * (m[6] * m[15] - m[14] * m[7])
				- m[5] * (m[2] * m[15] - m[14] * m[3])
				+ m[13] * (m[2] * m[7] - m[6] * m[3]);
			break;

			// r=0, c=3
			/* 01 05 09
			  02 06 10
			  03 07 11 */
		case 3:
			result = -m[1] * (m[6] * m[11] - m[10] * m[7])
				+ m[5] * (m[2] * m[11] - m[10] * m[3])
				- m[9] * (m[2] * m[7] - m[6] * m[3]);
			break;

			// r=1, c=0
			/* 04 08 12
			  06 10 14
			  07 11 15 */
		case 4:
			result = -m[4] * (m[10] * m[15] - m[14] * m[11])
				+ m[8] * (m[6] * m[15] - m[14] * m[7])
				- m[12] * (m[6] * m[11] - m[10] * m[7]);
			break;

			// r=1, c=1
			/* 00 08 12
			  02 10 14
			  03 11 15 */
		case 5:
			result = +m[0] * (m[10] * m[15] - m[14] * m[11])
				- m[8] * (m[2] * m[15] - m[14] * m[3])
				+ m[12] * (m[2] * m[11] - m[10] * m[3]);
			break;

			// r=1, c=2
			/* 00 04 12
			  02 06 14
			  03 07 15 */
		case 6:
			result = -m[0] * (m[6] * m[15] - m[14] * m[7])
				+ m[4] * (m[2] * m[15] - m[14] * m[3])
				- m[12] * (m[2] * m[7] - m[6] * m[3]);
			break;

			// r=1, c=3
			/* 00 04 08
			  02 06 10
			  03 07 11 */
		case 7:
			result = +m[0] * (m[6] * m[11] - m[10] * m[7])
				- m[4] * (m[2] * m[11] - m[10] * m[3])
				+ m[8] * (m[2] * m[7] - m[6] * m[3]);
			break;

			// r=2, c=0
			/* 04 08 12
			  05 09 13
			  07 11 15 */
		case 8:
			result = +m[4] * (m[9] * m[15] - m[13] * m[11])
				- m[8] * (m[5] * m[15] - m[13] * m[7])
				+ m[12] * (m[5] * m[11] - m[9] * m[7]);
			break;

			// r=2, c=1
			/* 00 08 12
			  01 09 13
			  03 11 15 */
		case 9:
			result = -m[0] * (m[9] * m[15] - m[13] * m[11])
				+ m[8] * (m[1] * m[15] - m[13] * m[3])
				- m[12] * (m[1] * m[11] - m[9] * m[3]);
			break;

			// r=2, c=2
			/* 00 04 12
			  01 05 13
			  03 07 15 */
		case 10:
			result = +m[0] * (m[5] * m[15] - m[13] * m[7])
				- m[4] * (m[1] * m[15] - m[13] * m[3])
				+ m[12] * (m[1] * m[7] - m[5] * m[3]);
			break;

			// r=2, c=3
			/* 00 04 08
			  01 05 09
			  03 07 11 */
		case 11:
			result = -m[0] * (m[5] * m[11] - m[9] * m[7])
				+ m[4] * (m[1] * m[11] - m[9] * m[3])
				- m[8] * (m[1] * m[7] - m[5] * m[3]);
			break;

			// r=3, c=0
			/* 04 08 12
			  05 09 13
			  06 10 14 */
		case 12:
			result = -m[4] * (m[9] * m[14] - m[13] * m[10])
				+ m[8] * (m[5] * m[14] - m[13] * m[6])
				- m[12] * (m[5] * m[10] - m[9] * m[6]);
			break;

			// r=3, c=1
			/* 00 08 12
			  01 09 13
			  02 10 14 */
		case 13:
			result = +m[0] * (m[9] * m[14] - m[13] * m[10])
				- m[8] * (m[1] * m[14] - m[13] * m[2])
				+ m[12] * (m[1] * m[10] - m[9] * m[2]);
			break;

			// r=3, c=2
			/* 00 04 12
			  01 05 13
			  02 06 14 */
		case 14:
			result = -m[0] * (m[5] * m[14] - m[13] * m[6])
				+ m[4] * (m[1] * m[14] - m[13] * m[2])
				- m[12] * (m[1] * m[6] - m[5] * m[2]);
			break;

			// r=3, c=3
			/* 00 04 08
			  01 05 09
			  02 06 10 */
		case 15:
			result = +m[0] * (m[5] * m[10] - m[9] * m[6])
				- m[4] * (m[1] * m[10] - m[9] * m[2])
				+ m[8] * (m[1] * m[6] - m[5] * m[2]);
			break;

		default:
			break;
		}

		return result;
	}

	//! Calculates the inverse matrix
	/**
	 * The determinant of the matrix must not be zero.
	 * \returns the inverted matrix
	 */
	inline Matrix Inverse() const
	{
		float d = Det();
		assert(!IsZero(d));

		float result[16] = { 0.0f };

		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				// Already transposed!
				result[4 * r + c] = (1.0f / d) * Cofactor(r, c);
			}
		}

		return Matrix(result);
	}

	//! Calculates the multiplication of this matrix * given matrix
	/**
	 * \param right right-hand matrix
	 * \returns multiplication result
	 */
	inline Matrix Multiply(const Matrix &right) const
	{
		float result[16] = { 0.0f };

		for (int c = 0; c < 4; ++c)
		{
			for (int r = 0; r < 4; ++r)
			{
				result[4 * c + r] = 0.0f;
				for (int i = 0; i < 4; ++i)
				{
					result[4 * c + r] += m[4 * i + r] * right.m[4 * c + i];
				}
			}
		}

		return Matrix(result);
	}


#endif  // TOBE_CONTINUED

}MATRIX; // struct Matrix
// Matrix 









// \struct Vector

// \brief 3D (3x1) vector,  a universal 3x1 vector.
// All methods are made inline to maximize optimization.
// Unit tests for the structure and related functions are in module: math/test/vector_test.cpp.

// 2-维向量视为3-维, z=0.

typedef class Vector{

private:
float *a=0L;  
int n=4;   // 3

public:    // 
// names of first 3 coords  // inline 关键字并无必要  
float x() { if (!a || n < 1) return 0;  return a[0]; }
float y() { if (!a || n < 2) return 0;  return a[1]; }
float z() { if (!a || n < 3) return 0;  return a[2]; }
float t() { if (!a || n < 4) return 0;  return a[3]; }
		
inline  //! Creates a zero vector (0, 0, 0)   inline 关键词没有必要 
Vector(const float* _v=0, int _n=4){      // 默认 4 维.  _v 容量小于 4 时可能出错!   
n=_n;  if (n < 4)n = 4;  
a = (float*) malloc(n*sizeof(float));  memset(a, 0, n*sizeof(float));    a[3]=1;  
if(_v) for (int i = 0; i < n; i++) a[i] = _v[i];   
}

~Vector() { if(0)qTrace("a=(%.1f, %.1f, %.1f)... free(a)", a[0],a[1],a[2]);   free(a);  }


// friend  // friend -- 允许函数访问 private 成员 

float& operator [](int i) const   // 后边(而非前面) 加 const 表示可以是 const Vector 的运算符! 
{ float r=0;  if(i<0 || i>=n) return r; return a[i];  }


 




//! Creates a vector from given values  -- Constructors declared explicit will not be considered for implicit conversions.  

// 如果没有 explicit,  且构造函数只有一个参数时,  在编译的时候就会有一个缺省的 implicit 转换操作. 

// 例如

//  class CxString {
//	 ... 
//		explicit CxString(int size) (int size) {...}
//	 ... 
// };

// CxString string1(24);     // 这样是OK的  
// CxString string2 = 24;    // 这样是不行的, 因为 explicit 关键字取消了隐式转换. 如果没有 explicit, 则是OK的, 且与上句效果相同.   



// explicit  // It is meaningless to apply explicit to constructors with multiple arguments, since such constructors cannot take part in implicit conversions.
Vector(float _x, float _y, float _z=0){  // : x(_x), y(_y), z(_z)
	if (n < 4) n = 4; 
	a = (float*)realloc(a,n * sizeof(float));
	a[0] = _x;  a[1] = _y; a[2] = _z; a[3] = 1; 
}





void Zero() { for (int i = 0; i < n; i++) a[i] = 0; }  //! Loads the zero vector (0, 0, 0)

operator float*() { return a; }  // cast operator --- return (float*)this; 
operator const float*() { return a; }  // cast to const float* 
	
float Norm() const {	float d = 0; for (int i = 0; i < 3; i++)  d += a[i] * a[i]; return sqrtf(d); }  //! Returns the vector length

void Normalize() {	float d = Norm(); if (IS_ZERO(d))return;  for (int i = 0; i < n; i++) a[i] /= d; }	//! Normalizes the vector









#if ARITHEMETIC_OPERAS|1     // +  -  ×  ÷  


//! Adds two vectors
friend const
Vector& operator+(const Vector &left, const Vector &right) 
{
    Vector* v = new Vector( left.a);  // 不带 .a 不行, 可能是"复制构造函数"惹的祸, a指向left.a, 而不是新分配内存. 
    for (int i = 0; i < left.n; i++) v->a[i] += right.a[i];
    return *v;
}


 

//! Returns the inverted vector
//inline 
const Vector& operator-() const { for (int i = 0; i < n; i++) a[i] = -a[i];   return *this; }   // const Vector& 才可以 return  *this


 

//! Subtracts two vectors 
friend   // const
Vector& operator-(const Vector&left, const Vector &right) {

// Vector  t=Vector(left[0] - right[0], left[1] - right[1], left[2] - right[2]);  return t;   // 这样不行!  注意变量生存周期. 
// return Vector(left[0] - right[0], left[1] - right[1], left[2] - right[2]);    // 为什么这样就可以呢? (去掉返回类型中的 &)

Vector* v = new Vector(left.a); // Vector* v=new Vector(left); // 不行 -- 注意"Copy Constructor"  
for (int i = 0; i < right.n; i++) v->a[i] -= right.a[i];
return *v;
}

 


//! Subtracts two vectors 
friend   // const
Vector& operator-(const float*&left, const Vector &right) {

    // Vector  t=Vector(left[0] - right[0], left[1] - right[1], left[2] - right[2]);  return t;   // 这样不行!  注意变量生存周期. 
    // return Vector(left[0] - right[0], left[1] - right[1], left[2] - right[2]);    // 为什么这样就可以呢? (去掉返回类型中的 &)

    Vector* v = new Vector(left); // Vector* v=new Vector(left); // 不行 -- 注意"Copy Constructor"  
    for (int i = 0; i < right.n; i++) v->a[i] -= right.a[i];
    return *v;
}


 


//! Subtracts two vectors
//inline friend const 
// Vector operator-(const Vector &left, const Vector &right){    return Vector(left[0] - right[0], left[1] - right[1], left[2] - right[2]);  }




//! Subtracts the given vector
inline  const
Vector& operator-=(const Vector &right) {
    assert(right.n == n);
    for (int i = 0; i < n; i++)  a[i] -= right.a[i];
    return *this;
}

Vector& operator-=(const float* right) {
    if (!right) return *this;
    for (int i = 0; i < n; i++) a[i] -= right[i];
    return *this;
}





//! Multiply
// inline     const // 
friend
float operator*(const Vector &u, const Vector &v) 
{
int m=v.n;  if(m==4) m=3;  
    float r = 0; 
    for (int i = 0; i < m; i++) r += u[i] * v[i];
    return r;
}


friend
float operator*(const float*u, const Vector &v)
{
    int m = v.n;  if (m == 4) m = 3;
    float r = 0;  
    for (int i = 0; i < m; i++) r += u[i] * v[i];
    return r;
}



//! Divide  -- 实际上是指外积  u×v 

//! Convenience function for calculating cross product
// inline Vector CrossProduct(const Vector &left, const Vector &right){    return left.CrossMultiply(right); }


//! Calculates the cross product with another vector \param right right-hand side vector \returns the cross product

// inline Vector CrossMultiply(const Vector &right) const{  float px = y * right.z - z * right.y;   
// float py = z * right.x - x * right.z;  float pz = x * right.y - y * right.x;  return Vector(px, py, pz);  }
  
inline   friend  // 仅针对 3-维向量 
const  Vector& operator /(const Vector &u, const Vector &v)   {  
// return Vector(u[1]*v[2]-u[2]*v[1],   u[2]*v[0]-u[0]*v[2],   u[0]*v[1]-u[1]*v[0]);     // 这样要求返回类型必须带 const, 为 const Vector&, 但仍然有内存错误!
return *new Vector(u[1]*v[2]-u[2]*v[1],   u[2]*v[0]-u[0]*v[2],   u[0]*v[1]-u[1]*v[0]);   // 这样不必带 const 
}


//! Divides by given vector
inline const
Vector& operator/=(const Vector &right) {  return *this/right;  }












//! Multiplies vector by scalar
inline friend const Vector operator*(const float &left, const Vector &right)
{
    return Vector(left * right[0], left * right[1], left * right[2]);
}

//! Multiplies vector by scalar
inline friend const Vector operator*(const Vector &left, const float &right)
{
    return Vector(left[0] * right, left[1] * right, left[2] * right);
}


/*//! Multiplies vector by scalar
friend  // inline const 
Vector& operator*(const float &left, const Vector &right){
Vector v= right; 
for(int i=0; i<v.n; i++) v[i]*= left;
return v;
}

//! Multiplies vector by scalar
inline friend const 
Vector& operator*(const Vector &left, const float &right){
if(right==0) return left;
    Vector v = left;
    for (int i = 0; i < v.n; i++) v[i] /= right;
    return v;
}

*/


//! Divides by given scalar
// inline const 
Vector& operator/=(const float &right){
if(right==0) return *this; 
for (int i = 0; i < n; i++) a[i] /= right;
return *this;
}


//! Multiplies by given scalar
inline const 
Vector& operator*=(const float &right){
    for (int i = 0; i < n; i++) a[i] *= right;
    return *this;
}








#endif  // ARITHEMETIC_OPERS




#if TOBE_CONTINUED
 


	//! Calculates the dot product with another vector
	/**
	 * \param right right-hand side vector
	 * \returns the dot product
	 */
	inline float DotMultiply(const Vector &right) const
	{
		return x * right.x + y * right.y + z * right.z;
	}

	//! Returns the cosine of angle between this and another vector
	inline float CosAngle(const Vector &right) const
	{
		return DotMultiply(right) / (Length() * right.Length());
	}

	//! Returns angle (in radians) between this and another vector
	inline float Angle(const Vector &right) const
	{
		return acos(CosAngle(right));
	}


	/* Operators */


	//! Adds the given vector
	inline const Vector& operator+=(const Vector &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}



	







	//! Divides vector by scalar
	inline friend const Vector operator/(const Vector &left, const float &right)
	{
		return Vector(left.x / right, left.y / right, left.z / right);
	}


	//! Returns a string "[x, y, z]"
	inline std::string ToString() const
	{
		std::stringstream s;
		s.precision(3);
		s << "[" << x << ", " << y << ", " << z << "]";
		return s.str();
	}


#endif  // TOBE_CONTINUED

} VECTOR;
// struct Vector





inline 
void t_Vector() {
VECTOR u(1, 2, 3), w(4,5,6);
	 
Vector v=w+u; 
qTrace("(%.1f,%.1f,%.1f)", v[0],v[1],v[2]);
}


#endif    // VECTOR_MATRIX







int& getr();
void setr(int v);



template<class T> 
int triz(T*A, int m, int n, T*P);



// 梯形化模板. 返回行变换矩阵 P. 
template <class T>
T*& matr_triz(T*A, int m, int n, T*P){
setr(0); 

if(P){   // P 初始化为单位阵
memset(P,0,n*n*sizeof(T)); 
for(int i=0; i<n; i++)  *LOCK(P,n,n,i,i)=1;
}

triz(A, m, n, P); 

return P; 
}





template <class T>    // T*&C 否则函数内部申请的C的地址无以传出
T*& matr_x(const T*A, const T*B, T*& C = 0, int m = 4, int s = 4, int n = 4); 

template < class T >
T matr_inv(T*mat, int N);














// show_matrix, 显示矩阵A=a[], A is row-major, etc.  sza 为附加说明   
// | a0  a1  a2 |
// | a3  a4  a5 |
// | a6  a7  a8 |  
// or A is column-major, etc. 
// | a0  a3  a6 |
// | a1  a4  a7 |
// | a2  a5  a8 |  
// 若参数 A=0, 则仅打开文件 show_matrix.txt  



template<class T>   // c++编译期间不能决定模板参数的类型,  不能生成实例, 会带到链接期间. 此时若无调用特定类型实例, 则会导致错误 LINK1120、2019.  
void show_matr(const T*A=0, int m=0, int n=0, const char*sza=0) {  // =0 ... 

    if (!A) { ShellExecute(0, "open", "show_matr.txt", 0, 0, SW_SHOW);  return; }

    static int ic; ic++;  // if(ic>1) return;  // 测试 ...  

    int b_int = 1;
    // static char fmt[128]="%.1f\t";  // sprintf(fmt,"%c.%df\t",'%',b_int);   // printf("\n\n..........\n%s\n..........\n",fmt,0,0);



FILE*fp = 0;

 //   if (ic == 1) fp = fopen("show_matr.txt", "wb"); \
    else         fp = fopen("show_matr.txt", "a+b");
    
fp = fopen("show_matr.txt", "a+b");   // 手工清理文件内容 



    if (sza) fprintf(fp, "\r\n[%d] %s\r\n\r\n", ic, sza);
    else    fprintf(fp, "\r\n[%d] \r\n\r\n", ic);


    const T* p = A;  // a(i,j)=a[p], p= i*n +j,  i=p\n, j=p modn   

    for (int i = 0; i < m; i++) {

        p = ROW_PTR(A, m, n, i);

        for (int j = 0; j < n; j++) {



#if SHOW_FRACTION|0
            if (*p == 0)  fprintf(fp, "%.0f\t", *p);
            else {
                T v = *p;   //  { T u=v; u*=1E30; u=u-floor(u);    if(u!=0)  qTrace("v=%.12f", v); } // 
                int s = 1;
                if (v < 0) { s = -1; v = -v; }

                int a, b; frac(v, &a, &b);

                if (b == 1) fprintf(fp, "%d\t", s*a);
                else     fprintf(fp, "%d/%d\t", s*a, b);
            }

#else

            fprintf(fp, "% .3f\t", *p);

#endif  // SHOW_FRACTION 

            INC(p, m, n);  // p++; 

        }  // 显示i行



        fprintf(fp, "\r\n");

    } // for i

    fprintf(fp, "\r\n\r\n");

    fclose(fp);


    // if(bShow) ShellExecute(0,"open","show_matrix.txt",0,0,SW_SHOW); 
}












void t_cofr();
void t_frac();
void t_triz(); 
void t_inv(); 









#endif  // _MATRIX_H_ //


