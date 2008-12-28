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

#ifndef GEOM_BOX_H
#define GEOM_BOX_H

#include "vector3.h"

namespace Geom
{
  class Sphere;

  /**
   * The maximum value that a coordinate in the bounding box can use.
   * This is considered the 'infinity' value used for empty bounding boxes.
   */
#define BOUNDINGBOX_MAXVALUE 1000000000.


  /**
   * \name Indices of corner vertices for Box.
   * Used by Box::GetCorner().
   */
  enum
  {
    /// min X, min Y, min Z
    BOX_CORNER_xyz = 0,
    /// min X, min Y, max Z
    BOX_CORNER_xyZ = 1,
    /// min X, max Y, min Z
    BOX_CORNER_xYz = 2,
    /// min X, max Y, max Z
    BOX_CORNER_xYZ = 3,
    /// min X, min Y, min Z
    BOX_CORNER_Xyz = 4,
    /// max X, min Y, max Z
    BOX_CORNER_XyZ = 5,
    /// max X, max Y, min Z
    BOX_CORNER_XYz = 6,
    /// max X, max Y, max Z
    BOX_CORNER_XYZ = 7,
    /// center
    BOX_CENTER3 = 8
  };

  /**
   * A bounding box in 3D space.
   * In order to operate correctly, this bounding box assumes that all values
   * entered or compared against lie within the range
   * (-#BOUNDINGBOX_MAXVALUE, #BOUNDINGBOX_MAXVALUE).  It is not
   * recommended to use points outside of this range.
   */
  class Box
  {
  protected:
    /// The top-left of this bounding box.
    Vector3 minbox;
    /// The bottom-right.
    Vector3 maxbox;

  public:
    /// Get the 3d vector of minimum (x, y, z) values
    inline const Vector3& Min () const { return minbox; }
    /// Get the 3d vector of maximum (x, y, z) values
    inline const Vector3& Max () const { return maxbox; }

    /// Get the minimum X value of the box
    inline float MinX () const { return minbox.x; }
    /// Get the minimum Y value of the box
    inline float MinY () const { return minbox.y; }
    /// Get the minimum Z value of the box
    inline float MinZ () const { return minbox.z; }
    /// Get the maximum X value of the box
    inline float MaxX () const { return maxbox.x; }
    /// Get the maximum Y value of the box
    inline float MaxY () const { return maxbox.y; }
    /// Get the maximum Z value of the box
    inline float MaxZ () const { return maxbox.z; }

    /// Compute volume of box
    inline float Volume () const
    { return (MaxX()-MinX())*(MaxY()-MinY())*(MaxZ()-MinZ()); }
    /// Compute area of box
    inline float Area () const
    {
      float x = MaxX()-MinX();
      float y = MaxY()-MinY();
      float z = MaxZ()-MinZ();
      return 2.0f*(x*y + x*z + y*z);
    }

    /**
     * Return every corner of this bounding box from 0
     * to 7. This contrasts with Min() and Max() because
     * those are only the min and max corners.
     * Corner 0 = xyz, 1 = xyZ, 2 = xYz, 3 = xYZ,
     *        4 = Xyz, 5 = XyZ, 6 = XYz, 7 = XYZ.
     * Use #BOX_CORNER_xyz etc. defines.
     * #BOX_CENTER3 also works.
     */
    Vector3 GetCorner (int corner) const;

    /// Get the center of this box.
    inline Vector3 GetCenter () const { return (minbox+maxbox)/2; }

    /**
     * Set the center of this box. This will not change the size
     * of the box but just relocate the center.
     */
    void SetCenter (const Vector3& c);

    /// Set the size of the box but keep the center intact.
    void SetSize (const Vector3& s);

    /// Get the size of the box 
    inline Vector3 GetSize () const { return (maxbox-minbox); }

    /// Test if this box is empty.
    inline bool Empty () const
    {
      if (minbox.x > maxbox.x) return true;
      if (minbox.y > maxbox.y) return true;
      if (minbox.z > maxbox.z) return true;
      return false;
    }

    /// Initialize this box to empty.
    inline void StartBoundingBox ()
    {
      minbox.x =  BOUNDINGBOX_MAXVALUE;
      minbox.y =  BOUNDINGBOX_MAXVALUE;
      minbox.z =  BOUNDINGBOX_MAXVALUE;
      maxbox.x = -BOUNDINGBOX_MAXVALUE;
      maxbox.y = -BOUNDINGBOX_MAXVALUE;
      maxbox.z = -BOUNDINGBOX_MAXVALUE;
    }

    /// Initialize this box to one vertex.
    inline void StartBoundingBox (const Vector3& v)
    {
      minbox = v; maxbox = v;
    }

    /// Add a new vertex and recalculate the bounding box.
    inline void AddBoundingVertex (const Vector3& v)
    {
      if (v.x < minbox.x) minbox.x = v.x; if (v.x > maxbox.x) maxbox.x = v.x;
      if (v.y < minbox.y) minbox.y = v.y; if (v.y > maxbox.y) maxbox.y = v.y;
      if (v.z < minbox.z) minbox.z = v.z; if (v.z > maxbox.z) maxbox.z = v.z;
    }

    /// Initialize this box to empty.
    Box () :
    minbox ( BOUNDINGBOX_MAXVALUE,
      BOUNDINGBOX_MAXVALUE,
      BOUNDINGBOX_MAXVALUE),
      maxbox (-BOUNDINGBOX_MAXVALUE,
      -BOUNDINGBOX_MAXVALUE,
      -BOUNDINGBOX_MAXVALUE) {}

    /// Initialize this box with one point.
    Box (const Vector3& v) : minbox (v), maxbox (v) 
    { }

    /// Initialize this box with two points.
    Box (const Vector3& v1, const Vector3& v2) :
    minbox (v1), maxbox (v2)
    { if (Empty ()) StartBoundingBox (); }

    /// Sets the bounds of the box with the given values.
    inline void Set (const Vector3& bmin, const Vector3& bmax)
    {
      minbox = bmin;
      maxbox = bmax;
    }

    /// Test if the given coordinate is in this box.
    bool Contains (const Vector3& v) const;

    /// Test if the two boxes have an intersection.
    bool TestIntersect (const Box& box) const;

    /// Test if this box contains the other box.
    bool Contains (const Box& box) const;

    bool TestIntersect (const Sphere& sphere) const;

    bool Contains (const Sphere& sphere) const;

  };

} // namespace Geom

#endif // GEOM_BOX_H
