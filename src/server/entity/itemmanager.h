/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef _ITEMMANAGER_H_
#define _ITEMMANAGER_H_

#include "common/util/array.h"
#include "item.h"
#include "server/server.h"
#include "server/database/database.h"
#include "server/database/table-items.h"

class ItemManager
{
private:
  Array<Item*> items;

public:
  ItemManager() {}
  ~ItemManager() { items.delAll(); }

  size_t getItemCount()
  {
    return items.getCount();
  }

  Item* getItem(size_t index)
  {
    return items.get(index);
  }

  void addItem(Item* item)
  {
    items.add(item);
  }

  void delItem(size_t index)
  {
    ItemTable* it = Server::getServer()->getDatabase()->getItemTable();
    Item* item = getItem(index);
    items.remove(index);
    it->remove(item->getId());
  }

  void delItem(Item* item)
  {
    if (!item) return;
    for (size_t i = 0; i<items.getCount(); i++)
    {
      Item* _item = items.get(i);
      if (_item->getId() == item->getId())
      {
        items.remove(i);
        return;
      }
    }
    ItemTable* it = Server::getServer()->getDatabase()->getItemTable();
    it->remove(item->getId());
  }

  bool exists(Item* item)
  {
    if (!item) return false;
    for (size_t i = 0; i<items.getCount(); i++)
    {
      Item* _item = items.get(i);

      if (_item->getId() == item->getId())
        return true;
    }
    return false;
  }

  Item* findByName(ptString name)
  {
    if (name == ptString::Null) return 0;
    for (size_t i = 0; i<items.getCount(); i++)
    {
      Item* item = items.get(i);
      if (item->getName() == name)
      {
        return item;
      }
    }
    return 0;
  }

  Item* findById(unsigned int id)
  {
    if ( id == 0 ) return 0;
    for (size_t i = 0; i<items.getCount(); i++)
    {
      Item* item = items.get(i);
      if (item->getId() == id)
      {
        return item;
      }
    }
    return 0;
  }

  void loadFromDB(ItemTable* it)
  {
    //Load all Items from Database
    Array<Item*> loadedItems;
    it->getAllItems(loadedItems);
    for (size_t i = 0; i<loadedItems.getCount(); i++)
    {
      addItem(loadedItems[i]);
    }
  }

  void clear()
  {
    items.removeAll();
  }
};

#endif // _ITEMMANAGER_H_
