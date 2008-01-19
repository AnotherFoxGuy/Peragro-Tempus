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
#include "table-vertices.h"

VerticesTable::VerticesTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from vertices;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

VerticesTableVO* VerticesTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  VerticesTableVO* vo = new VerticesTableVO();
  vo->mesh = atoi(rs->GetData(row,0).c_str());
  vo->num = atoi(rs->GetData(row,1).c_str());
  vo->x = (float) atof(rs->GetData(row,2).c_str());
  vo->y = (float) atof(rs->GetData(row,3).c_str());
  vo->z = (float) atof(rs->GetData(row,4).c_str());
  return vo;
}

Array<VerticesTableVO*> VerticesTable::parseMultiResultSet(ResultSet* rs)
{
  Array<VerticesTableVO*> arr(rs->GetRowCount());
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    VerticesTableVO* obj = parseSingleResultSet(rs, i);
    arr.add(obj);
  }
  return arr;
}

void VerticesTable::createTable()
{
  printf("Creating Table vertices...\n");
  db->update("create table vertices ("
             "mesh INTEGER,"
             "num INTEGER,"
             "x FLOAT,"
             "y FLOAT,"
             "z FLOAT,"
             "PRIMARY KEY (Mesh, Num) );");
}

void VerticesTable::insert(VerticesTableVO* vo)
{
  const char* query = { "insert into vertices(mesh, num, x, y, z) values (%d, %d, %.2f, %.2f, %.2f);" };
  db->update(query, vo->mesh, vo->num, vo->x, vo->y, vo->z);
}

void VerticesTable::remove(int mesh, int num)
{
  db->update("delete from vertices where mesh = %d and num = %d", mesh, num);
}

Array<VerticesTableVO*> VerticesTable::getAll()
{
  ResultSet* rs = db->query("select * from vertices;");
  Array<VerticesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

Array<VerticesTableVO*> VerticesTable::getAllByMesh(int mesh)
{
  ResultSet* rs = db->query("select * from vertices where mesh = %d;", mesh);
  Array<VerticesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

