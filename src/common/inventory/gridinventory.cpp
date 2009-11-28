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
 * @file gridinventory.cpp
 */

#include "gridinventory.h"

namespace Common
{
  namespace Inventory
  {
    GridInventory::GridInventory(const std::string& name, Utils::Flags type, unsigned int columns, unsigned int rows)
      :Inventory(name, type, columns, rows)
    {
    }

    GridInventory::~GridInventory()
    {
    }

    WFMath::AxisBox<2> GridInventory::GetBB(const PositionRef& pos, const WFMath::AxisBox<2>& size)
    {
      using namespace WFMath;
      AxisBox<2> p(size.lowCorner(), size.highCorner());
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

    boost::shared_ptr<GridInventory::PositionedObject> GridInventory::GetPositionedObjectAt(const PositionRef& position) const
    {
      std::list<boost::shared_ptr<PositionedObject> > result;
      result = quadtree.Query<PositionedObject>(WFMath::Point<2>(position.column+0.0001f, position.row+0.0001f));
      if (result.size() == 1) return result.front();
      return boost::shared_ptr<PositionedObject>();
    }

    bool GridInventory::AddObjectAtImpl(const PositionRef& position, boost::shared_ptr<Object> object)
    {
      boost::shared_ptr<PositionedObject> o(new PositionedObject(object, position));
      if (!IsInInventory(o->GetShape())) return false;
      bool success = quadtree.Add(o);
      if (success)
      {
        objects.push_back(o);
        // Notify the other inventory that this object has moved here.
        if (object->GetParent())
        {
          object->GetParent()->ObjectMovedToOther(this, object);
        }

        object->SetParent(this);
      }
      return success;
    }

    boost::shared_ptr<Object> GridInventory::RemoveObjectAtImpl(const PositionRef& position)
    {
      boost::shared_ptr<PositionedObject> o = GetPositionedObjectAt(position);
      if (o)
      {
        boost::shared_ptr<Object> obj = o->object;
        quadtree.Remove(o);
        objects.remove(o);
        return obj;
      }
      return boost::shared_ptr<Object>();
    }

    PositionRef GridInventory::RemoveObjectImpl(boost::shared_ptr<Object> object)
    {
      std::list<boost::shared_ptr<PositionedObject> >::iterator it;
      for (it = objects.begin(); it != objects.end(); it++)
      {
        if ((*it)->object == object)
        {
          PositionRef ref = (*it)->position;
          quadtree.Remove(*it);
          objects.erase(it);
          return ref;
        }
      }
      return PositionRef();
    }

    void GridInventory::ClearInventory()
    {
      objects.clear();
    }

    void GridInventory::AutoArrange()
    {
    }

    bool GridInventory::FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const
    {
      return false;
    }

    bool GridInventory::FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const
    {
      return false;
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

    bool GridInventory::AddObjectsArrange(const std::list<boost::shared_ptr<Object> >& objects)
    {
      std::list<boost::shared_ptr<Object> > current;
      std::list<PositionRef> notUsed;
      GetObjects(notUsed, current);

      // Append and make one list with all objects.
      std::list<boost::shared_ptr<Object> > all;
      std::merge(current.begin(), current.end(), objects.begin(), objects.end(),
        std::insert_iterator<std::list<boost::shared_ptr<Object> > >(all, all.end()));
/*
      std::list<PositionRef>& poss;
      RectanglePacker packer(columns, rows);

      all.sort(); //Sort from big to small

      if (!packer.FindPositions(all, poss)) return false;

      ClearInventory();
      AddObjectsAt(all, poss);
*/
      return true;
    }

    void GridInventory::AddObjectsAt(const std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects)
    {
      std::list<PositionRef>::const_iterator it1 = positions.begin();
      std::list<boost::shared_ptr<Object> >::const_iterator it2 = objects.begin();
      for (; ((it1 != positions.end()) || (it2 != objects.end())); it1++, it2++)
        if(!AddObjectAt(*it1, *it2)) throw PT_EX(InventoryException("Invalid position for AddObjectsAt!")); //TODO: undo possible adds?
    }

    bool GridInventory::AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object)
    {
      if (AddObjectAtImpl(position, object))
      {
        NotifyObjectAdded(object, position);
        return true;
      }
      return false;
    }

    boost::shared_ptr<Object> GridInventory::RemoveObjectAt(const PositionRef& position)
    {
      boost::shared_ptr<Object> o = RemoveObjectAtImpl(position);
      if (o) NotifyObjectRemoved(o, position);
      return o;
    }

    PositionRef GridInventory::RemoveObject(boost::shared_ptr<Object> object)
    {
      PositionRef o = RemoveObjectImpl(object);
      if (o.valid) NotifyObjectRemoved(object, o);
      return o;
    }

    bool GridInventory::MoveObject(const PositionRef& curpos, const PositionRef& newpos, bool allowSwap)
    {
      if (curpos == newpos) return false;
      boost::shared_ptr<PositionedObject> curpo = GetPositionedObjectAt(curpos);
      if (!curpo || !curpo->object) return false; // No object at curpos

      // Update position for the swapped object to position of the moving object.
      PositionRef curposU = curpo->position;
      // idem.
      PositionRef newposU = newpos;

      boost::shared_ptr<Object> curo = curpo->object;
      // Check if the destination is still inside.
      WFMath::AxisBox<2> bb = GetBB(newposU, curo->GetSize());
      if (!IsInInventory(bb)) return false;
      // Check if there is room at the destination.
      std::list<boost::shared_ptr<PositionedObject> > result;
      result = quadtree.Query<PositionedObject>(bb);
      result.remove(curpo); // Remove moving object from query.

      if (result.size() && !allowSwap) return false; // No swapping allowed.

      if (result.size() > 1) return false; // Too many objects at destination.

      bool objectAtDestination = false;
      if (result.size())
      {
        boost::shared_ptr<PositionedObject> newo = *result.begin();
        if (newo && newo->object && newo->object != curo)
        {
          objectAtDestination = true;
          // Update.
          newposU = newo->position;
          // Check if the object will fit.
          WFMath::AxisBox<2> bb2 = GetBB(curposU, newo->object->GetSize());
          if (!IsInInventory(bb2)) return false;
          if (Intersect(bb, bb2, true)) return false;
          // Check if there is room at the origin for the object at the destination.
          std::list<boost::shared_ptr<PositionedObject> > result1;
          result1 = quadtree.Query<PositionedObject>(bb2);
          result1.remove(curpo); // Remove moving object from query.
          result1.remove(newo); // Remove object being swapped from query.

          if (result1.size()) return false; // There is not enough room.
        }
      }

      /*boost::shared_ptr<Object>*/ curo = RemoveObjectAtImpl(curposU);
      boost::shared_ptr<Object> newo;
      if (objectAtDestination) newo = RemoveObjectAtImpl(newposU);
      if (!AddObjectAtImpl(newpos, curo)) { throw PT_EX(InventoryException("Failed to add object for unknown reason!"));}
      if (newo)
      {
        if (!AddObjectAtImpl(curposU, newo)) throw PT_EX(InventoryException("Failed to add object for unknown reason!"));
      }
      NotifyObjectMoved(curo, newpos, newo, curposU);
      return true;
    }

    void GridInventory::GetObjects(std::list<PositionRef>& positions, std::list<boost::shared_ptr<Object> >& objs)
    {
      std::list<boost::shared_ptr<PositionedObject> >::iterator it;
      for (it = objects.begin(); it != objects.end(); it++)
      {
        positions.push_back((*it)->position);
        objs.push_back((*it)->object);
      }
    }

    void GridInventory::ObjectMovedToOther(Inventory* other, boost::shared_ptr<Object> object)
    {
      //TODO
    }


  } // Inventory namespace
} // Common namespace
