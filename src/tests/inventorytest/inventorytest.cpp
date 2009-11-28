/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <istream>

#include "common/inventory/item.h"
#include "common/inventory/slotinventory.h"
#include "common/inventory/gridinventory.h"

using namespace Common::Inventory;

typedef std::list<boost::shared_ptr<Object> > Objects;

struct CallBack : public InventoryCallBack
{
  virtual void ObjectAdded(boost::shared_ptr<Object> o, const PositionRef& r)
  {
    std::cout << "Added: " << *o.get() << " at " << r.column<<","<< r.row<< std::endl;
  }
  virtual void ObjectRemoved(boost::shared_ptr<Object> o, const PositionRef& r)
  {
    std::cout << "Removed: " << *o.get() << " at " << r.column<<","<< r.row<< std::endl;
  }
};

#include "slotinventory.h"

#include "gridinventory.h"

int main()
{
  //TestSlotInventory();
  //TestGridInventory();

  return 0;
}
