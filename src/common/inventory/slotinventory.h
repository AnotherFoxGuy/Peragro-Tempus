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

#ifndef PT_COMMON_SLOTINVENTORY_H
#define PT_COMMON_SLOTINVENTORY_H

#include <vector>

#include <boost/smart_ptr.hpp>

#include "src/common/inventory/inventory.h"
#include "src/common/inventory/slot.h"

namespace PT
{
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
      private:
        unsigned int visibleRows;
        unsigned int visibleColumns;

        std::vector<boost::shared_ptr<Slot> > slots;

        PositionRef IdToPos(unsigned int id) const;
        Slot* GetSlot(PositionRef position) const;
        Slot* GetSlot(unsigned int id) const;

      public:
        /**
        * Base constructor
        */
        SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns);

        ///Create scrollable inventory. 
        SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns, 
          unsigned int visibleRows, unsigned int visibleColumns);

        virtual ~SlotInventory();

        ///@todo Make these functions throw an exception instead of returning 0.
        virtual Object* GetObjectAt(const PositionRef& position) const;
        virtual Object* GetObjectAt(unsigned int id) const;

        ///@todo Make these functions throw an exception instead of returning bool.
        virtual bool AddObjectAt(const PositionRef& position, Object* object);
        virtual bool AddObjectAt(unsigned int id, Object* object);

        ///@todo Make these functions throw an exception instead of returning bool.
        virtual bool RemoveObjectAt(const PositionRef& position);
        virtual bool RemoveObjectAt(unsigned int id);

        virtual bool HasObjectAt(const PositionRef& position) const;
        virtual bool HasObjectAt(unsigned int id) const;
      };
      

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_SLOTINVENTORY_H

