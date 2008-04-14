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
// Note: most of these functions (all at the time of writing) are borrowed from Crystal Space

#ifndef PTMATH_H
#define PTMATH_H

#include <sys/types.h>
#include "ptvector2.h"

namespace Math
{
  static int WhichSide2D (const PtVector2& v,
                          const PtVector2& s1, const PtVector2& s2)
  {
    float k  = (s1.y - v.y)*(s2.x - s1.x);
    float k1 = (s1.x - v.x)*(s2.y - s1.y);
    if (k < k1) return -1;
    else if (k > k1) return 1;
    else return 0;
  }

  bool IsInArea (PtVector2 *poly, size_t num_poly, const PtVector2 &v)
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
}

#endif // PTMATH_H
