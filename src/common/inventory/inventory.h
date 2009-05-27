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

#ifndef COMMON_INVENTORY_H
#define COMMON_INVENTORY_H

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "src/common/util/flags.h"
#include "src/common/inventory/positionref.h"

#include "common/util/exception.h"

namespace Common
{
  namespace Inventory
  {
    #define ALLOW_ITEMS    0x00000001
    #define ALLOW_SKILLS   0x00000010
    #define ALLOW_ACTIONS  0x00000100

    class Object;

    PT_DEFINE_EXCEPTION(InventoryException);

    struct InventoryCallBack
    {
      virtual void ObjectAdded(boost::shared_ptr<Object>, const PositionRef&) = 0;
      virtual void ObjectRemoved(boost::shared_ptr<Object>, const PositionRef&) = 0;
    };

    /*
     * Inventory base class.
     *
     *      columns/x -->
     * (0,0)-----------
     *      |    |    |  rows/y
     *      -----------   |
     *      |    |    |   |
     *      -----------   v
     *      |    |    |
     *      ----------- (columns, rows)
     *
     */
    class Inventory
    {
    protected:
      std::string inventoryName;
      Utils::Flags inventoryType;
      unsigned int inventoryRows;
      unsigned int inventoryColumns;

    protected:
      std::list<InventoryCallBack*> callback_list;
      void NotifyObjectAdded(boost::shared_ptr<Object>, const PositionRef&);
      void NotifyObjectRemoved(boost::shared_ptr<Object>, const PositionRef&);

    protected:
      bool AllowsType(boost::shared_ptr<Object> object);

    public:
      friend std::ostream& operator<< (std::ostream&, const Inventory&);

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
      virtual void ClearInventory() = 0;

	    /**
       * Auto arranges the inventory.
       * @return void.
       */
      virtual void AutoArrange() = 0;

	    /**
       * Find a free position in the inventory for the given object.
       * @return True if a valid position was found, false otherwise.
       */
      virtual bool FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const = 0;

	    /**
       * Find free positions in the inventory for the given objects.
       * @return True if valid positions for all objects were found, false otherwise.
       */
      virtual bool FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const = 0;

      /**
       * Check if there is an object at the given position.
       * @return True if there is an object, false if the slot is empty.
       */
      virtual bool HasObjectAt(const PositionRef& position) const = 0;

      /**
       * Returns the object at the given position.
       * @return Object.
       */
      virtual boost::shared_ptr<Object> GetObjectAt(const PositionRef& position) const = 0;

      /**
       * Adds an object at the given position.
       * @return True if successful, false if an error occured.
       */
      virtual bool AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object) = 0;

      /**
       * Try to add a list of objects by rearranging the contents.
       * @return True if successful, false otherwise.
       */
      virtual bool AddObjectsArrange(const std::list<boost::shared_ptr<Object> >& objects) = 0;

      /**
       * Adds objects at the given positions.
       * All the positions NEED to be valid.
       * Use with FindFreePositions() only.
       * @return void.
       */
      virtual void AddObjectsAt(const std::list<PositionRef>& position, const std::list<boost::shared_ptr<Object> >& objects) = 0;

      /**
       * Remove the object at the given position.
       * @return The object if successful, 0 if an error occured.
       */
      virtual boost::shared_ptr<Object> RemoveObjectAt(const PositionRef& position) = 0;

      /**
       * Remove the given object from this inventory.
       * @return The PositionRef at which the object was removed if successful, invalid PositionRef if an error occured.
       */
      virtual PositionRef RemoveObject(boost::shared_ptr<Object> object) = 0;

      /**
       * Move an object at the given position to the new location.
       * @return True if successful, false if an error occured.
       */
      virtual bool MoveObject(const PositionRef& curpos, const PositionRef& newpos, bool allowSwap=true) = 0;
     
      /**
       *
       * @return void.
       */
      virtual void GetObjects(std::list<PositionRef>& positions, std::list<boost::shared_ptr<Object> >& objects) = 0;

      /**
       * This is called from the other inventory to notify this inventory
       * that the object has moved. This inventory would then do some cleanup
       * and remove the object from it's list.
       */
      virtual void ObjectMovedToOther(Inventory* other, boost::shared_ptr<Object> object) = 0;

      /**
       * Add the callback to this inventory.
       */
      void AddInventoryCallBack(InventoryCallBack*);

      /**
       * Remove the callback to this inventory.
       */
      void RemoveInventoryCallBack(InventoryCallBack*);
    };

  } // Inventory namespace
} // Common namespace

#endif // COMMON_INVENTORY_H

