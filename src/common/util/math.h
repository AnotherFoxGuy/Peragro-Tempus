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

#ifndef PT_MATH_H
#define PT_MATH_H

#include <cstddef>
#include "common/constants.h"
#include <wfmath/point.h>

namespace PT
{
  /**
   * Container class for math helper functions.
   */
  struct Math
  {
    /**
     * Used to get the largest value.
     * @param f1 The first value.
     * @param f2 The second value.
     * @return The largest of the two values.
     */
    inline static unsigned int MaxUI(unsigned int f1, unsigned int f2)
    {
      return f1 > f2 ? f1 : f2;
    }

    /**
     * Used to get the smallest value.
     * @param f1 The first value.
     * @param f2 The second value.
     * @return The smallest of the two values.
     */
    inline static unsigned int MinUI(unsigned int f1, unsigned int f2)
    {
      return f1 < f2 ? f1 : f2;
    }
    /**
     * Used to get the largest value.
     * @param f1 The first value.
     * @param f2 The second value.
     * @return The largest of the two values.
     */
    inline static float MaxF(float f1, float f2)
    {
      return f1 > f2 ? f1 : f2;
    }

    /**
     * Used to get the smallest value.
     * @param f1 The first value.
     * @param f2 The second value.
     * @return The smallest of the two values.
     */
    inline static float MinF(float f1, float f2)
    {
      return f1 < f2 ? f1 : f2;
    }

    /**
     * Normalizes the angle to -Pi, +Pi.
     * @param angle The angle in radians.
     * @return The angle normalized to -Pi, +Pi.
     */
    static float NormalizeAngle(float angle)
    {
      while (angle > PT_PI)
      {
        angle = angle - PT_2PI;
      }
      while (angle < -PT_PI)
      {
        angle = angle + PT_2PI;
      }
      return angle;
    }

    /**
     * Calculates which side of a line a point is on
     * @param v The point.
     * @param s1 The first end of the line.
     * @param s2 The second end of the line.
     * @return -1 if the point is left of the line, 1 if the point is right of
     *   the line, and 0 if the point is on the line.
     */
    static int WhichSide2D (const WFMath::Point<2>& v,
                            const WFMath::Point<2>& s1, const WFMath::Point<2>& s2)
    {
      float k  = (s1[1] - v[1])*(s2[0] - s1[0]);
      float k1 = (s1[0] - v[0])*(s2[1] - s1[1]);
      if (k < k1) return -1;
      else if (k > k1) return 1;
      else return 0;
    }

    /**
     * Calculates whether a point is inside a polygon.
     * @param poly An array of points defining a polygon.
     * @param num_poly The number of points in the array.
     * @param v The point.
     * @return True if the point is inside.
     */
    static bool IsInArea (WFMath::Point<2> *poly, size_t num_poly, const WFMath::Point<2>& v)
    {
      if (num_poly > 0)
      {
        size_t i1 = num_poly - 1;
        for (size_t i = 0; i < num_poly; i++)
        {
          if (WhichSide2D (v, poly[i1], poly[i]) < 0)
            return false;
          i1 = i;
        }
      }
      return true;
    }
  };

} // PT namespace

#endif // PT_MATH_H
