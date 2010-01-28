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

#include <string>

#include "rectangle.h"

namespace Common
{
  namespace Inventory
  {
    namespace Math
    {
      // Some helpers.
      inline bool _Less(int x1, int x2, bool proper)
      {
        return !proper ? x1 < x2 : x1 <= x2;
      }

      inline bool _LessEq(int x1, int x2, bool proper)
      {
        return !proper ? x1 <= x2 : x1 < x2;
      }

      inline bool _Greater(int x1, int x2, bool proper)
      {
        return !proper ? x1 > x2 : x1 >= x2;
      }

      inline bool _GreaterEq(int x1, int x2, bool proper)
      {
        return !proper ? x1 >= x2 : x1 > x2;
      }


      Rectangle::Rectangle()
      {
      }

      Rectangle::Rectangle(const PositionRef& p1, const PositionRef& p2)
      {
        Rectangle::m_lowCorner = p1;
        Rectangle::m_highCorner = p2;
      }

      Rectangle::~Rectangle()
      {
      }

      bool Rectangle::operator==(const Rectangle& reference) const
      {
        return m_lowCorner == reference.m_lowCorner && m_highCorner == reference.m_highCorner;
      }

      bool Rectangle::operator!=(const Rectangle& reference) const
      {
        return !operator==(reference);
      }

      Rectangle& Rectangle::operator+=(const PositionRef& p)
      {
        m_lowCorner += p;
        m_highCorner += p;
        return *this;
      }

      Size Rectangle::GetSize() const
      {
        return Size(m_highCorner.column-m_lowCorner.column, m_highCorner.row-m_lowCorner.row);
      }

      bool Rectangle::Intersects(const Rectangle& b2, bool proper) const
      {
        if(_Greater(m_lowCorner.column, b2.m_highCorner.column, proper)
          || _Less(m_highCorner.column, b2.m_lowCorner.column, proper))
          return false;

        if(_Greater(m_lowCorner.row, b2.m_highCorner.row, proper)
          || _Less(m_highCorner.row, b2.m_lowCorner.row, proper))
          return false;

        return true;
      }

      bool Rectangle::Contains(const Rectangle& inner, bool proper) const
      {
        if(_Less(inner.m_lowCorner.column, m_lowCorner.column, proper)
          || _Greater(inner.m_highCorner.column, m_highCorner.column, proper))
          return false;

        if(_Less(inner.m_lowCorner.row, m_lowCorner.row, proper)
          || _Greater(inner.m_highCorner.row, m_highCorner.row, proper))
          return false;

        return true;
      }

      bool Rectangle::Intersects(const PositionRef& p, bool proper) const
      {
        if(_Greater(m_lowCorner.column, p.column, !proper) || _Less(m_highCorner.column, p.column, proper))
          return false;

        if(_Greater(m_lowCorner.row, p.row, !proper) || _Less(m_highCorner.row, p.row, proper))
          return false;

        return true;
      }

    } // Math namespace
  } // Inventory namespace
} // Common namespace
