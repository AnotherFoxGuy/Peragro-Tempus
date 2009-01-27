// vector_funcs.h (Vector<> template functions)
//
//  The WorldForge Project
//  Copyright (C) 2001  The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-7

// Extensive amounts of this material come from the Vector2D
// and Vector3D classes from stage/math, written by Bryce W.
// Harrington, Kosh, and Jari Sundell (Rakshasa).

#ifndef WFMATH_VECTOR_FUNCS_H
#define WFMATH_VECTOR_FUNCS_H

#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>
#include <wfmath/const.h>

#include <cmath>

#include <cassert>

namespace WFMath {

template<const int dim>
Vector<dim>::Vector(const Vector<dim>& v) : m_valid(v.m_valid)
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = v.m_elem[i];
  }
}

template<const int dim>
Vector<dim>& Vector<dim>::operator=(const Vector<dim>& v)
{
  m_valid = v.m_valid;

  for(int i = 0; i < dim; ++i) {
    m_elem[i] = v.m_elem[i];
  }

  return *this;
}

template<const int dim>
bool Vector<dim>::isEqualTo(const Vector<dim>& v, double epsilon) const
{
  double delta = _ScaleEpsilon(m_elem, v.m_elem, dim, epsilon);

  for(int i = 0; i < dim; ++i) {
    if(fabs(m_elem[i] - v.m_elem[i]) > delta) {
      return false;
    }
  }

  return true;
}

template <const int dim>
Vector<dim>& operator+=(Vector<dim>& v1, const Vector<dim>& v2)
{
  v1.m_valid = v1.m_valid && v2.m_valid;

  for(int i = 0; i < dim; ++i) {
    v1.m_elem[i] += v2.m_elem[i];
  }

  return v1;
}

template <const int dim>
Vector<dim>& operator-=(Vector<dim>& v1, const Vector<dim>& v2)
{
  v1.m_valid = v1.m_valid && v2.m_valid;

  for(int i = 0; i < dim; ++i) {
    v1.m_elem[i] -= v2.m_elem[i];
  }

  return v1;
}

template <const int dim>
Vector<dim>& operator*=(Vector<dim>& v, CoordType d)
{
  for(int i = 0; i < dim; ++i) {
    v.m_elem[i] *= d;
  }

  return v;
}

template <const int dim>
Vector<dim>& operator/=(Vector<dim>& v, CoordType d)
{
  for(int i = 0; i < dim; ++i) {
    v.m_elem[i] /= d;
  }

  return v;
}

template <const int dim>
Vector<dim> operator+(const Vector<dim>& v1, const Vector<dim>& v2)
{
  Vector<dim> ans(v1);

  ans += v2;

  return ans;
}

template <const int dim>
Vector<dim> operator-(const Vector<dim>& v1, const Vector<dim>& v2)
{
  Vector<dim> ans(v1);

  ans -= v2;

  return ans;
}

template <const int dim>
Vector<dim> operator*(const Vector<dim>& v, CoordType d)
{
  Vector<dim> ans(v);

  ans *= d;

  return ans;
}

template<const int dim>
Vector<dim> operator*(CoordType d, const Vector<dim>& v)
{
  Vector<dim> ans(v);

  ans *= d;

  return ans;
}

template <const int dim>
Vector<dim> operator/(const Vector<dim>& v, CoordType d)
{
  Vector<dim> ans(v);

  ans /= d;

  return ans;
}

template <const int dim>
Vector<dim> operator-(const Vector<dim>& v)
{
  Vector<dim> ans;

  ans.m_valid = v.m_valid;

  for(int i = 0; i < dim; ++i) {
    ans.m_elem[i] = -v.m_elem[i];
  }

  return ans;
}

template<const int dim>
Vector<dim>& Vector<dim>::sloppyNorm(CoordType norm)
{
  CoordType mag = sloppyMag();

  assert("need nonzero length vector" && mag > norm / WFMATH_MAX);

  return (*this *= norm / mag);
}

template<const int dim>
Vector<dim>& Vector<dim>::zero()
{
  m_valid = true;

  for(int i = 0; i < dim; ++i) {
    m_elem[i] = 0;
  }

  return *this;
}

template<const int dim>
CoordType Angle(const Vector<dim>& v, const Vector<dim>& u)
{
  // Adding numbers with large magnitude differences can cause
  // a loss of precision, but Dot() checks for this now

  CoordType dp = FloatClamp(Dot(u, v) / sqrt(u.sqrMag() * v.sqrMag()),
			 -1.0, 1.0);

  CoordType angle = acos(dp);
 
  return angle;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(int axis1, int axis2, CoordType theta)
{
  assert(axis1 >= 0 && axis2 >= 0 && axis1 < dim && axis2 < dim && axis1 != axis2);

  CoordType tmp1 = m_elem[axis1], tmp2 = m_elem[axis2];
  CoordType stheta = (CoordType) sin(theta), ctheta = (CoordType) cos(theta);

  m_elem[axis1] = tmp1 * ctheta - tmp2 * stheta;
  m_elem[axis2] = tmp2 * ctheta + tmp1 * stheta;

  return *this;
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(const Vector<dim>& v1, const Vector<dim>& v2,
				 CoordType theta)
{
  RotMatrix<dim> m;
  return operator=(Prod(*this, m.rotation(v1, v2, theta)));
}

template<const int dim>
Vector<dim>& Vector<dim>::rotate(const RotMatrix<dim>& m)
{
  return *this = Prod(*this, m);
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<> Vector<3>& Vector<3>::rotate(const Vector<3>& axis, CoordType theta);
template<> Vector<3>& Vector<3>::rotate(const Quaternion& q);
#else
Vector<3>& _NCFS_Vector3_rotate(Vector<3>& v, const Vector<3>& axis, CoordType theta);
Vector<3>& _NCFS_Vector3_rotate(Vector<3>& v, const Quaternion& q);

template<>
Vector<3>& Vector<3>::rotate(const Vector<3>& axis, CoordType theta)
{
  return _NCFS_Vector3_rotate(*this, axis, theta);
}

template<>
Vector<3>& Vector<3>::rotate(const Quaternion& q)
{
  return _NCFS_Vector3_rotate(*this, q);
}
#endif

template<const int dim>
CoordType Dot(const Vector<dim>& v1, const Vector<dim>& v2)
{
  double delta = _ScaleEpsilon(v1.m_elem, v2.m_elem, dim);

  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    ans += v1.m_elem[i] * v2.m_elem[i];
  }

  return (fabs(ans) >= delta) ? ans : 0;
}

template<const int dim>
CoordType Vector<dim>::sqrMag() const
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    // all terms > 0, no loss of precision through cancelation
    ans += m_elem[i] * m_elem[i];
  }

  return ans;
}

template<const int dim>
bool Parallel(const Vector<dim>& v1, const Vector<dim>& v2, bool& same_dir)
{
  CoordType dot = Dot(v1, v2);

  same_dir = (dot > 0);

  return Equal(dot * dot, v1.sqrMag() * v2.sqrMag());
}

template<const int dim>
bool Parallel(const Vector<dim>& v1, const Vector<dim>& v2)
{
  bool same_dir;

  return Parallel(v1, v2, same_dir);
}

template<const int dim>
bool Perpendicular(const Vector<dim>& v1, const Vector<dim>& v2)
{
  double max1 = 0, max2 = 0;

  for(int i = 0; i < dim; ++i) {
    double val1 = fabs(v1[i]), val2 = fabs(v2[i]);
    if(val1 > max1) {
      max1 = val1;
    }
    if(val2 > max2) {
      max2 = val2;
    }
  }

  // Need to scale by both, since Dot(v1, v2) goes like the product of the magnitudes
  int exp1, exp2;
  (void) frexp(max1, &exp1);
  (void) frexp(max2, &exp2);

  return fabs(Dot(v1, v2)) < ldexp(WFMATH_EPSILON, exp1 + exp2);
}

template<>
const CoordType Vector<1>::sloppyMagMax()
{
  return (CoordType) 1;
}

template<>
const CoordType Vector<2>::sloppyMagMax()
{
  return (CoordType) 1.082392200292393968799446410733;
}

template<>
const CoordType Vector<3>::sloppyMagMax()
{
  return (CoordType) 1.145934719303161490541433900265;
}

template<>
const CoordType Vector<1>::sloppyMagMaxSqrt()
{
  return (CoordType) 1;
}

template<>
const CoordType Vector<2>::sloppyMagMaxSqrt()
{
  return (CoordType) 1.040380795811030899095785063701;
}

template<>
const CoordType Vector<3>::sloppyMagMaxSqrt()
{
  return (CoordType) 1.070483404496847625250328653179;
}

// Note for people trying to compute the above numbers
// more accurately:

// The worst value for dim == 2 occurs when the ratio of the components
// of the vector is sqrt(2) - 1. The value above is equal to sqrt(4 - 2 * sqrt(2)).

// The worst value for dim == 3 occurs when the two smaller components
// are equal, and their ratio with the large component is the
// (unique, real) solution to the equation q x^3 + (q-1) x + p == 0,
// where p = sqrt(2) - 1, and q = sqrt(3) + 1 - 2 * sqrt(2).
// Running the script bc_sloppy_mag_3 provided with the WFMath source
// will calculate the above number.

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<> Vector<2>& Vector<2>::polar(CoordType r, CoordType theta);
template<> void Vector<2>::asPolar(CoordType& r, CoordType& theta) const;

template<> Vector<3>& Vector<3>::polar(CoordType r, CoordType theta,
				       CoordType z);
template<> void Vector<3>::asPolar(CoordType& r, CoordType& theta,
				   CoordType& z) const;
template<> Vector<3>& Vector<3>::spherical(CoordType r, CoordType theta,
					   CoordType phi);
template<> void Vector<3>::asSpherical(CoordType& r, CoordType& theta,
				       CoordType& phi) const;

template<> CoordType Vector<2>::sloppyMag() const;
template<> CoordType Vector<3>::sloppyMag() const;
#else
void _NCFS_Vector2_polar(CoordType *m_elem, CoordType r, CoordType theta);
void _NCFS_Vector2_asPolar(CoordType *m_elem, CoordType& r, CoordType& theta);

void _NCFS_Vector3_polar(CoordType *m_elem, CoordType r, CoordType theta,
			 CoordType z);
void _NCFS_Vector3_asPolar(CoordType *m_elem, CoordType& r, CoordType& theta,
			   CoordType& z);
void _NCFS_Vector3_spherical(CoordType *m_elem, CoordType r, CoordType theta,
			     CoordType phi);
void _NCFS_Vector3_asSpherical(CoordType *m_elem, CoordType& r, CoordType& theta,
			       CoordType& phi);

CoordType _NCFS_Vector2_sloppyMag(CoordType *m_elem);
CoordType _NCFS_Vector3_sloppyMag(CoordType *m_elem);

template<>
Vector<2>& Vector<2>::polar(CoordType r, CoordType theta)
{
  _NCFS_Vector2_polar((CoordType*) m_elem, r, theta);
  m_valid = true;
  return *this;
}

template<>
void Vector<2>::asPolar(CoordType& r, CoordType& theta) const
{
  _NCFS_Vector2_asPolar((CoordType*) m_elem, r, theta);
}

template<>
Vector<3>& Vector<3>::polar(CoordType r, CoordType theta, CoordType z)
{
  _NCFS_Vector3_polar((CoordType*) m_elem, r, theta, z);
  m_valid = true;
  return *this;
}

template<>
void Vector<3>::asPolar(CoordType& r, CoordType& theta, CoordType& z) const
{
  _NCFS_Vector3_asPolar((CoordType*) m_elem, r, theta, z);
}

template<>
Vector<3>& Vector<3>::spherical(CoordType r, CoordType theta, CoordType phi)
{
  _NCFS_Vector3_spherical((CoordType*) m_elem, r, theta, phi);
  m_valid = true;
  return *this;
}

template<>
void Vector<3>::asSpherical(CoordType& r, CoordType& theta, CoordType& phi) const
{
  _NCFS_Vector3_asSpherical((CoordType*) m_elem, r, theta, phi);
}

template<>
CoordType Vector<2>::sloppyMag() const
{
  return _NCFS_Vector2_sloppyMag((CoordType*) m_elem);
}

template<>
CoordType Vector<3>::sloppyMag() const
{
  return _NCFS_Vector3_sloppyMag((CoordType*) m_elem);
}
#endif

template<> CoordType Vector<1>::sloppyMag() const
	{return (CoordType) fabs(m_elem[0]);}

template<> Vector<2>::Vector(CoordType x, CoordType y) : m_valid(true)
	{m_elem[0] = x; m_elem[1] = y;}
template<> Vector<3>::Vector(CoordType x, CoordType y, CoordType z) : m_valid(true)
	{m_elem[0] = x; m_elem[1] = y; m_elem[2] = z;}

// Don't need asserts here, they're taken care of in the general function

template<> Vector<2>& Vector<2>::rotate(CoordType theta)
	{return rotate(0, 1, theta);}

template<> Vector<3>& Vector<3>::rotateX(CoordType theta)
	{return rotate(1, 2, theta);}
template<> Vector<3>& Vector<3>::rotateY(CoordType theta)
	{return rotate(2, 0, theta);}
template<> Vector<3>& Vector<3>::rotateZ(CoordType theta)
	{return rotate(0, 1, theta);}


} // namespace WFMath

#endif // WFMATH_VECTOR_FUNCS_H
