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

#ifndef GEOM_VECTOR2_H
#define GEOM_VECTOR2_H

namespace Geom
{
  /**
   * Simple convenience class representing 2D vector. Access the coordinates
   * directly via members called x and y.
   */
  class Vector2
  {
  public:
    float x,y;

    Vector2(float xv=0, float yv=0) : x(xv), y(yv) {}

    bool operator==(const Vector2 v);

  };

} // namespace Geom

typedef Geom::Vector2 PtVector2;

#endif
