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
#include "table-doors.h"

DoorsTable::DoorsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from doors;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

DoorsTableVO* DoorsTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  if (rs == 0 || rs->GetRowCount() <= row) return 0;

  DoorsTableVO* vo = new DoorsTableVO();
  vo->id = (unsigned short) atoi(rs->GetData(row,0).c_str());
  vo->name = ptString(rs->GetData(row,1).c_str(), rs->GetData(row,1).length());
  vo->islocked = (unsigned char) atoi(rs->GetData(row,2).c_str());
  vo->isopen = (unsigned char) atoi(rs->GetData(row,3).c_str());
  vo->sector = ptString(rs->GetData(row,4).c_str(), rs->GetData(row,4).length());
  vo->mesh = ptString(rs->GetData(row,5).c_str(), rs->GetData(row,5).length());
  vo->animation = ptString(rs->GetData(row,6).c_str(), rs->GetData(row,6).length());
  vo->x = (float) atof(rs->GetData(row,7).c_str());
  vo->y = (float) atof(rs->GetData(row,8).c_str());
  vo->z = (float) atof(rs->GetData(row,9).c_str());
  return vo;
}

Array<DoorsTableVO*> DoorsTable::parseMultiResultSet(ResultSet* rs)
{
  Array<DoorsTableVO*> arr;
  if (!rs)
  {
    return arr;
  }
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    DoorsTableVO* obj = parseSingleResultSet(rs, i);
    if (obj)
    {
      arr.add(obj);
    }
  }
  return arr;
}

void DoorsTable::createTable()
{
  printf("Creating Table doors...\n");
  db->update("create table doors ("
             "id INTEGER,"
             "name TEXT,"
             "islocked INTEGER,"
             "isopen INTEGER,"
             "sector TEXT,"
             "mesh TEXT,"
             "animation TEXT,"
             "x FLOAT,"
             "y FLOAT,"
             "z FLOAT,"
             "PRIMARY KEY (Id) );");
}

void DoorsTable::dropTable()
{
  db->update("drop table doors;");
}

void DoorsTable::insert(DoorsTableVO* vo)
{
  const char* query = { "insert into doors(id, name, islocked, isopen, sector, mesh, animation, x, y, z) values (%d, '%q', %d, %d, '%q', '%q', '%q', %.2f, %.2f, %.2f);" };

  if (!vo)
  {
    return;
  }

  db->update(query, vo->id, *vo->name, vo->islocked, vo->isopen, *vo->sector, *vo->mesh, *vo->animation, vo->x, vo->y, vo->z);
}

void DoorsTable::remove(int id)
{
  db->update("delete from doors where id = %d", id);
}

unsigned int DoorsTable::getCount()
{
  ResultSet* rs = db->query("select count(*) from doors;");
  if (!rs)
  {
    return 0;
  }
  unsigned int count = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return count;
}

bool DoorsTable::existsByName(ptString name)
{
  ResultSet* rs = db->query("select * from doors where name like '%s';", *name);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

bool DoorsTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from doors where id = %d;", id);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

DoorsTableVO* DoorsTable::getByName(ptString name)
{
  ResultSet* rs = db->query("select * from doors where name like '%s';", *name);
  DoorsTableVO* vo = parseSingleResultSet(rs);
  delete rs;
  return vo;
}

DoorsTableVO* DoorsTable::getById(int id)
{
  ResultSet* rs = db->query("select * from doors where id = %d;", id);
  DoorsTableVO* vo = parseSingleResultSet(rs);
  delete rs;
  return vo;
}

Array<DoorsTableVO*> DoorsTable::getAll()
{
  ResultSet* rs = db->query("select * from doors;");
  Array<DoorsTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

