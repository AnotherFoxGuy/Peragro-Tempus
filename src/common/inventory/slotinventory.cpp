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

#include "object.h"

namespace Common
{
  namespace Inventory
  {
    SlotInventory::SlotInventory(const std::string& name, Utils::Flags type, unsigned int columns, unsigned int rows)
      :Inventory(name, type, columns, rows)
    {
    }

    SlotInventory::SlotInventory(const std::string& name, Utils::Flags type, unsigned int columns, unsigned int rows,
        unsigned int visibleRows, unsigned int visibleColumns)
        :Inventory(name, type, columns, rows)
    {
      SlotInventory::visibleRows = visibleRows;
      SlotInventory::visibleColumns = visibleColumns;
    }

    SlotInventory::~SlotInventory()
    {
    }

    void SlotInventory::ClearInventory()
    {
      std::list<boost::shared_ptr<Object> > objects;
      std::list<PositionRef> positions;
      GetObjects(positions, objects);

      std::list<boost::shared_ptr<Object> >::iterator it;
      for (it = objects.begin(); it != objects.end(); it++)
      {
        RemoveObject(*it);
      }
    }

    void SlotInventory::AutoArrange()
    {
    }

    bool SlotInventory::FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const
    {
      std::list<PositionRef> positions;
      std::list<boost::shared_ptr<Object> > objects; objects.push_back(object);
      if (FindFreePositions(positions, objects))
      {
        position = *positions.begin();
        return true;
      }
      return false;
    }

    bool SlotInventory::FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const
    {
      positions.clear();
      for (unsigned int row = 0; row < inventoryRows; row++)
      {
        for (unsigned int col = 0; col < inventoryColumns; col++)
        {
          PositionRef pos(col, row);
          if(!HasObjectAt(pos))
          {
            positions.push_back(pos);
            if (positions.size() == objects.size()) return true;
          }
        }
      }

      return false;
    }

    PositionRef SlotInventory::IdToPos(unsigned int id) const
    {
      if (id >= inventoryColumns*inventoryRows)
        throw PT_EX(InventoryException("ID out of inventory range."));
      unsigned int c = id%inventoryColumns;
      unsigned int r = id/inventoryColumns;
      return PositionRef(c, r);
    }

    unsigned int SlotInventory::PosToId(const PositionRef& position) const
    {
      return (position.row * inventoryColumns) + position.column;
    }

    boost::shared_ptr<Slot> SlotInventory::GetSlot(const PositionRef& position) const
    {
      std::list<boost::shared_ptr<Slot> >::const_iterator it;
      for (it=slots.begin(); it != slots.end(); it++)
      {
        if((*it)->GetPosition() == position)
          return *it;
      }
      return boost::shared_ptr<Slot>();
    }

    boost::shared_ptr<Slot> SlotInventory::GetSlot(unsigned int id) const
    {
      return GetSlot(IdToPos(id));
    }

    bool SlotInventory::AddObjectAtImpl(const PositionRef& position, boost::shared_ptr<Object> object)
    {
      if(position.column >= inventoryColumns
        && position.row >= inventoryRows)
        throw PT_EX(InventoryException("Position out of inventory range."));

      if (!AllowsType(object)) return false;

      boost::shared_ptr<Slot> slot = GetSlot(position);
      if(!slot)
      {
        slot = boost::shared_ptr<Slot>(new Slot(this, position));
        slots.push_back(slot);
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

    boost::shared_ptr<Object> SlotInventory::RemoveObjectAtImpl(const PositionRef& position)
    {
      std::list<boost::shared_ptr<Slot> >::iterator it;
      for (it=slots.begin(); it != slots.end(); it++)
      {
        if((*it)->GetPosition() == position)
        {
          boost::shared_ptr<Object> object = (*it)->GetContents();
          if (object && object->GetParent() == this) object->SetParent(0);
          slots.erase(it);
          return object;
        }
      }
      return boost::shared_ptr<Object>();
    }

    boost::shared_ptr<Object> SlotInventory::GetObjectAt(const PositionRef& position) const
    {
      boost::shared_ptr<Slot> slot = GetSlot(position);
      if(!slot){ return boost::shared_ptr<Object>(); }
      return slot->GetContents();
    }

    boost::shared_ptr<Object> SlotInventory::GetObjectAt(unsigned int id) const
    {
      boost::shared_ptr<Slot> slot = GetSlot(id);
      if(!slot){ return boost::shared_ptr<Object>(); }
      return slot->GetContents();
    }

    bool SlotInventory::AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object)
    {
      if (AddObjectAtImpl(position, object))
      {
        NotifyObjectAdded(object, position);

        // Notify the other inventory that this object has moved here.
        if (object->GetParent() && object->GetParent() != this)
        {
          object->GetParent()->ObjectMovedToOther(this, object);
        }

        object->SetParent(this);

        return true;
      }
      return false;
    }

    bool SlotInventory::AddObjectAt(unsigned int id, boost::shared_ptr<Object> object)
    {
      return AddObjectAt(IdToPos(id), object);
    }

    bool SlotInventory::AddObjectsArrange(const std::list<boost::shared_ptr<Object> >& objects)
    {
      return false;
    }

    void SlotInventory::AddObjectsAt(const std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects)
    {
      std::list<PositionRef>::const_iterator it1 = positions.begin();
      std::list<boost::shared_ptr<Object> >::const_iterator it2 = objects.begin();
      for (; ((it1 != positions.end()) && (it2 != objects.end())); it1++, it2++)
      {
        if(!AddObjectAt(*it1, *it2))
          throw PT_EX(InventoryException("Invalid position for AddObjectsAt!")); //TODO: undo possible adds?
      }
    }

    boost::shared_ptr<Object> SlotInventory::RemoveObjectAt(const PositionRef& position)
    {
      boost::shared_ptr<Object> object = RemoveObjectAtImpl(position);
      if (object) NotifyObjectRemoved(object, position);

      return object;
    }

    boost::shared_ptr<Object> SlotInventory::RemoveObjectAt(unsigned int id)
    {
      return RemoveObjectAt(IdToPos(id));
    }

    PositionRef SlotInventory::RemoveObject(boost::shared_ptr<Object> object)
    {
      std::list<boost::shared_ptr<Slot> >::iterator it;
      for (it=slots.begin(); it != slots.end(); it++)
      {
        if((*it)->GetContents() == object)
        {
          if (object && object->GetParent() == this) object->SetParent(0);
          PositionRef ref = (*it)->GetPosition();
          NotifyObjectRemoved(object, ref);
          slots.erase(it);
          return ref;
        }
      }
      return PositionRef();
    }

    bool SlotInventory::HasObjectAt(const PositionRef& position) const
    {
      boost::shared_ptr<Slot> slot = GetSlot(position);
      if(!slot){return false;}
      return slot->HasContents();
    }

    bool SlotInventory::HasObjectAt(unsigned int id) const
    {
      return HasObjectAt(IdToPos(id));
    }

    bool SlotInventory::MoveObject(const PositionRef& curpos, const PositionRef& newpos, bool allowSwap)
    {
      if (curpos == newpos) return false;

      bool hasObjAtNew = HasObjectAt(newpos);
      if (hasObjAtNew && !allowSwap) return false; // No swapping allowed.

      boost::shared_ptr<Object> curo = RemoveObjectAtImpl(curpos);
      boost::shared_ptr<Object> newo;
      if (!curo) return false; // No object at curpos
      curo->SetParent(this);
      if (hasObjAtNew)
      {
        newo = RemoveObjectAtImpl(newpos);
        if (!AddObjectAtImpl(curpos, newo)) throw PT_EX(InventoryException("Failed to add object for unknown reason!"));
        newo->SetParent(this);
      }
      if (!AddObjectAtImpl(newpos, curo)) throw PT_EX(InventoryException("Failed to add object for unknown reason!"));

      NotifyObjectMoved(curo, newpos, newo, curpos);

      return true;
    }

    bool SlotInventory::MoveObject(unsigned int curpos, unsigned int newpos, bool allowSwap)
    {
      return MoveObject(IdToPos(curpos), IdToPos(newpos), allowSwap);
    }

    void SlotInventory::GetObjects(std::list<PositionRef>& positions, std::list<boost::shared_ptr<Object> >& objects)
    {
      std::list<boost::shared_ptr<Slot> >::const_iterator it = slots.begin();
      for (; it != slots.end(); it++)
      {
        if (!(*it)->HasContents()) continue;
        positions.push_back((*it)->GetPosition());
        objects.push_back((*it)->GetContents());
      }
    }

    void SlotInventory::ObjectMovedToOther(Inventory* other, boost::shared_ptr<Object> object)
    {
      RemoveObject(object);
    }

  } // Inventory namespace
} // Common namespace
