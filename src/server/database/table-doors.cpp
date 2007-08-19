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
#include "table-doors.h"

DoorsTable::DoorsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from doors;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
  ResultSet* rs2 = db->query("select count(*) from doors;");
  delete rs2;
}

DoorsTableVO* DoorsTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  if (rs == 0 || rs->GetRowCount() != 1) return 0;

  DoorsTableVO* vo = new DoorsTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->name = ptString(rs->GetData(row,1).c_str(), rs->GetData(row,1).length());
  vo->islocked = atoi(rs->GetData(row,2).c_str());
  vo->isopen = atoi(rs->GetData(row,3).c_str());
  return vo;
}

Array<DoorsTableVO*> DoorsTable::parseMultiResultSet(ResultSet* rs)
{
  Array<DoorsTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    DoorsTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
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
             "PRIMARY KEY (Id) );");

  DoorsTableVO vo;
  vo.id = 1;
  vo.name = ptString("North Gate", strlen("North Gate"));
  vo.isopen = false;
  vo.islocked = true;

  insert(&vo);

  vo.id = 2;
  vo.name = ptString("Door", strlen("Door"));
  vo.isopen = false;
  vo.islocked = false;

  insert(&vo);
}

void DoorsTable::dropTable()
{
  db->update("drop table doors;");
}

void DoorsTable::insert(DoorsTableVO* vo)
{
  const char* query = { "insert into doors(id, name, islocked, isopen) values (%d, '%s', %d, %d);" };
  db->update(query, vo->id, *vo->name, vo->islocked, vo->isopen);
}

void DoorsTable::remove(int id)
{
  db->update("delete from doors where id = %d");
}

unsigned int DoorsTable::getCount()
{
  ResultSet* rs = db->query("select count(*) from books;");
  unsigned int count = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return count;
}

bool DoorsTable::existsByName(ptString name)
{
  ResultSet* rs = db->query("select * from doors where name like '%s';", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

bool DoorsTable::existsById(int id)
{
  ResultSet* rs = db->query("select * from doors where id = %d;", id);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

DoorsTableVO* DoorsTable::getByName(ptString name)
{
  ResultSet* rs = db->query("select * from doors where name like '%s';", *name);
  return parseSingleResultSet(rs);
}

DoorsTableVO* DoorsTable::getById(int id)
{
  ResultSet* rs = db->query("select * from doors where id = %d;", id);
  return parseSingleResultSet(rs);
}

Array<DoorsTableVO*> DoorsTable::getAll()
{
  ResultSet* rs = db->query("select * from doors;");
  return parseMultiResultSet(rs);
}

