#ifndef PT_VECMATH_H_
#define PT_VECMATH_H_

#include <cssysdef.h>
#include <csgeom/math3d.h>
#include <csgeom/vector4.h>
#include <csgeom/transfrm.h>
#include <csgeom/matrix3.h>

#include <cmath>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cassert>
 
template<class T> class Matrix4;

typedef Matrix4<float>	csMatrix4;

template<class T> class Matrix4
{
 public:
    Matrix4();
    Matrix4(const Matrix4<T>& m);
    Matrix4(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T);
    Matrix4(const T*);
    Matrix4(const csVector4&, const csVector4&,
            const csVector4&, const csVector4&);

	inline Matrix4& operator=(const Matrix4<T>&);
	inline Matrix4& operator=(const T*);
	inline void Set(T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T);

    inline T* operator[](int);
	inline operator T*();
	inline operator const T*() const;

    inline Matrix4 operator-(const Matrix4<T>&) const;
    inline Matrix4 operator+(const Matrix4<T>&) const;
    inline Matrix4 operator*(const Matrix4<T>&) const;

    inline csVector3 operator*(const csVector3&) const;
    inline csVector3 multiplyWithoutTranslation(const csVector3& a) const;
	inline csVector4 operator*(const csVector4&) const;

    static Matrix4<T> Identity();
    static Matrix4<T> translation(const csVector3&);

    static Matrix4<T> rotation(const csVector3&, T);
    static Matrix4<T> xrotation(T);
    static Matrix4<T> yrotation(T);
    static Matrix4<T> zrotation(T);

    Matrix4<T> GetTranspose() const;
    Matrix4<T> GetInverse() const;

    inline void transfo4d(csVector3&);

    T r[16];
};

////////////////////////// Matrix4 class methods ///////////////////////////////

template<class T> void Matrix4<T>::transfo4d(csVector3& v)
{
	v=(*this)*v;
}

template<class T> Matrix4<T>::Matrix4()
{
	r[0]=0; r[1]=0; r[2]=0; r[3]=0; r[4]=0; r[5]=0; r[6]=0; r[7]=0;
	r[8]=0; r[9]=0; r[10]=0; r[11]=0; r[12]=0; r[13]=0; r[14]=0; r[15]=0;
}

template<class T> Matrix4<T>::Matrix4(const Matrix4<T>& m)
{
	memcpy(r,m.r,sizeof(m.r));
}

template<class T> Matrix4<T>::Matrix4(const T* m)
{
	memcpy(r,m,sizeof(T)*16);
}

template<class T> Matrix4<T>::Matrix4(const csVector4& v0,
                                      const csVector4& v1,
                                      const csVector4& v2,
                                      const csVector4& v3)
{
    r[0] = v0.x;
    r[1] = v0.y;
    r[2] = v0.z;
    r[3] = v0.w;
    r[4] = v1.x;
    r[5] = v1.y;
    r[6] = v1.z;
    r[7] = v1.w;
    r[8] = v2.x;
    r[9] = v2.y;
    r[10] = v2.z;
    r[11] = v2.w;
    r[12] = v3.x;
    r[13] = v3.y;
    r[14] = v3.z;
    r[15] = v3.w;
}

template<class T> Matrix4<T>& Matrix4<T>::operator=(const Matrix4<T>& m)
{
	memcpy(r,m.r,sizeof(m.r));
	return (*this);
}

template<class T> Matrix4<T>::Matrix4(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
{
	r[0]=a; r[1]=b; r[2]=c; r[3]=d; r[4]=e; r[5]=f; r[6]=g; r[7]=h;
	r[8]=i; r[9]=j; r[10]=k; r[11]=l; r[12]=m; r[13]=n; r[14]=o; r[15]=p;
}

template<class T> void Matrix4<T>::Set(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
{
	r[0]=a; r[1]=b; r[2]=c; r[3]=d; r[4]=e; r[5]=f; r[6]=g; r[7]=h;
	r[8]=i; r[9]=j; r[10]=k; r[11]=l; r[12]=m; r[13]=n; r[14]=o; r[15]=p;
}

template<class T> T* Matrix4<T>::operator[](int n)
{
    return &(r[n*4]);
}

template<class T> Matrix4<T>::operator T*()
{
	return r;
}

template<class T> Matrix4<T>::operator const T*() const
{
	return r;
}

template<class T> Matrix4<T> Matrix4<T>::Identity()
{
    return Matrix4<T>(	1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1  );
}


template<class T> Matrix4<T> Matrix4<T>::translation(const csVector3& a)
{
    return Matrix4<T>(	1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						a.x, a.y, a.z, 1);
}

template<class T> Matrix4<T> Matrix4<T>::rotation(const csVector3& axis,
                                                  T angle)
{
    T c = (T) cos(angle);
    T s = (T) sin(angle);
    T t = 1 - c;

    return Matrix4<T>(csVector4(t * axis.x * axis.x + c,
                                 t * axis.x * axis.y - s * axis.z,
                                 t * axis.x * axis.y + s * axis.y,
                                 0),
                      csVector4(t * axis.x * axis.y + s * axis.z,
                                 t * axis.y * axis.y + c,
                                 t * axis.y * axis.z - s * axis.x,
                                 0),
                      csVector4(t * axis.x * axis.z - s * axis.y,
                                 t * axis.y * axis.z + s * axis.x,
                                 t * axis.z * axis.z + c,
                                 0),
                      csVector4(0, 0, 0, 1));
}

template<class T> Matrix4<T> Matrix4<T>::xrotation(T angle)
{
    T c = (T) cos(angle);
    T s = (T) sin(angle);

    return Matrix4<T>(1, 0, 0, 0,
                      0, c, s, 0,
                      0,-s, c, 0,
                      0, 0, 0, 1 );
}


template<class T> Matrix4<T> Matrix4<T>::yrotation(T angle)
{
    T c = (T) cos(angle);
    T s = (T) sin(angle);

    return Matrix4<T>( c, 0,-s, 0,
                       0, 1, 0, 0,
                       s, 0, c, 0,
                       0, 0, 0, 1 );
}


template<class T> Matrix4<T> Matrix4<T>::zrotation(T angle)
{
    T c = (T) cos(angle);
    T s = (T) sin(angle);

    return Matrix4<T>(c, s, 0, 0,
                     -s, c, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1 );
}

// multiply column vector by a 4x4 matrix in homogeneous coordinate (use a[3]=1)
template<class T> csVector3 Matrix4<T>::operator*(const csVector3& a) const
{
    return csVector3(	r[0]*a.x + r[4]*a.y +  r[8]*a.z + r[12],
						r[1]*a.x + r[5]*a.y +  r[9]*a.z + r[13],
						r[2]*a.x + r[6]*a.y + r[10]*a.z + r[14] );
}

template<class T> csVector3 Matrix4<T>::multiplyWithoutTranslation(const csVector3& a) const
{
    return csVector3(	r[0]*a.x + r[4]*a.y +  r[8]*a.z,
						r[1]*a.x + r[5]*a.y +  r[9]*a.z,
						r[2]*a.x + r[6]*a.y + r[10]*a.z );
}

// multiply column vector by a 4x4 matrix in homogeneous coordinate (considere a[3]=1)
template<class T> csVector4 Matrix4<T>::operator*(const csVector4& a) const
{
    return csVector4(	r[0]*a.x + r[4]*a.y +  r[8]*a.z + r[12]*a.w,
						r[1]*a.x + r[5]*a.y +  r[9]*a.z + r[13]*a.w,
						r[2]*a.x + r[6]*a.y + r[10]*a.z + r[14]*a.w );
}

template<class T> Matrix4<T> Matrix4<T>::GetTranspose() const
{
    return Matrix4<T>(	r[0], r[4], r[8],  r[12],
						r[1], r[5], r[9],  r[13],
						r[2], r[6], r[10], r[14],
						r[3], r[7], r[11], r[15]);
}

template<class T> Matrix4<T> Matrix4<T>::operator*(const Matrix4<T>& a) const
{
#define MATMUL(R, C) (r[R] * a.r[C] + r[R+4] * a.r[C+1] + r[R+8] * a.r[C+2] + r[R+12] * a.r[C+3])
    return Matrix4<T>(	MATMUL(0,0), MATMUL(1,0), MATMUL(2,0), MATMUL(3,0),
						MATMUL(0,4), MATMUL(1,4), MATMUL(2,4), MATMUL(3,4),
						MATMUL(0,8), MATMUL(1,8), MATMUL(2,8), MATMUL(3,8),
						MATMUL(0,12), MATMUL(1,12), MATMUL(2,12), MATMUL(3,12) );
#undef MATMUL
}


template<class T> Matrix4<T> Matrix4<T>::operator+(const Matrix4<T>& a) const
{
    return Matrix4<T>(	r[0]+a.r[0], r[1]+a.r[1], r[2]+a.r[2], r[3]+a.r[3],
						r[4]+a.r[4], r[5]+a.r[5], r[6]+a.r[6], r[7]+a.r[7],
						r[8]+a.r[8], r[9]+a.r[9], r[10]+a.r[10], r[11]+a.r[11],
						r[12]+a.r[12], r[13]+a.r[13], r[14]+a.r[14], r[15]+a.r[15] );
}

template<class T> Matrix4<T> Matrix4<T>::operator-(const Matrix4<T>& a) const
{
    return Matrix4<T>(	r[0]-a.r[0], r[1]-a.r[1], r[2]-a.r[2], r[3]-a.r[3],
						r[4]-a.r[4], r[5]-a.r[5], r[6]-a.r[6], r[7]-a.r[7],
						r[8]-a.r[8], r[9]-a.r[9], r[10]-a.r[10], r[11]-a.r[11],
						r[12]-a.r[12], r[13]-a.r[13], r[14]-a.r[14], r[15]-a.r[15] );
}

/*
 * Code ripped from the GLU library
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy jle@star.be
 * Return zero matrix on failure (singular matrix)
 */
template<class T> Matrix4<T> Matrix4<T>::GetInverse() const
{
	const T * m = r;
	T out[16];

/* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { T *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

   T wtmp[4][8];
   T m0, m1, m2, m3, s;
   T *r0, *r1, *r2, *r3;

   r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

   r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
      r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
      r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
      r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
      r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
      r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
      r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
      r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
      r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
      r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
      r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
      r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

   /* choose pivot - or die */
   if (fabs(r3[0]) > fabs(r2[0]))
      SWAP_ROWS(r3, r2);
   if (fabs(r2[0]) > fabs(r1[0]))
      SWAP_ROWS(r2, r1);
   if (fabs(r1[0]) > fabs(r0[0]))
      SWAP_ROWS(r1, r0);
   if (0.0 == r0[0])
      return Matrix4<T>();

   /* eliminate first variable     */
   m1 = r1[0] / r0[0];
   m2 = r2[0] / r0[0];
   m3 = r3[0] / r0[0];
   s = r0[1];
   r1[1] -= m1 * s;
   r2[1] -= m2 * s;
   r3[1] -= m3 * s;
   s = r0[2];
   r1[2] -= m1 * s;
   r2[2] -= m2 * s;
   r3[2] -= m3 * s;
   s = r0[3];
   r1[3] -= m1 * s;
   r2[3] -= m2 * s;
   r3[3] -= m3 * s;
   s = r0[4];
   if (s != 0.0) {
      r1[4] -= m1 * s;
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
   }
   s = r0[5];
   if (s != 0.0) {
      r1[5] -= m1 * s;
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
   }
   s = r0[6];
   if (s != 0.0) {
      r1[6] -= m1 * s;
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
   }
   s = r0[7];
   if (s != 0.0) {
      r1[7] -= m1 * s;
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
   }

   /* choose pivot - or die */
   if (fabs(r3[1]) > fabs(r2[1]))
      SWAP_ROWS(r3, r2);
   if (fabs(r2[1]) > fabs(r1[1]))
      SWAP_ROWS(r2, r1);
   if (0.0 == r1[1])
      return Matrix4<T>();

   /* eliminate second variable */
   m2 = r2[1] / r1[1];
   m3 = r3[1] / r1[1];
   r2[2] -= m2 * r1[2];
   r3[2] -= m3 * r1[2];
   r2[3] -= m2 * r1[3];
   r3[3] -= m3 * r1[3];
   s = r1[4];
   if (0.0 != s) {
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
   }
   s = r1[5];
   if (0.0 != s) {
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
   }
   s = r1[6];
   if (0.0 != s) {
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
   }
   s = r1[7];
   if (0.0 != s) {
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
   }

   /* choose pivot - or die */
   if (fabs(r3[2]) > fabs(r2[2]))
      SWAP_ROWS(r3, r2);
   if (0.0 == r2[2])
      return Matrix4<T>();

   /* eliminate third variable */
   m3 = r3[2] / r2[2];
   r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
      r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

   /* last check */
   if (0.0 == r3[3])
      return Matrix4<T>();

   s = 1.0 / r3[3];		/* now back substitute row 3 */
   r3[4] *= s;
   r3[5] *= s;
   r3[6] *= s;
   r3[7] *= s;

   m2 = r2[3];			/* now back substitute row 2 */
   s = 1.0 / r2[2];
   r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
      r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
   m1 = r1[3];
   r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
      r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
   m0 = r0[3];
   r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
      r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

   m1 = r1[2];			/* now back substitute row 1 */
   s = 1.0 / r1[1];
   r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
      r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
   m0 = r0[2];
   r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
      r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

   m0 = r0[1];			/* now back substitute row 0 */
   s = 1.0 / r0[0];
   r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
      r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

   MAT(out, 0, 0) = r0[4];
   MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
   MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
   MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
   MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
   MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
   MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
   MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
   MAT(out, 3, 3) = r3[7];

   return Matrix4<T>(out);

#undef MAT
#undef SWAP_ROWS
}


#endif