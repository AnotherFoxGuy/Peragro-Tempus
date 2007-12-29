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

#include "table-items.h"
#include "server/entity/item.h"

ItemTable::ItemTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from items;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void ItemTable::createTable()
{
  printf("Creating Table items...\n");
  db->update("create table items ("
    "id INTEGER, "
    "name TEXT, "
    "icon TEXT, "
    "description TEXT, "
    "file TEXT, "
    "mesh TEXT, "
    "weight INTEGER, "
    "equiptype TEXT, "
    "PRIMARY KEY (id) );");
}

void ItemTable::insert(ptString name, ptString icon, ptString description, ptString file, ptString mesh, unsigned int weight, ptString equiptype)
{
  db->update("insert into items (name, icon, description, file, mesh, weight, equiptype) values ('%q','%q','%q','%q','%q',%u,'%q');", *name, *icon, *description, *file, *mesh, weight, *equiptype);
}

void ItemTable::remove(int id)
{
  db->update("delete from items where id = %d;", id);
}

void ItemTable::dropTable()
{
  db->update("drop table items;");
}

bool ItemTable::existsItem(ptString name)
{
  ResultSet* rs = db->query("select id from items where name = '%q';", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Item* ItemTable::getItem(ptString name)
{
  ResultSet* rs = db->query("select * from items where name = '%q';", *name);

  Item* item = 0;

  if (rs && rs->GetRowCount() == 1) 
  {
    item = new Item();
    item->setId(atoi(rs->GetData(0,0).c_str()));
    item->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
    item->setIcon(ptString(rs->GetData(0,2).c_str(), rs->GetData(0,2).length()));
    item->setDescription(ptString(rs->GetData(0,3).c_str(), rs->GetData(0,3).length()));
    item->setFile(ptString(rs->GetData(0,4).c_str(), rs->GetData(0,4).length()));
    item->setMesh(ptString(rs->GetData(0,5).c_str(), rs->GetData(0,5).length()));
    item->setWeight(atoi(rs->GetData(0,6).c_str()));
    item->setEquiptype(ptString(rs->GetData(0,7).c_str(), rs->GetData(0,7).length()));
  }
  delete rs;
  return item;
}

Item* ItemTable::getItem(int id)
{
  ResultSet* rs = db->query("select * from items where id = %d;", id);

  Item* item = 0;

  if (rs && rs->GetRowCount() == 1) 
  {
    item = new Item();
    item->setId(atoi(rs->GetData(0,0).c_str()));
    item->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
    item->setIcon(ptString(rs->GetData(0,2).c_str(), rs->GetData(0,2).length()));
    item->setDescription(ptString(rs->GetData(0,3).c_str(), rs->GetData(0,3).length()));
    item->setFile(ptString(rs->GetData(0,4).c_str(), rs->GetData(0,4).length()));
    item->setMesh(ptString(rs->GetData(0,5).c_str(), rs->GetData(0,5).length()));
    item->setWeight(atoi(rs->GetData(0,6).c_str()));
    item->setEquiptype(ptString(rs->GetData(0,7).c_str(), rs->GetData(0,7).length()));
  }
  delete rs;
  return item;
}

void ItemTable::getAllItems(Array<Item*>& items)
{
  ResultSet* rs = db->query("select * from items;");
  if (!rs) return;

  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Item* item = new Item();
    item->setId(atoi(rs->GetData(i,0).c_str()));
    item->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
    item->setIcon(ptString(rs->GetData(i,2).c_str(), rs->GetData(i,2).length()));
    item->setDescription(ptString(rs->GetData(i,3).c_str(), rs->GetData(i,3).length()));
    item->setFile(ptString(rs->GetData(i,4).c_str(), rs->GetData(i,4).length()));
    item->setMesh(ptString(rs->GetData(i,5).c_str(), rs->GetData(i,5).length()));
    item->setWeight(atoi(rs->GetData(i,6).c_str()));
    item->setEquiptype(ptString(rs->GetData(i,7).c_str(), rs->GetData(i,7).length()));

/*    Item* item = new Item();
    item->setId(atoi(rs->GetData(i,0).c_str()));
    item->setName(ptString(rs->GetData(i,1).c_str(),rs->GetData(i,1).length()));
    item->setMesh(ptString(rs->GetData(i,2).c_str(),rs->GetData(i,2).length()));
    items.add(item);*/

    items.add(item);
  }
  delete rs;
}
