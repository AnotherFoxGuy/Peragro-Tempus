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

      PositionRef SlotInventory::IdToPos(unsigned int id) const
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
          return PositionRef(0, 0);
        }
        return PositionRef(slotRow, slotColumn);
      }

      Slot* SlotInventory::GetSlot(PositionRef position) const
      {
        for(unsigned int i=0; i<slots.size(); i++)
        {
          if(slots[i]->GetPosition() == position)
          { 
            return slots[i].get(); 
          }
        }
        return 0;
      }

      Slot* SlotInventory::GetSlot(unsigned int id) const
      {
        return GetSlot(IdToPos(id));
      }

      Object* SlotInventory::GetObjectAt(const PositionRef& position) const
      {
        Slot* slot = GetSlot(position);
        if(!slot){ return 0; }
        return slot->GetContents();
      }

      Object* SlotInventory::GetObjectAt(unsigned int id) const
      {
        Slot* slot = GetSlot(id);
        if(!slot){ return 0; }
        return slot->GetContents();
      }

      bool SlotInventory::AddObjectAt(const PositionRef& position, Object* object)
      {
        if(position.column > inventoryColumns
          && position.row > inventoryRows)
          return false; // Position out of inventory range.

        Slot* slot = GetSlot(position);
        if(!slot)
        {
          slot = new Slot(this, position);
          slots.push_back(boost::shared_ptr<Slot>(slot));
        }
        else
        {
          ///@todo Check for equal type for stacking?
          if(slot->HasContents())
          {
            return false; // Slot already taken
          } 
        }
        slot->SetContents(object);
        return true;
      }

      bool SlotInventory::AddObjectAt(unsigned int id, Object* object)
      {
        return AddObjectAt(IdToPos(id), object);
      }

      bool SlotInventory::RemoveObjectAt(const PositionRef& position)
      {
        for(unsigned int i=0; i<slots.size(); i++)
        {
          if(slots[i]->GetPosition() == position)
          { 
            slots.erase(slots.begin()+i); 
            return true; 
          }
        }
        return false;
      }

      bool SlotInventory::RemoveObjectAt(unsigned int id)
      {
        return RemoveObjectAt(IdToPos(id));
      }

      bool SlotInventory::HasObjectAt(const PositionRef& position) const
      {
        Slot* slot = GetSlot(position);
        if(!slot){return false;}
        return slot->HasContents();
      }

      bool SlotInventory::HasObjectAt(unsigned int id) const
      {
        return HasObjectAt(IdToPos(id));
      }

    } // Inventory namespace
  } // Common namespace
} // PT namespace
