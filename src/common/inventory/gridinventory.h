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
 * @file gridinventory.h
 */

#ifndef COMMON_GRIDINVENTORY_H
#define COMMON_GRIDINVENTORY_H

#include "src/common/inventory/inventory.h"

#include <wfmath/quadtree.h>

namespace Common
{
  namespace Inventory
  {
    /**
     * @ingroup Inventory
     * A Diablo style grid based inventory.
     */
    class GridInventory : public Inventory
    {
    private:
      struct PositionedObject;
      typedef WFMath::QuadTree<PositionedObject, WFMath::AxisBox<2>, false>::Type Quadtree;
      struct PositionedObject
      {
        std::string name;
        Object* object;
        Quadtree::Shape shape;
      };

    private:
      /// Thing that holds all the objects.
      Quadtree container;

    public:
      /**
       * Base constructor
       */
      GridInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns);
      virtual ~GridInventory();
    };


  } // Inventory namespace
} // Common namespace

#endif // COMMON_GRIDINVENTORY_H

