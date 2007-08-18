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

#ifndef _TABLE_INVENTORY_H_
#define _TABLE_INVENTORY_H_

#include "common/util/array.h"
#include "table.h"

class Database;
class Item;
class Entry;

class InventoryEntry
{
public:
  unsigned int id;
  unsigned int variation;

  InventoryEntry() {}
  InventoryEntry(unsigned int id, unsigned int variation)
  : id(id), variation(variation)
  {
  }
};

class InventoryTable : public Table
{
public:
  InventoryTable(Database* db);
  void createTable();
  void set(int inventory, const InventoryEntry& item, int slot, bool add);
  void dropTable();
  void getAllEntries(Array<InventoryEntry>& entries, int id);
};

#endif //_TABLE_INVENTORY_H_
