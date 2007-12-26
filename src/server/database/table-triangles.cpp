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
#include "table-triangles.h"

TrianglesTable::TrianglesTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from triangles;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

TrianglesTableVO* TrianglesTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  TrianglesTableVO* vo = new TrianglesTableVO();
  vo->mesh = atoi(rs->GetData(row,0).c_str());
  vo->num = atoi(rs->GetData(row,1).c_str());
  vo->a = atoi(rs->GetData(row,2).c_str());
  vo->b = atoi(rs->GetData(row,3).c_str());
  vo->c = atoi(rs->GetData(row,4).c_str());
  return vo;
}

Array<TrianglesTableVO*> TrianglesTable::parseMultiResultSet(ResultSet* rs)
{
  Array<TrianglesTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    TrianglesTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}

void TrianglesTable::createTable()
{
  printf("Creating Table triangles...\n");
  db->update("create table triangles ("
             "mesh INTEGER,"
             "num INTEGER,"
             "a INTEGER,"
             "b INTEGER,"
             "c INTEGER,"
             "PRIMARY KEY (Mesh, Num) );");
}

void TrianglesTable::insert(TrianglesTableVO* vo)
{
  const char* query = { "insert into triangles(mesh, num, a, b, c) values (%d, %d, %d, %d, %d);" };
  db->update(query, vo->mesh, vo->num, vo->a, vo->b, vo->c);
}

void TrianglesTable::remove(int mesh, int num)
{
  db->update("delete from triangles where mesh = %d and num = %d");
}

Array<TrianglesTableVO*> TrianglesTable::getAll()
{
  ResultSet* rs = db->query("select * from triangles;");
  Array<TrianglesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

Array<TrianglesTableVO*> TrianglesTable::getAllByMesh(int mesh)
{
  ResultSet* rs = db->query("select * from triangles where mesh = %d;", mesh);
  Array<TrianglesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

