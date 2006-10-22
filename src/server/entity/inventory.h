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

class Connection;

class Inventory
{
public:
  static const unsigned char NoSlot = 255;

private:
  Array<unsigned int> entries; // contains item_id

  InventoryTable* invtab;

  int inv_id;

  unsigned char getItemCount()
  {
    unsigned char amount = 0;
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i) != Item::NoItem)
      {
        amount++;
      }
    }
    return amount;
  }

public:
  Inventory(unsigned char slots) : invtab(0), entries(slots, 0)
  {
    unsigned int item = Item::NoItem;
    for(unsigned char i = 0; i < slots; i++)
    {
      entries.add(item);
    }
  }
  ~Inventory() {}

  unsigned char getSlot(unsigned int item_id)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i) == item_id)
      {
        return i;
      }
    }
    return NoSlot; // no slot!
  }

  unsigned char getFreeSlot()
  {
    for(unsigned int i=10; i<entries.getCount(); i++)
    {
      if (entries.get(i) == 0)
      {
        return i;
      }
    }
    return NoSlot; // no slot!
  }

  bool addItem(unsigned int item_id)
  {
    unsigned char slot = getFreeSlot();

    if (slot == NoSlot) // no free slots!
      return false;

    return addItem(item_id, slot);
  }

  bool addItem(unsigned int item_id, unsigned char slot)
  {
    if (slot >= entries.getCount()) return false; // invalid slot;

    unsigned int& item = entries.get(slot);
    if (item == Item::NoItem)
    {
      item = item_id;
      invtab->set(inv_id, item, slot, true);
      return true;
    }
    return false; //slot occupied
  }

  bool takeItem(unsigned int slot)
  {
    if (slot >= entries.getCount()) return false; // invalid slot;

    unsigned int& item = entries.get(slot);
    if (item == Item::NoItem)
      return false;

    item = Item::NoItem;
    if (invtab) invtab->set(inv_id, item, slot, false);

    return true;
  }

  unsigned int getAmount(unsigned int slot)
  {
    if (slot >= entries.getCount()) return false; // invalid slot;

    unsigned int& item = entries.get(slot);
    if (item == Item::NoItem)
      return false;

    return 1;
  }

  unsigned int getAmount(Item* item, unsigned int slot)
  {
    if (slot >= entries.getCount()) return false; // invalid slot;

    unsigned int& item_id = entries.get(slot);
    if (item_id != item->getId())
      return false;

    return 1;
  }

  unsigned int getTotalAmount(int item_id)
  {
    unsigned int amount = 0;
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i) == item_id)
      {
        amount++;
      }
    }
    return amount;
  }

  unsigned int getItemId(unsigned char slot) const
  {
    if (slot >= entries.getCount()) return false; // invalid slot;
    return entries.get(slot);
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
