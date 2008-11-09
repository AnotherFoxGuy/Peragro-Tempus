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

#include <stdlib.h>

#include "common/database/database.h"
#include "table-zonenodes.h"

ZonenodesTable::ZonenodesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from zonenodes;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

ZonenodesTableVO* ZonenodesTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  if (!rs || rs->GetRowCount() <= row) return 0;

  ZonenodesTableVO* vo = new ZonenodesTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->x = atof(rs->GetData(row,1).c_str());
  vo->z = atof(rs->GetData(row,2).c_str());
  return vo;
}

Array<ZonenodesTableVO*> ZonenodesTable::parseMultiResultSet(ResultSet* rs)
{
  Array<ZonenodesTableVO*> arr;
  if (!rs)
  {
    return arr;
  }
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    ZonenodesTableVO* obj = parseSingleResultSet(rs, i);
    arr.add(obj);
  }
  return arr;
}

void ZonenodesTable::createTable()
{
  printf("Creating Table zonenodes...\n");
  db->update("create table zonenodes ("
             "id INTEGER, "
             "x FLOAT, "
             "z FLOAT );");

/*  // Hard coded test zone (at the red sphere)
  ZonenodesTableVO zone(0,0.0f,0.0f);
  insert(&zone);
  zone.x=10.0f;
  insert(&zone);
  zone.z=10.0f;
  insert(&zone);
  zone.x=0.0f;
  insert(&zone);*/
}

void ZonenodesTable::dropTable()
{
  db->update("drop table zonenodes;");
}

void ZonenodesTable::insert(ZonenodesTableVO* vo)
{
  const char* query = { "insert into zonenodes(id, x, z) values (%d, %f, %f);" };
  if (!vo)
  {
    return;
  }
  db->update(query, vo->id, vo->x, vo->z);
}

void ZonenodesTable::remove(unsigned int id)
{
  db->update("delete from zonenodes where id = %d", id);
}

void ZonenodesTable::removeAll()
{
  db->update("delete from zonenodes");
}

unsigned int ZonenodesTable::getCount()
{
  ResultSet* rs = db->query("select count(*) from zonenodes;");
  if (!rs)
  {
    return 0;
  }
  unsigned int count = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return count;
}

bool ZonenodesTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from zonenodes where id = %d;", id);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Array<ZonenodesTableVO*> ZonenodesTable::getById(int id)
{
  ResultSet* rs = db->query("select * from zonenodes where id = %d;", id);
  Array<ZonenodesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

Array<ZonenodesTableVO*> ZonenodesTable::getAll()
{
  ResultSet* rs = db->query("select * from zonenodes;");
  Array<ZonenodesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}
