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
  printf("Creating Table inventory...\n");
  db->update("create table inventory ("
    "id INTEGER, "
    "slot INTEGERT, "
    "item INTEGER, "
    "variation INTEGER, "
    "PRIMARY KEY (id, slot) );");

  //InventoryEntry e(3, 0);
  //set(1, e, 0, true);
}

void InventoryTable::set(int inventory, const InventoryEntry& item, int slot, bool add)
{
  if (add)
    db->update("insert into inventory (id, item, variation, slot) values ('%d','%d','%d','%d');",
      inventory, item.id, item.variation, slot);
  else
    db->update("delete from inventory where id=%d and item=%d and slot=%d;",
      inventory, item.id, slot);
}

void InventoryTable::dropTable()
{
  db->update("drop table inventory;");
}

void InventoryTable::getAllEntries(Array<InventoryEntry>& entries, int id)
{
  ResultSet* rs = db->query("select item, slot, variation from inventory where id = '%d';", id);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    entries.get(atoi(rs->GetData(i,1).c_str())).id = atoi(rs->GetData(i,0).c_str());
    entries.get(atoi(rs->GetData(i,1).c_str())).variation = atoi(rs->GetData(i,2).c_str());
  }
  delete rs;
}  
