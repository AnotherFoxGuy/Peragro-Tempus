/*
    Copyright (C) 2008 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GEOM_MATH3D_H
#define GEOM_MATH3D_H

#include "common/constants.h"

#include "vector3.h"

bool Equal(double x1, double x2, double epsilon = PT_EPSILON)
{
  using namespace std;
  int exponent;
  (void) frexp(fabs(x1) < fabs(x2) ? x1 : x2, &exponent);
  return fabs(x1 - x2) <= ldexp(epsilon, exponent);
}
// Avoid template, make sure floats are cast to doubles.
inline bool Equal(float x1, float x2, double epsilon = PT_EPSILON)
	{return Equal((double) x1, (double) x2, epsilon);}

bool LessOrEqual(double x1, double x2)
{
  return x1 < x2 || Equal(x1, x2);
}

// These let us avoid including <algorithm> for the sake of
// std::max() and std::min().

inline float FloatMax(float a, float b)
	{return (a > b) ? a : b;}
inline float FloatMin(float a, float b)
	{return (a < b) ? a : b;}
inline float FloatClamp(float val, float min, float max)
	{return (min >= val) ? min : (max <= val ? max : val);}

/**
 * Some functions to perform squared distance calculations.
 * This is a static class and contains only static member functions.
 */
struct SquaredDist
{
  /// Returns the squared distance between two points.
  static float PointPoint (const Geom::Vector3& p1, const Geom::Vector3& p2)
  {
    Geom::Vector3 d = (p1-p2);
    return d*d;
  }
};

#endif // __CS_MATH3D_H__

