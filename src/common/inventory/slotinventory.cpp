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

#include "slotinventory.h"

namespace PT
{
  namespace Common
  {
    namespace Inventory
    {
      SlotInventory::SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns)
        :Inventory(name, type, rows, columns)
      {
      }

      SlotInventory::SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns, 
          unsigned int visibleRows, unsigned int visibleColumns)
          :Inventory(name, type, rows, columns)
      {
        SlotInventory::visibleRows = visibleRows;
        SlotInventory::visibleColumns = visibleColumns;
      }

      SlotInventory::~SlotInventory()
      {
      }

      Slot* SlotInventory::GetSlot(PositionRef position) const
      {
        for(unsigned int i=0; i<slots.size(); i++)
        {
          if(slots[i]->GetPosition() == position){ return slots[i]; }
        }
        return 0;
      }

      Slot* SlotInventory::GetSlot(unsigned int id) const
      {
        // Determine row
        int slotRow = 0;
        int slotColumn = 0;
        if (id > inventoryRows)
        {
          slotRow = id - inventoryRows;
        }
        else
        {
          slotRow = inventoryRows - id;
        }
        id -= slotRow;

        // Determine column
        if (id > inventoryColumns)
        {
          slotColumn = id - inventoryColumns;
        }
        else
        {
          slotColumn = inventoryColumns - id;
        }
        id -= slotColumn;

        if (id != 0)
        {
          return 0;
        }
        return GetSlot(PositionRef(slotRow, slotColumn));
      }

    } // Inventory namespace
  } // Common namespace
} // PT namespace
