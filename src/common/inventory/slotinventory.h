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

        std::vector<Slot*> slots;

        public:
        /**
         * Base constructor
         */
        SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns);

        ///Create scrollable inventory. 
        SlotInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns, 
          unsigned int visibleRows, unsigned int visibleColumns);

        virtual ~SlotInventory();

        virtual Slot* GetSlot(PositionRef position) const;
        virtual Slot* GetSlot(unsigned int id) const;
      };
      

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_SLOTINVENTORY_H

