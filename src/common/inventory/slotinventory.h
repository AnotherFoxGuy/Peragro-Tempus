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
 * @file slotinventory.h
 */

#ifndef COMMON_SLOTINVENTORY_H
#define COMMON_SLOTINVENTORY_H

#include <list>

#include <boost/shared_ptr.hpp>

#include "src/common/inventory/inventory.h"
#include "src/common/inventory/slot.h"

namespace Common
{
  namespace Inventory
  {
    /**
     * @ingroup Inventory
     * A slot based inventory.
     */
    class SlotInventory : public Inventory
    {
    protected:
      unsigned int visibleRows;
      unsigned int visibleColumns;

      std::list<boost::shared_ptr<Slot> > slots;

      PositionRef IdToPos(unsigned int id) const;
      unsigned int PosToId(const PositionRef& position) const;

      boost::shared_ptr<Slot> GetSlot(const PositionRef& position) const;
      boost::shared_ptr<Slot> GetSlot(unsigned int id) const;

    public:
      /**
      * Base constructor
      */
      SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns);

      ///Create scrollable inventory.
      SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns,
        unsigned int visibleRows, unsigned int visibleColumns);

      virtual ~SlotInventory();

      virtual void ClearInventory();
      virtual void AutoArrange();

      virtual bool FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const;
      virtual bool FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const;

      ///@todo Make these functions throw an exception instead of returning 0.
      virtual boost::shared_ptr<Object> GetObjectAt(const PositionRef& position) const;
      virtual boost::shared_ptr<Object> GetObjectAt(unsigned int id) const;

      ///@todo Make these functions throw an exception instead of returning bool.
      virtual bool AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object);
      virtual bool AddObjectAt(unsigned int id, boost::shared_ptr<Object> object);

      virtual bool AddObjectsArrange(const std::list<boost::shared_ptr<Object> >& objects);
      virtual void AddObjectsAt(const std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects);

      ///@todo Make these functions throw an exception instead of returning 0.
      virtual boost::shared_ptr<Object> RemoveObjectAt(const PositionRef& position);
      virtual boost::shared_ptr<Object> RemoveObjectAt(unsigned int id);
      virtual PositionRef RemoveObject(boost::shared_ptr<Object> object);

      virtual bool HasObjectAt(const PositionRef& position) const;
      virtual bool HasObjectAt(unsigned int id) const;

      virtual bool MoveObject(const PositionRef& curpos, const PositionRef& newpos, bool allowSwap=true);
      virtual bool MoveObject(unsigned int curpos, unsigned int newpos, bool allowSwap=true);

      virtual void GetObjects(std::list<PositionRef>& positions, std::list<boost::shared_ptr<Object> >& objects);

      virtual void ObjectMovedToOther(Inventory* other, boost::shared_ptr<Object> object);
    };


  } // Inventory namespace
} // Common namespace

#endif // COMMON_SLOTINVENTORY_H

