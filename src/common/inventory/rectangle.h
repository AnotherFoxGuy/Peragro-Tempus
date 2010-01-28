/*
    Copyright (C) 2010 Development Team of Peragro Tempus

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

#ifndef COMMON_MATH_SIZE_H
#define COMMON_MATH_SIZE_H

#include "size.h"
#include "positionref.h"

namespace Common
{
  namespace Inventory
  {
    namespace Math
    {
      struct Rectangle
      {
        PositionRef m_lowCorner;
        PositionRef m_highCorner;

        Rectangle();
        Rectangle(const PositionRef& p1, const PositionRef& p2);
        virtual ~Rectangle();

        /// Get a reference to corner 0
        const PositionRef& lowCorner() const	{return m_lowCorner;}
        /// Get a reference to corner 3
        const PositionRef& highCorner() const	{return m_highCorner;}

        bool operator==(const Rectangle& reference) const;
        bool operator!=(const Rectangle& reference) const;
        Rectangle& operator+=(const PositionRef& reference);

        operator bool() const { return m_lowCorner.valid && m_highCorner.valid; }

        Size GetSize() const;

        bool Intersects(const Rectangle& r, bool proper) const;
        bool Contains(const Rectangle& r, bool proper) const;

        bool Intersects(const PositionRef& r, bool proper) const;
      };

    } // Math namespace
  } // Inventory namespace
} // Common namespace

#endif // COMMON_SIZE_H
