/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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

#ifndef COMMON_POSITIONREF_H
#define COMMON_POSITIONREF_H

namespace Common
{
  namespace Inventory
  {
    struct PositionRef
    {
      bool valid;
      unsigned int row;
      unsigned int column;

      PositionRef();
      PositionRef(int column, int row);
      virtual ~PositionRef();

      bool operator==(const PositionRef& reference) const;
      bool operator!=(const PositionRef& reference) const;
      PositionRef& operator=(const PositionRef& reference);
      PositionRef& operator+=(const PositionRef& reference);
      PositionRef& operator-=(const PositionRef& reference);

      operator bool() const { return valid; }
    };

  } // Inventory namespace
} // Common namespace

#endif // COMMON_POSITIONREF_H
