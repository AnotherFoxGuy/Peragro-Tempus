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

#include "rectanglepacker.h"

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

    Math::Rectangle GridInventory::GetBB(const PositionRef& pos, const Size& size)
    {
      using namespace Math;
      Rectangle p(PositionRef(0, 0), PositionRef(size.width, size.height));
      p += pos;
      return p;
    }

    bool GridInventory::Collide(const std::vector<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects, const Math::Rectangle& bb) const
    {
      std::list<boost::shared_ptr<Object> >::const_iterator it = objects.begin();
      size_t i = 0;
      for (; it != objects.end(); i++, it++ )
      {
        if (positions[i]) //valid position
        {
          Math::Rectangle otherbb = GetBB(positions[i], (*it)->GetSize());
          if (bb.Intersects(otherbb, true)) return true;
        }
      }
      return false;
    }

    bool GridInventory::IsInInventory(const Math::Rectangle& bb) const
    {
      PositionRef low = bb.lowCorner();
      PositionRef high = bb.highCorner();
      if (low.column < 0 || low.row < 0) return false;
      if (high.column > inventoryColumns || high.row > inventoryRows) return false;
      return true;
    }

    boost::shared_ptr<GridInventory::PositionedObject> GridInventory::GetPositionedObjectAt(const PositionRef& position) const
    {
      std::list<boost::shared_ptr<PositionedObject> > result;
      result = quadtree.Query<PositionedObject>(position);
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
        if (obj->GetParent() == this) obj->SetParent(0);
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
      std::list<boost::shared_ptr<Object> > objects;
      std::list<PositionRef> positions;
      GetObjects(positions, objects);

      std::list<PositionRef>::iterator it;
      for (it = positions.begin(); it != positions.end(); it++)
      {
        RemoveObjectAt(*it);
      }
    }

    void GridInventory::AutoArrange()
    {
      std::list<boost::shared_ptr<Object> > objects;
      AddObjectsArrange(objects);
    }

    bool GridInventory::FindFreePosition(PositionRef& position, boost::shared_ptr<Object> object) const
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

    bool GridInventory::FindFreePositions(std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects) const
    {
      positions.clear();
      if (!objects.size()) return true;
      size_t founds = 0;
      std::vector<PositionRef> found(objects.size());
      
      for (unsigned int row = 0; row < inventoryRows; row++)
      {
        for (unsigned int col = 0; col < inventoryColumns; col++)
        {
          PositionRef pos(col, row);
          boost::shared_ptr<PositionedObject> obj = GetPositionedObjectAt(pos);
          if (obj)
          { // Skip ahead.
            const Math::Rectangle& bb = obj->GetShape();
            col = bb.highCorner().column-1;
          }
          else
          {
            std::list<boost::shared_ptr<Object> >::const_iterator it = objects.begin();
            size_t i = 0;
            for (; it != objects.end(); i++, it++ )
            {
              if (!found[i].valid) //Invalid position
              {
                // Check if the destination is still inside.
                Math::Rectangle bb = GetBB(pos, (*it)->GetSize());
                if (!IsInInventory(bb)) continue;
                // Check if there is room at the destination.
                std::list<boost::shared_ptr<PositionedObject> > result;
                result = quadtree.Query<PositionedObject>(bb);
                if (result.size()) continue;
                // Check for collission with already found objects.
                if (Collide(found, objects, bb)) continue;
                //Success
                found[i] = pos;
                founds++;

                // Found them all!
                if (founds == objects.size())
                {
                  std::copy(found.begin(), found.end(), std::insert_iterator<std::list<PositionRef> >(positions, positions.end()));
                  return true;
                }

                // Skip ahead.
                const Size& size = (*it)->GetSize();
                col += size.width-1;
                //We found something for this position, up to the next!
                break;
              }
            }
          } //end else
        } // end for
      } // end for

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

    bool BigToSmall(boost::shared_ptr<Object> i, boost::shared_ptr<Object> j) 
    { 
      const Size& size1 = i->GetSize();
      const Size& size2 = j->GetSize();

      return ((size2.valid*size2.height) < (size1.width*size1.height));
    }

    bool GridInventory::AddObjectsArrange(const std::list<boost::shared_ptr<Object> >& objects)
    {
      std::list<boost::shared_ptr<Object> > current;
      std::list<PositionRef> notUsed;
      GetObjects(notUsed, current);

      // Append and make one list with all objects.
      std::vector<boost::shared_ptr<Object> > all(current.size()+objects.size());
      std::merge(current.begin(), current.end(), objects.begin(), objects.end(), all.begin());

      //Sort from big to small
      std::sort(all.begin(), all.end(), BigToSmall); 

      // TODO all this copying, is there a better way?
      std::list<boost::shared_ptr<Object> > alll;
      std::copy(all.begin(), all.end(), std::insert_iterator<std::list<boost::shared_ptr<Object> > >(alll, alll.end()));

      std::list<PositionRef> poss;
      RectanglePacker packer(inventoryColumns, inventoryRows);

      if (!packer.FindFreePositions(poss, alll)) return false;
      
      ClearInventory();

      // Now add everything back with the new positions.
      AddObjectsAt(poss, alll);

      return true;
    }

    void GridInventory::AddObjectsAt(const std::list<PositionRef>& positions, const std::list<boost::shared_ptr<Object> >& objects)
    {
      std::list<PositionRef>::const_iterator it1 = positions.begin();
      std::list<boost::shared_ptr<Object> >::const_iterator it2 = objects.begin();
      for (; ((it1 != positions.end()) && (it2 != objects.end())); it1++, it2++)
      {
        if(!AddObjectAt(*it1, *it2))
        {
          throw PT_EX(InventoryException("Invalid position for AddObjectsAt!")); //TODO: undo possible adds?
        }
      }
    }

    bool GridInventory::AddObjectAt(const PositionRef& position, boost::shared_ptr<Object> object)
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
      Math::Rectangle bb = GetBB(newposU, curo->GetSize());
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
          Math::Rectangle bb2 = GetBB(curposU, newo->object->GetSize());
          if (!IsInInventory(bb2)) return false;
          if (bb.Intersects(bb2, true)) return false;
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
      curo->SetParent(this);
      if (newo)
      {
        if (!AddObjectAtImpl(curposU, newo)) throw PT_EX(InventoryException("Failed to add object for unknown reason!"));
        newo->SetParent(this);
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
      RemoveObject(object);
    }


  } // Inventory namespace
} // Common namespace
