/*
  Copyright (C) 2007 - 2008 Development Team of Peragro Tempus

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

#include "positionref.h"

namespace Common
{
  namespace Inventory
  {
    PositionRef::PositionRef()
    {
      valid = false;
    }

    PositionRef::PositionRef(int column, int row)
    {
      PositionRef::row = row;
      PositionRef::column = column;
      valid = true;
    }

    PositionRef::~PositionRef()
    {
    }

    bool PositionRef::operator==(const PositionRef& reference) const
    {
      return row == reference.row && column == reference.column;
    }

    bool PositionRef::operator!=(const PositionRef& reference) const
    {
      return !operator==(reference);
    }

    PositionRef& PositionRef::operator=(const PositionRef& reference)
    {
      if (this == &reference)
      {
        return *this;
      }

      row = reference.row;
      column = reference.column;
      return *this;
    }

    PositionRef& PositionRef::operator+=(const PositionRef& reference)
    {
      row += reference.row;
      column += reference.column;
      return *this;
    }

    PositionRef& PositionRef::operator-=(const PositionRef& reference)
    {

      if (reference.row <= row)
        row -= reference.row;
      else
        row = 0;

      if (reference.column <= column)
        column -= reference.column;
      else
        column = 0;

      return *this;
    }

  }
}
