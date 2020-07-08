

#pragma warning(disable:4996)



#define _GLWT_CPP_ 

#include "glWT.h"

#include <Windows.h> 

#include <math.h>    // floor  

#include "InterfaceMDI/qTrace.h"


#include "Matrix.h" 

#include <gl/GL.h>           // glGetFloatv
#include <gl/GLU.h>          // gluLookAt 


#if OGL_MATRICES|1           // see Colobot/math/matrix.h、geometry.h 


//! Loads an othogonal projection matrix   \param mat  result matrix   
//  Rt 表示 Clip Plane:  
//  (l,t)-----------------|
//   |                    |
//   |-------------------(r,b)

// 参见  glOrtho

//  2/(r-l)          0          0             -(r+l)/(r-l)      
//                2/(t-b)       0             -(t+b)/(t-b)
//                             2/(n-p)        -(p+n)/(p-n)
//                              0             -1



float* Ortho(float* mat, float*Rt, float zNear = -1.0f, float zFar = 1.0f) {

    memset(mat, 0, 16 * sizeof(float));    // for(int i=0; i<4; i++) mat[i+i*4]=1;   // 单位阵 
    mat[3 + 3 * 4] = 1;   // LoadOrthoProjectionMatrix 指定为 +1, MSDN 为 -1. 测试说明后者错.    

    float   l = Rt[0], t = Rt[1], r = Rt[2], b = Rt[3];

    mat[0 + 0 * 4] = 2.0f / (r - l);                      // (1,1)  
    mat[1 + 1 * 4] = 2.0f / (t - b);                      // (2,2) 
    mat[2 + 2 * 4] = 2.0f / (zNear - zFar);                // (3,3) 

    qTrace("mat(3,3)=%.2f,  v=%.2f", mat[2 + 2 * 4], 2.0f / (zNear - zFar));

    mat[0 + 3 * 4] = -(r + l) / (r - l);                    // (1,4)  
    mat[1 + 3 * 4] = -(t + b) / (t - b);                    // (2,4)  
    mat[2 + 3 * 4] = -(zFar + zNear) / (zFar - zNear);      // (3,4) 
    return mat;
}




//! Loads a perspective projection matrix, column-major, LoadProjectionMatrix,  see  gluPerspective
// \param mat        result matrix  \param fov        field of view in radians   -- 为与 gluPerspective 保持一致, 改为 in degree 
// \param aspect     aspect ratio (width / height)
// \param nearPlane  distance to near cut plane  \param farPlane   distance to far cut plane
// v=fov,  a=aspect, p=farPlane, n=nearPlane 

//  ctg(v/2)/a 
//               ctg(v/2)
//                         (n+p)/(n-p)     2p*n/(n-p)
//                         -1              0


float* Perspective(float*mat, float fov = 45.0, float aspect = 1.0f,  float nearPlane = 1.0f, float farPlane = 1000.0f){

assert(fabs(farPlane - nearPlane) >= 0.01f);    assert(fabs(sin(fov / 2)) >= 0.01f);

fov*=PI/180; 

float f = cosf(fov / 2.0f) / sinf(fov / 2.0f);

memset(mat,0,16*sizeof(float));  
mat[0+0*4] = f / aspect;                                                      // (1,1) 
mat[1+1*4] = f;                                                               // (2,2)  
mat[2+2*4] = (nearPlane + farPlane) / (nearPlane - farPlane);                 // (3,3)   
mat[2+3*4] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);          // (3,4)   
mat[3+2*4] = -1.0f;                                                           // (4,3)  
return mat;                                         
}



// Frustrum, see glFrustrum(left, right, bottom,top, n,p),  与 glPerspective 可以相互取代  
//  Rt = (left, top, right, bottom)    w=right-left,  h= bottom- top  

//  2n/(r-l)         0         (r+l)/(r-l)
//               2n/(t-b)      (t+b)/(t-b)
//                             (n+p)/(n-p)     2p*n/(n-p)
//                             -1              0


// gluPerspective(fov, as, zNear, zFar) 可以被 glFrustrum 取代: 

// rFov = fov * PI / 180.0;
// glFrustum( -zNear * tan(rFov / 2.0) * as,     zNear * tan(rFov / 2.0) * as, -zNear * tan(rFov / 2.0),  zNear * tan(rFov / 2.0),   zNear, zFar);

//  Rt 表示 Clip Plane:  
//  (l,t)-----------------|
//   |                    |
//   |-------------------(r,b)


float* Frustum(float*mat, float*Rt, float n=1, float p=1000.){

if(!Rt) return mat; 

assert( fabs(p - n) >= 0.01f );    // assert(fabs(sin(fov / 2)) >= 0.01f);

float   l=Rt[0], t=Rt[1], r=Rt[2], b=Rt[3]; 
// float   w=Rt[2]-Rt[0], h=Rt[3] - Rt[1] ; 



    memset(mat, 0, 16 * sizeof(float));

    mat[0 + 0 * 4] = 2*n / (r-l);                                                      // (1,1) 
    mat[1 + 1 * 4] = 2*n / (t-b);                                                      // (2,2)  
    mat[2 + 2 * 4] = (n + p) / (n - p);                                                // (3,3)   
    mat[2 + 3 * 4] = (2.0f * p * n) / (n - p);                                         // (3,4)   
    mat[3 + 2 * 4] = -1.0f;                                                            // (4,3)  
   
    mat[0 + 2 * 4] = (r+l) / (r-l);                                                    // (1,3) 
    mat[1 + 2 * 4] = (b+t) / (b-t);                                                    // (2,3)  

 
return mat;
}




//! Make view matrix, LoadViewMatrix, see gluLookAt(eye, center, up)
 
// \param mat      result matrix
// \param from     origin, center  (0,0,0) 
// \param at       view direction, eye (0,0,1)  
// \param Up       up vector, (0,1,0)
 
// 


float* LookAt(float* mat,  const float* at, const float* from=0,const float* Up=0){

float vUp[4]={0,1,0,1}, vCenter[4]={0,0,0,1}; 
if(!from) from=vCenter;
if(!Up) Up=vUp;  

Vector view = at;   // Get the z basis vector, which points straight ahead. This is the  difference from the eyepoint to the lookat point.
view -= from;

float length = view.Norm();  assert(!IS_ZERO(length));
view /= length;	 // Normalize the z basis vector

// Get the dot product, and calculate the projection of the z basis vector onto the up vector. The projection is the y basis vector.
 
float dotProduct = Up*view;   // 先把 Up 转为中间变量 Vector 再相乘, 完了还要析构中间变量

Vector  up = Up- dotProduct * view;  // 注意重载运算符  const float*& - const Vector&

#if RESET_UP|1

	// If this vector has near-zero length because the input specified a bogus假的 up vector, let's try a default up vector
 
	if (IS_ZERO(length = up.Norm()))
	{
		up = Vector(0.0f, 1.0f, 0.0f) - view[1] * view;

		if (IS_ZERO(length = up.Norm()))		// If we still have near-zero length, resort to a different axis.
		{
			up = Vector(0.0f, 0.0f, 1.0f) - view[2] * view;

			assert(!IS_ZERO(up.Norm()));
		}
	}

	up /= length;	// Normalize the y basis vector

#endif  // RESET_UP


Vector right = up/view;   // ×  The x basis vector is found simply with the cross product of the y and z basis vectors



// build the matrix. The first three rows contains the basis vectors used to rotate the view to point at the lookat point
memset(mat, 0, 16*sizeof(float));  for(int i=0; i<4; i++) mat[i+i*4]=1; 

for (int j = 0; j < 3; j++) {  // column-major   i + j*m 
mat[0+j*4] = right[j];         // 第1行 = right
mat[1+j*4] = up[j];            // 第2行 = up
mat[2+j*4] = view[j];          // 第3行 = view
}


// qTrace("(%.2f,%.2f,%.2f) * (%.2f,%.2f,%.2f)", from[0], from[1], from[2], right[0], right[1], right[2]);
mat[0+3*4] = -(at*right);      // [0,3] 第4列 j=3  // Do the translation values (rotations are still about the eyepoint)
mat[1+3*4] = -(at*up);         // [1,3]
mat[2+3*4] = -(at*view);       // [2,3]
	

return mat; 
}



float* LookAtf(float *mt, float *vE, float *vO=0, float *vUp=0) {

    float vup[4] = { 0,1,0,1 }, vcenter[4] = { 0,0,0,1 };    // 默认值  
    if (!vO) vO = vcenter;
    if (!vUp) vUp = vup;

float Vz[3]={vO[0] - vE[0], vO[1] - vE[1],   vO[2] - vE[2]  };  
float d=Vz[0]*Vz[0]+ Vz[1] * Vz[1] + Vz[2] * Vz[2];   d=sqrt(d); // NormalizeVector(Vz)  
for(int i=0; i<3; i++)  Vz[i]/=d; 

// ComputeNormalOfPlane(Vx, Vz, vUp) 
float Vx[3]={ Vz[1] * vUp[2] - Vz[2] * vUp[1],    Vz[2] * vUp[0] - Vz[0] * vUp[2],    Vz[0]*vUp[1]-Vz[1]*vUp[0], } ;   // Vx = Vz x up
d=Vx[0]*Vx[0]+ Vx[1] * Vx[1] + Vx[2] * Vx[2];    d=sqrt(d); // NormalizeVector(Vx) 
for(int i=0; i<3; i++)  Vx[i]/=d; 

//Recompute up as: up = Vx x Vz,     ComputeNormalOfPlane(up, Vx, Vz);
float up[3]={ Vx[1] * Vz[2] - Vx[2] * Vz[1],    Vx[2] * Vz[0] - Vx[0] * Vz[2],    Vx[0]* Vz[1]-Vx[1]* Vz[0], } ;       // up = Vx x Vz

for(int j=0; j<3; j++){
mt[0+ j*4]=  Vx[j];
mt[1+ j*4]=  up[j];
mt[2+ j*4]=  -Vz[j];
mt[3+ j*4]= 0;
}

// -A^T*b


mt[0 + 3 * 4] =  -(mt[0 + 0 * 4] * vE[0] + mt[0 + 1 * 4] * vE[1] + mt[0 + 2 * 4] * vE[2] ) ;  //  4-列
mt[1 + 3 * 4] =  -(mt[1 + 0 * 4] * vE[0] + mt[1 + 1 * 4] * vE[1] + mt[1 + 2 * 4] * vE[2] ) ;
mt[2 + 3 * 4] =  -(mt[2 + 0 * 4] * vE[0] + mt[2 + 1 * 4] * vE[1] + mt[2 + 2 * 4] * vE[2] ) ;

// MultiplyMatrices4by4OpenGL_FLOAT(resultMatrix, matrix, mt);

// glhTranslatef2(resultMatrix,    -vE[0], -vE[1], -vE[2]);
// mt[3 + 0 * 4] =  mt[3 + 1 * 4] =  mt[3 + 2 * 4] = 0;   //  4-行

mt[3 + 3 * 4] = 1;  

//  memcpy(matrix, resultMatrix, 16 * sizeof(float));
return mt;
}






// Viewport 变换矩阵,    △ NDC Space  -- Viewport Mapping   -->    △ Screen Space     

// 实际上是2-维变换. 参见  glViewport   
// xw=(xnd+1)*w/2+x
// yw=(ynd+1)*h/2+y
// zw=(znd+1)*(p-n)/2+n   // z-还与剪切平面有关  --- z 可以单独进行变换    


//  w/2                           w/2+x       
//            h/2                 h/2+y   
//                    (p-n)/2    (p+n)/2    // z-还与剪切平面有关  --- z 可以单独进行变换    
//                                1             

// https://blog.csdn.net/wangdingqiaoit/article/details/51589825


// 这里规定 vp = viewport =(x,y,w,h,   n,p ) 多出两个参数  n--nearPlane, p-- farPlane 
float* Viewport(float *mt, float*vp) {
if(!vp) return mt;
float x=vp[0], y=vp[1], w=vp[2], h=vp[3], n=vp[4], p=vp[5];   // 6 个分量 

memset(mt, 0, 16 * sizeof(float));  for(int i=0; i<4; i++) mt[i+i*4]=1;   

    mt[0 + 0 * 4] = w/2;           // (1,1) 
    mt[1 + 1 * 4] = h/2;           // (2,2) 
 
    mt[2 + 2 * 4] = (p-n)/2;       // (3,3)  

    mt[0 + 3 * 4] = w/2+x;         // (1,4) 
    mt[1 + 3 * 4] = h/2+y;         // (2,4)  

    mt[2 + 3 * 4] = (p+n)/2;       // (3,4)  



return mt;
}





#endif  // OGL_MATRICES





// 世界坐标变换为屏幕坐标. 参见 gluProject


float* ToScreen(const float*xo, float*xs){

if(!xo || !xs) return xs;

float mv[16], mp[16], vp[6],   mt[16];
float* mc=new float[16], * mr=new float[16];   // vp=(x,y,w,h)

float* xt=new float[4]; 

glGetFloatv(GL_MODELVIEW_MATRIX, mv);
glGetFloatv(GL_PROJECTION_MATRIX, mp);
glGetFloatv(GL_VIEWPORT, vp);  


// 为 vp 补充两个参数 
float nf[2]; glGetFloatv(GL_DEPTH_RANGE, nf);
// xs[2] = xo[2] * (nf[1] - nf[0]) / 2 + (nf[0]+nf[1])/2;
// qTrace("%.2f,%.2f,%.2f,%.2f  n=%.2f, p=%.2f", vp[0], vp[1], vp[2], vp[3],  nf[0], nf[1]);
vp[4]=nf[0];  vp[5]=nf[1];  

Viewport(mt,vp);       // mt, Viewport 变换矩阵 
// matr_x(mv, mp, mc);   matr_x(mc, mt, mr); 

// show_matr(mv, 4, 4, "ModelView Matrix");
// show_matr(mt,4,4,"Viewport Matrix");

matr_x(mv,xo, xs, 4,4,1); 
matr_x(mp,xs, xt, 4,4,1); 
matr_x(mt,xt, xs, 4,4,1); 

if(xs[3]){ for(int i=0; i<3; i++)  xs[i]/=xs[3];  xs[3]=1; }



#if CHECK_RESULT |0
// qTrace("X (%.2f, %.2f, %.2f) --> (%.2f, %.2f, %.2f)", xo[0], xo[1], xo[2],   xs[0], xs[1], xs[2]);

show_matr(xo,4,1,"物体坐标");
show_matr(xs,4,1,"屏幕坐标");

if(1){

double mv[16], mp[16]; int vp[4];
glGetDoublev(GL_MODELVIEW_MATRIX, mv);  
glGetDoublev(GL_PROJECTION_MATRIX, mp);
glGetIntegerv(GL_VIEWPORT, vp);
// qTrace("%d,%d,%d,%d", vp[0], vp[1], vp[2], vp[3]);

// show_matr(mv, 4, 4, "ModelView Matrix");



double yo[4]={0,0,0,1}, ys[4]={0,0,0,1};  for(int i=0; i<4; i++) yo[i]=xo[i]; 

// memset(mv,0, 16*sizeof(double));  for(int i=0; i<4; i++) mv[i+i*4]=1; 
// memset(mp,0, 16*sizeof(double));  for(int i=0; i<4; i++) mp[i+i*4]=1; 


gluProject(yo[0], yo[1], yo[2], mv,mp, vp, &ys[0], &ys[1], &ys[2] ); 


// qTrace("Y (%.2f, %.2f, %.2f) --> (%.2f, %.2f, %.2f)", yo[0], yo[1], yo[2],   ys[0], ys[1], ys[2]);

show_matr(yo,4,1,"gluProject, 物体坐标");
show_matr(ys,4,1,"gluProject, 屏幕坐标");

}

#endif  //  CHECK_RESULT


delete[] mc,mr;  
return xs;
}




// 屏幕坐标变换为世界坐标. 参见 gluUnProject

float* ToWorld( const float*xs, float*xo) {

    if (!xo || !xs) return xo;

    float mv[16], mp[16],    vp[6], mt[16];
    float* mc = new float[16], *mr = new float[16];   // vp=(x,y,w,h)


    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    glGetFloatv(GL_PROJECTION_MATRIX, mp);
    glGetFloatv(GL_VIEWPORT, vp);


    // 为 vp 补充两个参数 
    float nf[2]; glGetFloatv(GL_DEPTH_RANGE, nf);
    vp[4] = nf[0];  vp[5] = nf[1];
    Viewport(mt, vp);       // mt, Viewport 变换矩阵 

// 先把矩阵乘起来再求逆  

// show_matr(mv,4,4,"ModelViewMatrix float ");
    
// memset(mv, 0, 16 * sizeof(float));  for (int i = 0; i < 4; i++) mv[i + i * 4] = 1;
// memset(mp, 0, 16 * sizeof(float));  for (int i = 0; i < 4; i++) mp[i + i * 4] = 1;




#if  STEP_INV |0


float* _xs=new float[4];  for (int i = 0; i < 4; i++) _xs[i] = xs[i];

matr_inv(mt, 4);  matr_x(mt, xs, xo, 4, 4, 1);  
matr_inv(mp, 4);  matr_x(mp, xo, _xs, 4, 4, 1);  
matr_inv(mv, 4);  matr_x(mv, _xs, xo, 4, 4, 1);


#if CHECK_MATRIX

float* mtt=new float[16];  // 测试, 记录逆阵 
float*  mrr=new float[16];
matr_x((float*)mp, (float*)mt,  mtt, 4, 4, 4);
matr_x((float*)mv, (float*)mtt,  mrr, 4, 4, 4);

show_matr(mrr,4,4,"复合逆阵"); 

#endif  // CHECK_MATRIX


#else

matr_x(mp, mv, mc, 4, 4, 4);    //  mp*mv 
matr_x(mt, mc,mr, 4, 4, 4);     //  mt*mp*mv

// show_matr(mr,4,4,"乘积");  \
memcpy(mp,mr,16*sizeof(float));   // 乘积备份   

matr_inv(mr, 4);    //  show_matr(mr, 4,4,"matr_inv 逆阵");

// memcpy(mv,mr,16*sizeof(float));   // 备份 

matr_x(mr, xs, xo, 4, 4, 1);  if (xo[3]) { for (int i = 0; i < 3; i++)  xo[i] /= xo[3];  xo[3] = 1; }


#if CHECK_MATRIX|0

show_matr(xo, 4,1," matr_inv 逆矩阵变换得到的物体坐标"); 

// matr_x(mr, mp, mc, 4,4,4);  show_matr(mc, 4,4,"matr_inv 逆阵检验");


memcpy(mt,mp,16*sizeof(float));    show_matr(mt,4,4,"乘积备份");

// setr(0); triz(mp,4 ,4, mc); 
matr_triz(mp,4,4,mc); show_matr(mc, 4,4,"matr_triz 逆阵");



matr_x(mc, xs, xo, 4, 4, 1);  if (xo[3]) { for (int i = 0; i < 3; i++)  xo[i] /= xo[3];  xo[3] = 1; }
show_matr(xo, 4,1," triz 逆矩阵变换得到的物体坐标"); 

matr_x(mt, mc, mr, 4,4,4);  show_matr(mr, 4,4,"triz 逆阵检验");

#endif  // CHECK_MATRIX


#endif   //  STEP_INV



if (xo[3]) { for (int i = 0; i < 3; i++)  xo[i] /= xo[3];  xo[3] = 1; }


#if CHECK_RESULT |0
   

    if (1) {

        double mv[16], mp[16]; int vp[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, mv);
        glGetDoublev(GL_PROJECTION_MATRIX, mp);
        glGetIntegerv(GL_VIEWPORT, vp);


// show_matr(mv, 4, 4, "ModelView Matrix double");

double  yo[4] = { 0,0,0,1 }; 
double  ys[4] = { 0,0,0,1 };  for (int i = 0; i < 4; i++) ys[i] = xs[i];



//memset(mv,0, 16*sizeof(double));  for(int i=0; i<4; i++) mv[i+i*4]=1; 
//memset(mp,0, 16*sizeof(double));  for(int i=0; i<4; i++) mp[i+i*4]=1; 


        gluUnProject(ys[0], ys[1], ys[2], mv, mp, vp, &yo[0], &yo[1], &yo[2]);



        show_matr(yo, 4, 1, "gluUnProject 变换得到的物体坐标");
        

    }

#endif  //  CHECK_RESULT


delete[] mc, mr;
return xo;
}








void t_glWT(){

// Opengl Transformation 

// [ModelView Matrix] 
//    Local(Object) Space      -- Modelling Transformation -->    
//    World Space              -- Viewing Transformation   -->    

// [Projection Matrix]
//    Eye(View) Space          -- Projection Transformation-->    
//  △ Clip Space               -- Perspective Divide       -->
    
// [Viewport] 
//  △ NDC Space                -- Viewport Mapping         -->    
//  △ Screen Space 

// 注意, OpenGL 只定义了裁剪坐标系、规范化设备坐标系和屏幕坐标系. 
// 局部坐标系(模型坐标系)、世界坐标系、照相机坐标系都是为了方便用户设计而自定义的. 
// OpenGL中相机始终位于原点,指向-Z轴.


float vm[16], pm[16];     
float vm_t[16]={0}, pm_t[16]={0};    // memset(vm_t, 0, 16 * sizeof(float));
 
// Vector eye(2, 1, 10.5), center(0, 0, 0), up(0, 1, 0);

Vector eye(4, 2, 21), center(0, 0, 0), up(0, 2, 0);


#if TEST_MODELVIEW|0

    glMatrixMode(GL_MODELVIEW);   glLoadIdentity();

    glGetFloatv(GL_MODELVIEW_MATRIX, vm);
    show_matr(vm, 4, 4, "ModelViewMatrix 此时应当为单位阵");

    //Vector eye(0,0,3),  center(0,0,0), up(0,1,0); 


    // gluPerspective(60, 1, 0.1, 1000.0f);
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glGetFloatv(GL_MODELVIEW_MATRIX, vm);
    show_matr(vm, 4, 4, "ModelViewMatrix gluLookAt 设置结果");

    LookAt(vm_t, eye, center, up);
    show_matr(vm_t, 4, 4, "ModelViewMatrix LookAt 计算结果");


#else    // TEST_PROJECTION

glMatrixMode(GL_PROJECTION);   glLoadIdentity();
glGetFloatv(GL_PROJECTION_MATRIX, pm);
show_matr(pm, 4, 4, "ProjectionwMatrix  此时应当为单位阵");

    
float a= 45.0, as=1.0, n=1, p=1000;    // (60, 1, 0.1, 1000.0f);
float av=a*PI/180;                     // 注意 radian、degree 转换 
float Rt[4]={-tan(av/2)*n*as,   tan(av/2)*n,   tan(av/2)*n*as,   -tan(av/2)*n}; 


// gluPerspective(a,as,n,p); 
glFrustum(-tan(av/2)*n*as, tan(av/2)*n*as,  tan(av/2)*n,   -tan(av/2)*n, n,p);

glGetFloatv(GL_PROJECTION_MATRIX, pm);
show_matr(pm, 4, 4, "gluPerspective/glFrustum 设置结果 ProjectionwMatrix");



Perspective(pm_t, a, as, n, p); 
show_matr(pm_t, 4, 4, "Perspective 计算结果 pm_t");

Frustum(pm_t, Rt, n,p); 
show_matr(pm_t, 4, 4, "Frustum 计算结果 pm_t");






gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
glGetFloatv(GL_PROJECTION_MATRIX, pm);
show_matr(pm, 4, 4, "gluPerspective + gluLookAt  设置结果  ProjectionMatrix ");


LookAtf(vm_t, eye, center, up); 
show_matr(vm_t, 4, 4, "LookAtf 计算结果 vm_t");

 
float* C=0L;  matr_x(pm_t, vm_t,C, 4,4,4);
show_matr(C, 4, 4, "pm_t * vm_t");


glLoadMatrixf(C); 
glGetFloatv(GL_PROJECTION_MATRIX, vm);
show_matr(vm, 4, 4, "glLoadMatrixf 设置结果 ProjectionMatrix");



delete[] C; 

#endif   // TEST_MODELVIEW、TEST_PROJECTION



show_matr((const float*)0L);
}
// t_glWT 




void t_unProject(){

float xo[4]={300, -5130.7, -83, 1};    // {1,2,3,0}; 
float xs[4]={4,-8,6,1}; 

show_matr(xo, 4, 1, "初始化 世界坐标");
 
ToScreen(xo,xs); 

show_matr(xs, 4, 1, "ToScreen 变换后的屏幕坐标"); 

ToWorld(xs,xo); 

show_matr(xo,4,1,"ToWorld 反变换后世界坐标");


show_matr((float*)0L); 
}












