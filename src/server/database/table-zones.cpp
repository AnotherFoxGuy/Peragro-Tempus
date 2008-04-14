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

#include "database.h"
#include "table-zones.h"

ZonesTable::ZonesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from zones;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

ZonesTableVO* ZonesTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  if (!rs || rs->GetRowCount() <= row) return 0;

  ZonesTableVO* vo = new ZonesTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->coords.x = atof(rs->GetData(row,1).c_str());
  vo->coords.y = atof(rs->GetData(row,2).c_str());
  vo->type = atoi(rs->GetData(row,3).c_str());
  return vo;
}

Array<ZonesTableVO*> ZonesTable::parseMultiResultSet(ResultSet* rs)
{
  Array<ZonesTableVO*> arr;
  if (!rs)
  {
    return arr;
  }
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    ZonesTableVO* obj = parseSingleResultSet(rs, i);
    arr.add(obj);
  }
  return arr;
}

void ZonesTable::createTable()
{
  printf("Creating Table zones...\n");
  db->update("create table zones ("
             "id INTEGER, "
             "x FLOAT, "
             "y FLOAT, "
             "type INTEGER );");

  // Hard coded test zone (at the red sphere)
  ZonesTableVO zone(0,PtVector2(0.0f,0.0f),1/*water*/);
  insert(&zone);
  zone.coords.x=10.0f;
  insert(&zone);
  zone.coords.y=10.0f;
  insert(&zone);
  zone.coords.x=0.0f;
  insert(&zone);
}

void ZonesTable::dropTable()
{
  db->update("drop table zones;");
}

void ZonesTable::insert(ZonesTableVO* vo)
{
  const char* query = { "insert into zones(id, x, y, type) values (%d, %f, %f, %d);" };
  if (!vo)
  {
    return;
  }
  db->update(query, vo->id, vo->coords.x, vo->coords.y, vo->type);
}

void ZonesTable::remove(unsigned int id)
{
  db->update("delete from zones where id = %d", id);
}

unsigned int ZonesTable::getCount()
{
  ResultSet* rs = db->query("select count(*) from zones;");
  if (!rs)
  {
    return 0;
  }
  unsigned int count = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return count;
}

bool ZonesTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from zones where id = %d;", id);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

ZonesTableVO* ZonesTable::getById(int id)
{
  ResultSet* rs = db->query("select * from zones where id = %d;", id);
  ZonesTableVO* vo = parseSingleResultSet(rs);
  delete rs;
  return vo;
}

Array<ZonesTableVO*> ZonesTable::getAll()
{
  ResultSet* rs = db->query("select * from zones;");
  Array<ZonesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}
