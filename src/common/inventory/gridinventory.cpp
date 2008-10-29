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

/**
 * @file slotinventory.cpp
 */

#include "gridinventory.h"

namespace PT
{
  namespace Common
  {
    namespace Inventory
    {
      GridInventory::GridInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns)
        :Inventory(name, type, rows, columns)
      {
        container = new Slot**[GetRowCount()];
        for (unsigned int x = 0; x < GetRowCount(); x++)
        {
          container[x] = new Slot*[GetColumnCount()];
          for (unsigned int y = 0; y < GetColumnCount(); y++)
          {
            container[x][y] = 0;
          }
        }
      }

      GridInventory::~GridInventory()
      {
        if (container != 0)
        {
          for (unsigned int x = 0; x < GetColumnCount(); x++)
          {
            delete [] container[x];
          }
          delete [] container;
        }
      }


    } // Inventory namespace
  } // Common namespace
} // PT namespace