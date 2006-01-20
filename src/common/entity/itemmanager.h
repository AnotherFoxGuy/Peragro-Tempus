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

#include <vector>

#include "item.h"
#include "server/database/table-items.h"

class ItemManager
{
private:
  std::vector<Item*> items;

public:
  ItemManager() {}

  size_t getItemCount() const
  {
    return items.size();
  }

  Item* getItem(size_t index) const
  {
    return items[index];
  }

  void addItem(Item* item)
  {
    items.push_back(item);
  }

  void delItem(size_t index)
  {
    items.erase(items.begin()+index);
  }

  void delItem(Item* item)
  {
    if (!item) 
    {
      printf("Removing item: Not a item!\n");
      return;
    }

    
    for (size_t i = 0; i < items.size(); i++)
    {
      Item* tmp_item = items[i];
      if (tmp_item->getName() == item->getName())
      {
        items.erase(items.begin()+i);
        printf("Removing item: Item removed!\n");
        return;
      }
    }
    printf("Removing item: Item not found!\n");
  }

  Item* findByName(const std::string& name) const
  {
    if (name.length() == 0) return 0;
    for (size_t i = 0; i < items.size(); i++)
    {
      Item* item = items[i];
      if (item->getName() == name)
      {
        return item;
      }
    }
    return 0;
  }

  bool exists(Item* item) const
  {
    if (!item) return false;
    std::string name = item->getName();
    if (name.length() == 0) return false;
    for (size_t i = 0; i < items.size(); i++)
    {
      Item* _item = items[i];

      if (_item->getId() == item->getId())
        return true;
    }
    return false;
  }

  Item* findById(int id) const
  {
    for (size_t i = 0; i < items.size(); i++)
    {
      Item* item = items[i];
      if (item->getId() == id)
      {
        return item;
      }
    }
    return 0;
  }

  void clear()
  {
    items.clear();
  }

  void loadFromDB(ItemTable* it)
  {
    //Load all Items from Database
    it->getAllItems(items);
  }
};

#endif // _ITEMMANAGER_H_

