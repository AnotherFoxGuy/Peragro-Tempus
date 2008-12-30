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

#include "vector3.h"


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
