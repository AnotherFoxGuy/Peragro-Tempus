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

#include "gridinventory.h"

namespace Common
{
  namespace Inventory
  {
    GridInventory::GridInventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns)
      :Inventory(name, type, rows, columns)
    {
    }

    GridInventory::~GridInventory()
    {
    }

    WFMath::AxisBox<2> GridInventory::GetBB(const PositionRef& pos, const WFMath::AxisBox<2>& size, float offset)
    {
      using namespace WFMath;
      AxisBox<2> p(size.lowCorner()+Vector<2>(offset), size.highCorner()-Vector<2>(offset));
      p += Point<2>(pos.column, pos.row);
      return p;
    }

    bool GridInventory::IsInInventory(const WFMath::AxisBox<2>& bb)
    {
      WFMath::Point<2> low = bb.lowCorner();
      WFMath::Point<2> high = bb.highCorner();
      if (low.x() < 0 || low.y() < 0) return false;
      if (high.x() > inventoryColumns || high.y() > inventoryRows) return false;
      return true;
    }

    void GridInventory::ClearInventory()
    {
      objects.clear();
    }

    boost::shared_ptr<GridInventory::PositionedObject> GridInventory::GetPositionedObjectAt(const PositionRef& position) const
    {
      std::list<boost::shared_ptr<PositionedObject> > result;
      result = quadtree.Query<PositionedObject>(WFMath::Point<2>(position.column+0.0001f, position.row+0.0001f));
      if (result.size() == 1) return result.front();  
      return boost::shared_ptr<PositionedObject>();
    }

    bool GridInventory::HasObjectAt(const PositionRef& position) const
    {
      return GetPositionedObjectAt(position) != 0;
    }

    boost::shared_ptr<Object> GridInventory::GetObjectAt(const PositionRef& position) const
    {
      boost::shared_ptr<PositionedObject> o = GetPositionedObjectAt(position);
      if (o) return o->object;
      return boost::shared_ptr<Object>();
    }

    bool GridInventory::AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object)
    {
      boost::shared_ptr<PositionedObject> o(new PositionedObject(object, position));
      if (!IsInInventory(o->GetShape())) return false;
      bool success = quadtree.Add(o);
      if (success)
      {
        objects.push_back(o);
        NotifyObjectAdded(object, position);
      }
      return success;
    }

    boost::shared_ptr<Object> GridInventory::RemoveObjectAt(const PositionRef& position)
    {
      boost::shared_ptr<PositionedObject> o = GetPositionedObjectAt(position);
      if (o) 
      {
        boost::shared_ptr<Object> obj = o->object;
        NotifyObjectRemoved(obj, position);
        quadtree.Remove(o);
        objects.remove(o);
        return obj;
      }
      return boost::shared_ptr<Object>();
    }

    PositionRef GridInventory::RemoveObject(boost::shared_ptr<Object> object)
    {
      std::list<boost::shared_ptr<PositionedObject> >::iterator it;
      for (it = objects.begin(); it != objects.end(); it++)
      {
        if ((*it)->object == object)
        {
          PositionRef ref = (*it)->position;
          NotifyObjectRemoved(object, ref);
          quadtree.Remove(*it);
          objects.erase(it);
          return ref;
        }
      }
      return PositionRef();
    }

    bool GridInventory::MoveObject(const PositionRef& curpos, const PositionRef& newpos, bool allowSwap)
    {
      if (curpos == newpos) return true;
      boost::shared_ptr<PositionedObject> curpo = GetPositionedObjectAt(curpos);
      if (!curpo || !curpo->object) return false; // No object at curpos
      
      // Update position for the swapped object to position of the moving object.
      PositionRef curposU = curpo->position;
      // idem.
      PositionRef newposU = newpos;

      boost::shared_ptr<Object> curo = curpo->object;
      // Check if the destination is still inside.
      WFMath::AxisBox<2> bb = GetBB(newpos, curo->GetSize(), 0.0001f);
      if (!IsInInventory(bb)) return false;
      // Check if there is room at the destination.
      std::list<boost::shared_ptr<PositionedObject> > result;
      result = quadtree.Query<PositionedObject>(bb);

      if (result.size() && !allowSwap) return false; // No swapping allowed.

      if (result.size() > 1) return false; // Too many objects at destination.

      if (result.size())
      {
        boost::shared_ptr<PositionedObject> newo = GetPositionedObjectAt(newpos);
        if (newo && newo->object)
        {
          // Update.
          newposU = newo->position;
          // Check if there is room at the destination.
          std::list<boost::shared_ptr<PositionedObject> > result1;
          result1 = quadtree.Query<PositionedObject>(GetBB(newposU, newo->object->GetSize(), 0.0001f));

          if (result1.size() != 1) // 1 == the object we're swapping with.
            return false; // There is not enough room.
        }
      }

      /*boost::shared_ptr<Object>*/ curo = RemoveObjectAt(curpos);
      boost::shared_ptr<Object> newo = RemoveObjectAt(newposU);
      if (!AddObjectAt(newposU, curo)) { throw PT_EX(InventoryException("Failed to add object for unknown reason!"));}
      if (newo) 
      {
        if (!AddObjectAt(curposU, newo)) throw PT_EX(InventoryException("Failed to add object for unknown reason!"));
      }
      return true;
    }

    void GridInventory::ObjectMovedToOther(Inventory* other, boost::shared_ptr<Object> object)
    {
      //TODO
    }


  } // Inventory namespace
} // Common namespace
