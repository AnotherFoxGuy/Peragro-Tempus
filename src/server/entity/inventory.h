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
  unsigned int item_id;
  int amount;
  unsigned int slot;
};

class Connection;

class Inventory
{
private:
  Array<InvEntries*> entries;

  InvEntries* findEntryByItem(int item_id)
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

  InvEntries* findEntryBySlot(int slot)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i)->slot == slot)
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

  int getSlot(Item* item)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i)->item_id == item->getId() && entries.get(i)->slot >= 10)
      {
        return entries.get(i)->slot;
      }
    }
    return -1;
  }

  int getFreeSlot()
  {
    for (int i = 10; i<30; i++)
    {
      InvEntries* entry = findEntryBySlot(i);
      if (!entry) 
        return i;
    }
    return -1;
  }

  void addItem(Item* item, int amount, int slot)
  {
    InvEntries* entry = findEntryBySlot(slot);
    if (!entry)
    {
      entry = new InvEntries();
      entry->item_id = item->getId();
      entry->amount = amount;
      entry->slot = slot;
      entries.add(entry);
    }
    else if (entry->item_id == item->getId())
    {
      entry->amount += amount;
    }
    else
    {
      // item ids are different
      return;
    }
    if (invtab) invtab->set(inv_id, item, entry->amount, entry->slot);
  }

  bool takeItem(Item* item, unsigned int amount, unsigned int slot)
  {
    if (slot >= 30) return false; // invalid slot;
    InvEntries* entry = findEntryBySlot(slot);
    if (!entry || entry->item_id != item->getId() || entry->amount < (int) amount)
      return false;

    entry->amount -= amount;

    if (invtab) invtab->set(inv_id, item, entry->amount, entry->slot);

    return true;
  }

  unsigned int getAmount(unsigned int slot)
  {
    if (slot >= 30) return false; // invalid slot;
    InvEntries* entry = findEntryBySlot(slot);
    if (!entry)
      return 0;

    return entry->amount;
  }

  unsigned int getAmount(Item* item, unsigned int slot)
  {
    if (slot >= 30) return false; // invalid slot;
    InvEntries* entry = findEntryBySlot(slot);
    if (!entry || entry->item_id != item->getId())
      return 0;

    return entry->amount;
  }

  unsigned int getItemIdFromSlot(int slot)
  {
    InvEntries* entry = findEntryBySlot(slot);
    return (entry?entry->item_id:-1);
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
