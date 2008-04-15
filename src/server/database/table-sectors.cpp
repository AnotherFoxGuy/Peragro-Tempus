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
#include "table-sectors.h"

SectorsTable::SectorsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from sectors;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

SectorsTableVO* SectorsTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  if (rs == 0 || rs->GetRowCount() <= row) return 0;

  SectorsTableVO* vo = new SectorsTableVO();
  vo->id = (unsigned short) atoi(rs->GetData(row,0).c_str());
  vo->name = ptString(rs->GetData(row,1).c_str(), rs->GetData(row,1).length());
  vo->region = ptString(rs->GetData(row,2).c_str(), rs->GetData(row,2).length());
  return vo;
}

Array<SectorsTableVO*> SectorsTable::parseMultiResultSet(ResultSet* rs)
{
  Array<SectorsTableVO*> arr;
  if (!rs)
  {
    return arr;
  }
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    SectorsTableVO* obj = parseSingleResultSet(rs, i);
    if (obj)
    {
      arr.add(obj);
    }
  }
  return arr;
}

void SectorsTable::createTable()
{
  printf("Creating Table sectors...\n");
  db->update("create table sectors ("
             "id INTEGER,"
             "name TEXT,"
             "region TEXT,"
             "PRIMARY KEY (Id) );");
}

void SectorsTable::dropTable()
{
  db->update("drop table sectors;");
}

void SectorsTable::insert(SectorsTableVO* vo)
{
  const char* query = { "insert into sectors(id, name, region) values (%d, '%s', '%s');" };

  if (!vo)
  {
    return;
  }

  db->update(query, vo->id, *vo->name, *vo->region);
}

void SectorsTable::remove(unsigned short id)
{
  db->update("delete from sectors where id = %d", id);
}

unsigned int SectorsTable::getCount()
{
  ResultSet* rs = db->query("select count(*) from sectors;");
  if (!rs)
  {
    return 0;
  }
  unsigned int count = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return count;
}

bool SectorsTable::existsByName(ptString name)
{
  ResultSet* rs = db->query("select * from sectors where name like '%s';", *name);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

bool SectorsTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from sectors where id = %d;", id);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

SectorsTableVO* SectorsTable::getByName(ptString name)
{
  ResultSet* rs = db->query("select * from sectors where name like '%s';", *name);
  SectorsTableVO* vo = parseSingleResultSet(rs);
  delete rs;
  return vo;
}

SectorsTableVO* SectorsTable::getById(int id)
{
  ResultSet* rs = db->query("select * from sectors where id = %d;", id);
  SectorsTableVO* vo = parseSingleResultSet(rs);
  delete rs;
  return vo;
}

Array<SectorsTableVO*> SectorsTable::getAll()
{
  ResultSet* rs = db->query("select * from sectors;");
  Array<SectorsTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}
