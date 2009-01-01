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
#include "box.h"

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

  bool Sphere::Intersect (const Sphere& sphere) const
  {
    float sqDist = SquaredDist::PointPoint (center, sphere.center);
    return (sqDist - (square (radius + sphere.radius))) < 0;
  }

  bool Sphere::Contains (const Sphere& sphere) const
  {
    return false;
  }

  bool Sphere::Intersect (const Box& box) const
  {
    double dist = 0;

    for(int i = 0; i < 3; ++i) 
    {
      double dist_i;
      if(center[i] < box.Min()[i])
        dist_i = center[i] - box.Min()[i];
      else if(center[i] > box.Max()[i])
        dist_i = center[i] - box.Max()[i];
      else
        continue;
      dist += dist_i * dist_i;
    }

    printf("Intersect: %s (%f-%f)\n", LessOrEqual(dist, radius * radius) ? "yes" : "no", dist, radius * radius);
    return LessOrEqual(dist, radius * radius);
  }

  bool Sphere::Contains (const Box& box) const
  {
    double sqr_dist = 0;

    for(size_t i = 0; i < 3; ++i) 
    {
      float furthest = FloatMax(fabs(center[i] - box.Min()[i]), fabs(center[i] - box.Max()[i]));
      sqr_dist += furthest * furthest;
    }

    return LessOrEqual(sqr_dist, radius * radius * (1 + PT_EPSILON));
  }

} // namespace Geom