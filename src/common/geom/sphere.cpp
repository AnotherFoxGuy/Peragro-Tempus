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

#include "sphere.h"

#include "math3d.h"

template <typename T>
T square(const T& value)
{
  return value * value;
}

namespace Geom
{

  bool Sphere::Contains (const Vector3& vector) const
  {
    return false;
  }

  bool Sphere::TestIntersect (const Sphere& sphere) const
  {
    float sqDist = SquaredDist::PointPoint (center, sphere.center);
    return (sqDist - (square (radius + sphere.radius))) < 0;
  }

  bool Sphere::Contains (const Sphere& sphere) const
  {
    return false;
  }

  bool Sphere::TestIntersect (const Box& box) const
  {
    return false;
  }

  bool Sphere::Contains (const Box& box) const
  {
    return false;
  }

} // namespace Geom