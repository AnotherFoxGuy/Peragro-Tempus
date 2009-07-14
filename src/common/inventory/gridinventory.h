/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
 * @file gridinventory.h
 */

#ifndef COMMON_GRIDINVENTORY_H
#define COMMON_GRIDINVENTORY_H

#include "object.h"

#include "inventory.h"

#include <wfmath/quadtree.h>

namespace Common
{
  namespace Inventory
  {
    class Object;

    /**
     * @ingroup Inventory
     * A Diablo style grid based inventory.
     */
    class GridInventory : public Inventory
    {
    private:
      typedef WFMath::QuadTree<WFMath::AxisBox<2>, false>::Type Quadtree;
      struct PositionedObject : public WFMath::Shape<WFMath::AxisBox<2> >
      {
        PositionRef position;
        boost::shared_ptr<Object> object;

        PositionedObject(boost::shared_ptr<Object> o, const PositionRef& pos) : position(pos)
        {
          object = o;
          SetPosition(pos);
        }

        void SetPosition(const PositionRef& pos)
        {
          position = pos;
          SetShape(GridInventory::GetBB(pos, object->GetSize()));
        }
      };

    private:
      /// Quadtree, doesn't hold refs to the objects.
      Quadtree quadtree;
      /// Container that holds refs to all the objects.
      std::list<boost::shared_ptr<PositionedObject> > objects;

      static WFMath::AxisBox<2> GetBB(const PositionRef& pos, const WFMath::AxisBox<2>& size, float offset = 0.0f);

      bool IsInInventory(const WFMath::AxisBox<2>& bb);

      boost::shared_ptr<PositionedObject> GetPositionedObjectAt(const PositionRef& position) const;

    public:
      /**
       * Base constructor
       */
      GridInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns);
      virtual ~GridInventory();

      /**
       * Clears the inventory, removing all slots their contents.
       * @return void.
       */
      virtual void ClearInventory();

      /**
       * Auto arranges the inventory.
       * @return void.
       */
      virtual void AutoArrange();

      /**
       * Find a free position in the inventory for the given object.
       * @return True if a valid position was found, false otherwise.
       */
      virtual bool FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const;

      /**
       * Find free positions in the inventory for the given objects.
       * @return True if valid positions for all objects were found, false otherwise.
       */
      virtual bool FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const;

      /**
       * Check if there is an object at the given position.
       * @return True if there is an object, false if the slot is empty.
       */
      virtual bool HasObjectAt(const PositionRef& position) const;

      /**
       * Returns the object at the given position.
       * @return Object.
       */
      virtual boost::shared_ptr<Object> GetObjectAt(const PositionRef& position) const;

      /**
       * Adds an object at the given position.
       * @return True if successful, false if an error occured.
       */
      virtual bool AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object);

      /**
       * Try to add a list of objects by rearranging the contents.
       * @return True if successful, false otherwise.
       */
      virtual bool AddObjectsArrange(const std::list<boost::shared_ptr<Object> >& objects);

      /**
       * Adds objects at the given positions.
       * All the positions NEED to be valid.
       * Use with FindFreePositions() only.
       * @return void.
       */
      virtual void AddObjectsAt(const std::list<PositionRef>& position, const std::list<boost::shared_ptr<Object> >& objects);

      /**
       * Remove the object at the given position.
       * @return The object if successful, 0 if an error occured.
       */
      virtual boost::shared_ptr<Object> RemoveObjectAt(const PositionRef& position);

      /**
       * Remove the given object from this inventory.
       * @return The PositionRef at which the object was removed if successful, invalid PositionRef if an error occured.
       */
      virtual PositionRef RemoveObject(boost::shared_ptr<Object> object);

      /**
       * Move an object at the given position to the new location.
       * @return True if successful, false if an error occured.
       */
      virtual bool MoveObject(const PositionRef& curpos, const PositionRef& newpos, bool allowSwap=true);

      /**
       *
       * @return void.
       */
      virtual void GetObjects(std::list<PositionRef>& positions, std::list<boost::shared_ptr<Object> >& objects);

      /**
       * This is called from the other inventory to notify this inventory
       * that the object has moved. This inventory would then do some cleanup
       * and remove the object from it's list.
       */
      virtual void ObjectMovedToOther(Inventory* other, boost::shared_ptr<Object> object);
    };


  } // Inventory namespace
} // Common namespace

#endif // COMMON_GRIDINVENTORY_H

