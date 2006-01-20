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

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "item.h"
#include "server/database/table-inventory.h"

class InvEntries
{
public:
  int item_id;
  int amount;
};

class Connection;

class Inventory
{
private:
  Array<InvEntries*> entries;

  InvEntries* findEntry(int item_id)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i)->item_id == item_id)
      {
        return entries.get(i);
      }
    }
    return 0;
  }

  InventoryTable* invtab;

  int inv_id;

public:
  Inventory() : invtab(0) {}
  ~Inventory() {}

  void addItem(Item* item, int amount)
  {
    InvEntries* entry = findEntry(item->getId());
    if (!entry)
    {
      entry = new InvEntries();
      entry->item_id = item->getId();
      entry->amount = amount;
      entries.add(entry);
    }
    else
    {
      entry->amount += amount;
    }
    if (invtab) invtab->set(inv_id, item, entry->amount);
  }

  bool takeItem(Item* item, int amount)
  {
    InvEntries* entry = findEntry(item->getId());
    if (!entry || entry->amount < amount)
      return false;

    entry->amount -= amount;

    if (invtab) invtab->set(inv_id, item, entry->amount);

    return true;
  }

  unsigned int getAmount(Item* item)
  {
    InvEntries* entry = findEntry(item->getId());
    return (entry?entry->amount:0);
  }

  void loadFromDatabase(InventoryTable* it, int id)
  {
    inv_id = id;
    invtab = it;

    //Load all Items from Database
    it->getAllEntries(entries, id);
  }

  void sendAllItems(Connection* conn);
};

#endif // INVENTORY_H_
