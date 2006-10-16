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

#include "database.h"
#include "table-npcentities.h"

NpcEntitiesTable::NpcEntitiesTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from npcentities;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

NpcEntitiesTableVO* NpcEntitiesTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  NpcEntitiesTableVO* vo = new NpcEntitiesTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->character = atoi(rs->GetData(row,1).c_str());
  vo->ai = ptString(rs->GetData(row,2).c_str(), rs->GetData(row,2).length());
  return vo;
}

Array<NpcEntitiesTableVO*> NpcEntitiesTable::parseMultiResultSet(ResultSet* rs)
{
  Array<NpcEntitiesTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    NpcEntitiesTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}

void NpcEntitiesTable::createTable()
{
  printf("Creating Table npcentities...\n");
  db->update("create table npcentities ("
             "id INTEGER,"
             "character INTEGER,"
             "ai TEXT,"
             "PRIMARY KEY (Id) );");

  ptString ai("stray",5);
  insert(1, 1, ai);
  insert(6, 2, ai);
}

void NpcEntitiesTable::insert(int id, int character, ptString ai)
{
  const char* query = { "insert into npcentities(id, character, ai) values (%d, %d, '%q');" };
  db->update(query, id, character, *ai);
}

void NpcEntitiesTable::remove(int id)
{
  db->update("delete from npcentities where id = %d");
}

bool NpcEntitiesTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from npcentities where id = %d;", id);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

NpcEntitiesTableVO* NpcEntitiesTable::getById(int id)
{
  ResultSet* rs = db->query("select * from npcentities where id = %d;", id);

  if (!rs || rs->GetRowCount() == 0) return 0;

  return parseSingleResultSet(rs);
}

Array<NpcEntitiesTableVO*> NpcEntitiesTable::getAll()
{
  ResultSet* rs = db->query("select * from npcentities;");
  return parseMultiResultSet(rs);
}

