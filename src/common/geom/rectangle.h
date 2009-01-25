/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#ifndef GEOM_RECTANGLE_H
#define GEOM_RECTANGLE_H

#include "vector2.h"

#include "vector3.h"

namespace Geom
{
  class Sphere;
  
  class Rectangle
  {
  protected:
    Vector2 minbox;
    Vector2 maxbox;

  public:
    Rectangle () {}

    Rectangle (const Vector3& v1, const Vector3& v2) {}
   
    bool Contains (const Vector2& v) const { return false; }
    bool Intersect (const Rectangle& rect) const { return false; }
    bool Contains (const Rectangle& rect) const { return false; }
  };

} // namespace Geom

#endif // GEOM_RECTANGLE_H
