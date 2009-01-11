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

#include "box.h"

namespace Geom
{

  Vector3 Box::GetCorner (int corner) const
  {
    switch (corner)
    {
    case BOX_CORNER_xyz: return Min ();
    case BOX_CORNER_xyZ: return Vector3 (MinX (), MinY (), MaxZ ());
    case BOX_CORNER_xYz: return Vector3 (MinX (), MaxY (), MinZ ());
    case BOX_CORNER_xYZ: return Vector3 (MinX (), MaxY (), MaxZ ());
    case BOX_CORNER_Xyz: return Vector3 (MaxX (), MinY (), MinZ ());
    case BOX_CORNER_XyZ: return Vector3 (MaxX (), MinY (), MaxZ ());
    case BOX_CORNER_XYz: return Vector3 (MaxX (), MaxY (), MinZ ());
    case BOX_CORNER_XYZ: return Max ();
    case BOX_CENTER3:    return GetCenter ();
    }

    return Vector3 (0, 0, 0);
  }

  void Box::SetCenter (const Vector3 &c)
  {
    Vector3 size = GetSize ();
    minbox = c - size * 0.5f;
    maxbox = c + size * 0.5f;
  }

  void Box::SetSize (const Vector3 &s)
  {
    Vector3 center = GetCenter ();
    minbox = center - s * .5;
    maxbox = center + s * .5;
  }

  bool Box::Contains (const Vector3& v) const
  {
    if (v.x < minbox.x || v.x > maxbox.x) return false;
    if (v.y < minbox.y || v.y > maxbox.y) return false;
    if (v.z < minbox.z || v.z > maxbox.z) return false;
    return true;
  }

  bool Box::TestIntersect (const Box& box) const
  {
    if (box.minbox.x >= maxbox.x) return false;
    if (box.minbox.y >= maxbox.y) return false;
    if (box.minbox.z >= maxbox.z) return false;
    if (box.maxbox.x <= minbox.x) return false;
    if (box.maxbox.y <= minbox.y) return false;
    if (box.maxbox.z <= minbox.z) return false;
    return true;
  }

  bool Box::Contains (const Box& box) const
  {
    return (box.minbox.x >= minbox.x && box.maxbox.x <= maxbox.x) &&
      (box.minbox.y >= minbox.y && box.maxbox.y <= maxbox.y) &&
      (box.minbox.z >= minbox.z && box.maxbox.z <= maxbox.z);
  }

  bool Box::TestIntersect (const Sphere& sphere) const
  {
    return false;
  }

  bool Box::Contains (const Sphere& sphere) const
  {
    return false;
  }

} // namespace Geom

