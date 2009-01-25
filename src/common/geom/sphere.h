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

#ifndef GEOM_SPHERE_H
#define GEOM_SPHERE_H

#include "vector3.h"

namespace Geom
{
  class Box;

  /**
   * This class represents a sphere.
   */
  class Sphere
  {
  private:
    Vector3 center;
    float radius;

  public:
    /// Create a new empty sphere at (0,0,0).
    Sphere () : center (0, 0, 0), radius(0)
    {}

    /// Create a new sphere.
    Sphere (const Vector3& center, float radius)
    {
      Sphere::center = center;
      Sphere::radius = radius;
    }

    /// Copy Constructor.
    Sphere (const Sphere& s) { center = s.center; radius = s.radius; }

    /// Get the center of this sphere.
    inline const Vector3& GetCenter () const { return center; }
    /// Set the center of this sphere.
    inline void SetCenter (const Vector3& c) { center = c; }
    /// Get the radius of this sphere.
    inline float GetRadius () const { return radius; }
    /// Set the radius of this sphere.
    inline void SetRadius (float r) { radius = r; }


    bool Contains (const Vector3& vector) const;

    bool Intersect (const Vector3& sphere) const;

    /// Test if the two spheres have an intersection.
    bool Intersect (const Sphere& sphere) const;

    bool Contains (const Sphere& sphere) const;

    bool Intersect (const Box& box) const;

    bool Contains (const Box& box) const;
  };

} // namespace Geom

#endif // GEOM_SPHERE_H

