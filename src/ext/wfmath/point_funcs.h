// point_funcs.h (point class copied from libCoal, subsequently modified)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001, 2002  The WorldForge Project
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
//

// Author: Ron Steinke


#ifndef WFMATH_POINT_FUNCS_H
#define WFMATH_POINT_FUNCS_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>

#include <cassert>

namespace WFMath {

template<const int dim>
inline Point<dim>::Point(const Point<dim>& p) : m_valid(p.m_valid)
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = p.m_elem[i];
  }
}

template<const int dim>
inline Point<dim>& Point<dim>::setToOrigin()
{
  for(int i = 0; i < dim; ++i) {
    m_elem[i] = 0;
  }

  m_valid = true;

  return *this;
}

template<const int dim>
inline bool Point<dim>::isEqualTo(const Point<dim> &p, double epsilon) const
{
  CoordType delta = (CoordType) _ScaleEpsilon(m_elem, p.m_elem, dim, epsilon);

  for(int i = 0; i < dim; ++i) {
    if(fabs(m_elem[i] - p.m_elem[i]) > delta) {
      return false;
    }
  }

  return true;
}

template<const int dim>
inline Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2)
{
  Vector<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = c1.m_elem[i] - c2.m_elem[i];
  }

  out.m_valid = c1.m_valid && c2.m_valid;

  return out;
}

template<const int dim>
inline Point<dim>& operator+=(Point<dim>& p, const Vector<dim> &rhs)
{
    for(int i = 0; i < dim; ++i) {
      p.m_elem[i] += rhs.m_elem[i];
    }

    p.m_valid = p.m_valid && rhs.m_valid;

    return p;
}

template<const int dim>
inline Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v)
{
  Point<dim> out(c);

  out += v;

  return out;
}

template<const int dim>
inline Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c)
{
  Point<dim> out(c);

  out += v;

  return out;
}

template<const int dim>
inline Point<dim>& operator-=(Point<dim>& p, const Vector<dim> &rhs)
{
    for(int i = 0; i < dim; ++i) {
      p.m_elem[i] -= rhs.m_elem[i];
    }

    p.m_valid = p.m_valid && rhs.m_valid;

    return p;
}

template<const int dim>
inline Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v)
{
  Point<dim> out(c);

  out -= v;

  return out;
}

template<const int dim>
inline Point<dim> operator*(const Point<dim>& c1, const Point<dim>& c2)
{
  Point<dim> out;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = c1.m_elem[i] * c2.m_elem[i];
  }

  out.m_valid = c1.m_valid && c2.m_valid;

  return out;
}

template<const int dim>
inline Point<dim>& Point<dim>::operator=(const Point<dim>& rhs)
{
    // Compare pointer addresses
    if (this == &rhs) {
      return *this;
    }

    for(int i = 0; i < dim; ++i) {
      m_elem[i] = rhs.m_elem[i];
    }

    m_valid = rhs.m_valid;

    return *this;
}

template<const int dim>
inline CoordType SquaredDistance(const Point<dim>& p1, const Point<dim>& p2)
{
  CoordType ans = 0;

  for(int i = 0; i < dim; ++i) {
    CoordType diff = p1.m_elem[i] - p2.m_elem[i];
    ans += diff * diff;
  }

  return (fabs(ans) >= _ScaleEpsilon(p1.m_elem, p2.m_elem, dim)) ? ans : 0;
}

#ifndef WFMATH_NO_TEMPLATES_AS_TEMPLATE_PARAMETERS
template<const int dim, template<class, class> class container,
			template<class, class> class container2>
Point<dim> Barycenter(const container<Point<dim>, std::allocator<Point<dim> > >& c,
		      const container2<CoordType, std::allocator<CoordType> >& weights)
{
  // FIXME become friend

  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator c_i = c.begin(), c_end = c.end();
  typename container2<CoordType, std::allocator<CoordType> >::const_iterator w_i = weights.begin(),
						 w_end = weights.end();

  assert("nonempty list of points" && c_i != c_end);
  assert("nonempty list of weights" && w_i != w_end);

  bool valid = c_i->isValid();

  CoordType tot_weight = *w_i, max_weight = fabs(*w_i);
  Point<dim> out;
  for(int j = 0; j < dim; ++j) {
    out[j] = (*c_i)[j] * *w_i;
  }

  while(++c_i != c_end && ++w_i != w_end) {
    tot_weight += *w_i;
    CoordType val = fabs(*w_i);
    if(val > max_weight)
      max_weight = val;
    if(!c_i->isValid())
      valid = false;
    for(int j = 0; j < dim; ++j)
      out[j] += (*c_i)[j] * *w_i;
  }

  // Make sure the weights don't add up to zero
  assert("sum of weights must be nonzero" && max_weight > 0
         && fabs(tot_weight) > max_weight * WFMATH_EPSILON);

  for(int j = 0; j < dim; ++j) {
    out[j] /= tot_weight;
  }

  out.setValid(valid);

  return out;
}

template<const int dim, template<class, class> class container>
Point<dim> Barycenter(const container<Point<dim>, std::allocator<Point<dim> > >& c)
{
  // FIXME become friend

  typename container<Point<dim>, std::allocator<Point<dim> > >::const_iterator i = c.begin(), end = c.end();

  assert("nonempty list of points" && i != end);

  Point<dim> out = *i;
  int num_points = 1;

  bool valid = i->isValid();

  while(++i != end) {
    ++num_points;
    if(!i->isValid())
      valid = false;
    for(int j = 0; j < dim; ++j)
      out[j] += (*i)[j];
  }

  for(int j = 0; j < dim; ++j) {
    out[j] /= num_points;
  }

  out.setValid(valid);

  return out;
}
#endif

template<const int dim>
inline Point<dim> Midpoint(const Point<dim>& p1, const Point<dim>& p2, CoordType dist)
{
  Point<dim> out;
  CoordType conj_dist = 1 - dist;

  for(int i = 0; i < dim; ++i) {
    out.m_elem[i] = p1.m_elem[i] * conj_dist + p2.m_elem[i] * dist;
  }

  out.m_valid = p1.m_valid && p2.m_valid;

  return out;
}

template<> Point<2>::Point(CoordType x, CoordType y) : m_valid(true)
{
  m_elem[0] = x;
  m_elem[1] = y;
}

template<> Point<3>::Point(CoordType x, CoordType y, CoordType z) : m_valid(true)
{
  m_elem[0] = x;
  m_elem[1] = y;
  m_elem[2] = z;
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<> Point<2>& Point<2>::polar(CoordType r, CoordType theta);
template<> void Point<2>::asPolar(CoordType& r, CoordType& theta) const;

template<> Point<3>& Point<3>::polar(CoordType r, CoordType theta,
				     CoordType z);
template<> void Point<3>::asPolar(CoordType& r, CoordType& theta,
				  CoordType& z) const;
template<> Point<3>& Point<3>::spherical(CoordType r, CoordType theta,
					 CoordType phi);
template<> void Point<3>::asSpherical(CoordType& r, CoordType& theta,
				      CoordType& phi) const;
#else
void _NCFS_Point2_polar(CoordType *m_elem, CoordType r, CoordType theta);
void _NCFS_Point2_asPolar(CoordType *m_elem, CoordType& r, CoordType& theta);

void _NCFS_Point3_polar(CoordType *m_elem, CoordType r, CoordType theta,
			CoordType z);
void _NCFS_Point3_asPolar(CoordType *m_elem, CoordType& r, CoordType& theta,
			  CoordType& z);
void _NCFS_Point3_spherical(CoordType *m_elem, CoordType r, CoordType theta,
			    CoordType phi);
void _NCFS_Point3_asSpherical(CoordType *m_elem, CoordType& r, CoordType& theta,
			      CoordType& phi);

template<>
inline Point<2>& Point<2>::polar(CoordType r, CoordType theta)
{
  _NCFS_Point2_polar((CoordType*) m_elem, r, theta);
  m_valid = true;
  return *this;
}

template<>
inline void Point<2>::asPolar(CoordType& r, CoordType& theta) const
{
  _NCFS_Point2_asPolar((CoordType*) m_elem, r, theta);
}

template<>
inline Point<3>& Point<3>::polar(CoordType r, CoordType theta, CoordType z)
{
  _NCFS_Point3_polar((CoordType*) m_elem, r, theta, z);
  m_valid = true;
  return *this;
}

template<>
inline void Point<3>::asPolar(CoordType& r, CoordType& theta, CoordType& z) const
{
  _NCFS_Point3_asPolar((CoordType*) m_elem, r, theta, z);
}

template<>
inline Point<3>& Point<3>::spherical(CoordType r, CoordType theta, CoordType phi)
{
  _NCFS_Point3_spherical((CoordType*) m_elem, r, theta, phi);
  m_valid = true;
  return *this;
}

template<>
inline void Point<3>::asSpherical(CoordType& r, CoordType& theta, CoordType& phi) const
{
  _NCFS_Point3_asSpherical((CoordType*) m_elem, r, theta, phi);
}
#endif

} // namespace WFMath

#endif  // WFMATH_POINT_FUNCS_H
