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

#ifndef PT_SPHERE_H
#define PT_SPHERE_H

#include "ptvector3.h"

class PtBox;

/**
 * This class represents a sphere.
 */
class PtSphere
{
private:
  PtVector3 center;
  float radius;

public:
  /// Create a new empty sphere at (0,0,0).
  PtSphere () : center (0, 0, 0), radius(0)
  {}

  /// Create a new sphere.
  PtSphere (const PtVector3& center, float radius)
  {
    PtSphere::center = center;
    PtSphere::radius = radius;
  }

  /// Copy Constructor.
  PtSphere (const PtSphere& s) { center = s.center; radius = s.radius; }

  /// Get the center of this sphere.
  inline const PtVector3& GetCenter () const { return center; }
  /// Set the center of this sphere.
  inline void SetCenter (const PtVector3& c) { center = c; }
  /// Get the radius of this sphere.
  inline float GetRadius () const { return radius; }
  /// Set the radius of this sphere.
  inline void SetRadius (float r) { radius = r; }


  bool Contains (const PtVector3& vector) const;
  
  /// Test if the two spheres have an intersection.
  bool TestIntersect (const PtSphere& sphere) const;

  bool Contains (const PtSphere& sphere) const;

  bool TestIntersect (const PtBox& box) const;

  bool Contains (const PtBox& box) const;
};


#endif // PT_SPHERE_H

