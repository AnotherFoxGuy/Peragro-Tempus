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

#include <sstream>

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "database.h"

#include "table-items.h"
#include "common/entity/item.h"

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
  db->update("create table items ("
    "id INTEGER, "
    "name TEXT, "
    "mesh TEXT, "
    "PRIMARY KEY (id) );");

  //Example Data!
  insert("apple", "apple");
}

void ItemTable::insert(const std::string& name, const std::string& mesh)
{
  std::ostringstream query;
  query << "insert into items (name, mesh) values ('"
        << db->escape(name)
        << "','"
        << db->escape(mesh)
        << "');";
  db->update(query.str());
}

void ItemTable::dropTable()
{
  db->update("drop table items;");
}

bool ItemTable::existsItem(const std::string& name)
{
  std::ostringstream query;
  query << "select id from items where name = '"
        << db->escape(name)
        << "';";
  ResultSet* rs = db->query(query.str());
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Item* ItemTable::getItem(const std::string& name)
{
  std::ostringstream query;
  query << "select * from users where name = '"
        << db->escape(name)
        << "';";
  ResultSet* rs = db->query(query.str());

  Item* item = 0;

  if (rs && rs->GetRowCount() == 1) 
  {
    /*
    Item* item = new Item();
    item->setId(atoi(rs->GetData(0,0).c_str()));
    item->setName(rs->GetData(0,1).c_str(), rs->GetData(0,1).length());
    */
  }
  delete rs;
  return item;
}

void ItemTable::getAllItems(std::vector<Item*>& items)
{
  ResultSet* rs = db->query("select * from items;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Item* item = new Item();
    item->setId(atoi(rs->GetData(i,0).c_str()));
    item->setName(rs->GetData(i,1));
    item->setMesh(rs->GetData(i,2));
    items.push_back(item);
  }
}  
