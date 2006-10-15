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

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "database.h"

#include "table-inventory.h"

#include "server/entity/inventory.h"

InventoryTable::InventoryTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from inventory;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void InventoryTable::createTable()
{
  db->update("create table inventory ("
    "id INTEGER, "
    "slot INTEGERT, "
    "item INTEGER, "
    "amount INTEGER, "
    "PRIMARY KEY (id, item, slot) );");
}

void InventoryTable::set(int inventory, Item* item, int amount, int slot)
{
  if (amount == 0)
    db->update("delete from inventory where id=%d and item=%d and slot=%d;",
      inventory, item->getId(), slot);
  else
    db->update("insert or replace into inventory (id, item, slot, amount) values ('%d','%d','%d','%d');",
      inventory, item->getId(), slot, amount);
}

int InventoryTable::get(int inventory, Item* item)
{
  ResultSet* rs = db->query("select * from inventory where id = '%d' and item = '%d';",
    inventory, item->getId());

  int amount = 0;

  if (rs->GetRowCount() == 1)
  {
    amount = atoi(rs->GetData(0,0).c_str());
  }
  else if (rs->GetRowCount() == 0)
  {
    amount = 0;
  }
  else
  {
    printf("DB inconsistency: multiple equal items in the inventory!");
  }

  delete rs;

  return amount;
}

void InventoryTable::dropTable()
{
  db->update("drop table inventory;");
}

void InventoryTable::getAllEntries(Array<InvEntries*>& entries, int id)
{
  ResultSet* rs = db->query("select item, amount, slot from inventory where id = '%d';", id);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    InvEntries* entry = new InvEntries();
    int item_id = atoi(rs->GetData(i,0).c_str());
    entry->item_id = item_id;
    entry->amount = atoi(rs->GetData(i,1).c_str());
    entry->slot =  atoi(rs->GetData(i,2).c_str());
    entries.add(entry);
  }
  delete rs;
}  
