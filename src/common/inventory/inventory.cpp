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

#include "slotinventory.h"
#include "object.h"

namespace Common
{
  namespace Inventory
  {
    Inventory::Inventory(const std::string& name, Utils::Flags type, unsigned int rows, unsigned int columns)
    {
      inventoryName = name;
      inventoryType = type;
      inventoryRows = rows;
      inventoryColumns = columns;
    }

    Inventory::~Inventory()
    {
    }

    void Inventory::SetName(const std::string& name){ inventoryName = name; }

    const std::string& Inventory::GetName() const { return inventoryName; }

    unsigned int Inventory::GetRowCount() const { return inventoryRows; }

    unsigned int Inventory::GetColumnCount() const { return inventoryColumns; }

    bool Inventory::AllowsType(boost::shared_ptr<Object> object)
    {
      switch (object->GetType())
      {
      case Object::Item:
        {
          return inventoryType.Check(ALLOW_ITEMS);
        } break;
      case Object::Action:
        {
          return inventoryType.Check(ALLOW_ACTIONS);
        } break;
      default:
        break;
      }

      return false;
    }

    void Inventory::AddInventoryCallBack(InventoryCallBack* cb)
    {
      callback_list.remove(cb);
      callback_list.push_back(cb);
    }

    void Inventory::RemoveInventoryCallBack(InventoryCallBack* cb)
    {
      callback_list.remove(cb);
    }

    void Inventory::NotifyObjectAdded(boost::shared_ptr<Object> o, const PositionRef& r)
    {
      std::list<Common::Inventory::InventoryCallBack*>::const_iterator it;
      for ( it=callback_list.begin() ; it != callback_list.end(); it++ )
      {
        (*it)->ObjectAdded(o, r);
      }
    }

    void Inventory::NotifyObjectRemoved(boost::shared_ptr<Object> o, const PositionRef& r)
    {
      std::list<Common::Inventory::InventoryCallBack*>::const_iterator it;
      for ( it=callback_list.begin() ; it != callback_list.end(); it++ )
      {
        (*it)->ObjectRemoved(o, r);
      }
    }

    std::ostream& operator<< (std::ostream& os, const Inventory& i)
    {
      for (size_t r = 0; r < i.GetRowCount(); r++)
      {
        os << std::endl <<"-";
        for (size_t c = 0; c < i.GetColumnCount(); c++)
          os << "-----";
        os << std::endl;

        for (size_t c = 0; c < i.GetColumnCount(); c++)
        {
          boost::shared_ptr<Object> o = i.GetObjectAt(PositionRef(c, r));
          if (o) {os << "| "<<std::setw(2)<< *o.get() << " ";} else {os << "|    ";}
          if (c == i.GetColumnCount()-1) os << "|";
        }

        if (r == i.GetRowCount()-1)
        {
          os << std::endl <<"-";
          for (size_t c = 0; c < i.GetColumnCount(); c++)
            os << "-----";
          os << std::endl;
        }
      }
      return os;
    }

  } // Inventory namespace
} // Common namespace
