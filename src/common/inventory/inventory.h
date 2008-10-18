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
 * @file inventory.h
 */

#include <string>

#include "src/common/util/flags.h"
#include "src/common/inventory/positionref.h"

#ifndef PT_COMMON_INVENTORY_H
#define PT_COMMON_INVENTORY_H

namespace PT
{
  namespace Common
  {
    namespace Inventory
    {
      #define 	ALLOW_ITEMS    0x00000001
      #define 	ALLOW_SKILLS   0x00000010
      #define 	ALLOW_ACTIONS  0x00000100

      class Object;

      class Inventory
      {
      protected:
        std::string inventoryName;
        Utils::Flags inventoryType;
        unsigned int inventoryRows;
        unsigned int inventoryColumns;

      public:
        /**
         * Base constructor
         */
        Inventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns);
        virtual ~Inventory();

        /**
         * Set the name for this Inventory.
         * @param name  The name to set it to.
         */
        virtual void SetName(const std::string& name);

        /**
         * Get the name for this inventory.
         * @return A string with the name.
         */
        virtual const std::string& GetName() const;

        /**
         * Get the amount of rows this inventory holds.
         * @return Unsigned int with the amount of rows.
         */
        virtual unsigned int GetRowCount() const;

        /**
         * Get the amount of columns this inventory holds.
         * @return Unsigned int with the amount of columns.
         */
        virtual unsigned int GetColumnCount() const;

        /**
         * Clears the inventory, removing all slots their contents.
         * @return void.
         */
        virtual void ClearInventory();

        /**
         * Check if there is an object at the given position.
         * @return True if there is an object, false if the slot is empty.
         */
        virtual bool HasObjectAt(const PositionRef& position) const;

        /**
         * Check if there is an object in the rectangle that is defined by the 
         * first(upper left corner) and the second position(bottom right corner).
         * @return True if there is an object, false if the slot is empty..
         */
        virtual bool HasObjectBetween(const PositionRef& upperLeftCorner, const PositionRef& bottomRightCorner) const;

        /**
         * Returns the object at the given position.
         * @return Object*.
         */
        virtual Object* GetObjectAt(const PositionRef& position) const;

        /**
         * Adds an object at the given position.
         * @return True if successful, false if an error occured.
         */
        virtual bool AddObjectAt(const PositionRef& position, const Object* object);

        /**
         * Remove the object at the given position.
         * @return True if successful, false if an error occured.
         */
        virtual bool RemoveObjectAt(const PositionRef& position) = 0;

        /**
         * Remove the given object from this inventory.
         * @return True if successful, false if an error occured.
         */
        virtual bool RemoveObject(const Object* object) = 0;

        /**
         * This is called from the other inventory to notify this inventory
         * that the object has moved. This inventory would then do some cleanup
         * and remove the object from it's list.
         */
        virtual void ObjectMovedToOther(Inventory* other, Object* object) = 0;
      };
      

    } // Inventory namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_INVENTORY_H

