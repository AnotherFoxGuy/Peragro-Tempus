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
  vo->dialog = atoi(rs->GetData(row,3).c_str());
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
             "entity_id INTEGER,"
             "character INTEGER,"
             "ai TEXT,"
             "dialog INTEGER,"
             "PRIMARY KEY (entity_id) );");

  ptString idle("idle",4);
  ptString stray("stray",5);

  insert(1, 1, stray, 0);
  insert(2, 2, stray, 9);
  insert(3, 3, idle, 6);

  // Creating Undead Squad.
  for (int i = 0; i < 100; i++)
  {
    insert(4 + i, 4 + i, idle, -1);
  }
}

void NpcEntitiesTable::insert(int id, int character, ptString ai, int dialog_id)
{
  const char* query = { "insert into npcentities(entity_id, character, ai, dialog) values (%d, %d, '%q', %d);" };
  db->update(query, id, character, *ai, dialog_id);
}

void NpcEntitiesTable::remove(int id)
{
  db->update("delete from npcentities where entity_id = %d");
}

bool NpcEntitiesTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from npcentities where entity_id = %d;", id);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

NpcEntitiesTableVO* NpcEntitiesTable::getById(int id)
{
  ResultSet* rs = db->query("select * from npcentities where entity_id = %d;", id);

  if (!rs || rs->GetRowCount() == 0) return 0;

  NpcEntitiesTableVO* vo = parseSingleResultSet(rs);
  delete rs;
  return vo;
}

Array<NpcEntitiesTableVO*> NpcEntitiesTable::getAll()
{
  ResultSet* rs = db->query("select * from npcentities;");
  Array<NpcEntitiesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

