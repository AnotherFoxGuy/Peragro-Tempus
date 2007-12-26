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
#include "table-meshes.h"

MeshesTable::MeshesTable(Database* db) : db(db)
{
  ResultSet* rs = db->query("select count(*) from meshes;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

MeshesTableVO* MeshesTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  MeshesTableVO* vo = new MeshesTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->sector = atoi(rs->GetData(row,1).c_str());
  vo->name = ptString(rs->GetData(row,2).c_str(), rs->GetData(row,2).length());
  return vo;
}

Array<MeshesTableVO*> MeshesTable::parseMultiResultSet(ResultSet* rs)
{
  Array<MeshesTableVO*> arr;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    MeshesTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}

void MeshesTable::createTable()
{
  printf("Creating Table meshes...\n");
  db->update("create table meshes ("
             "id INTEGER,"
             "sector INTEGER,"
             "name TEXT,"
             "PRIMARY KEY (Id) );");
}

void MeshesTable::insert(MeshesTableVO* vo)
{
  const char* query = { "insert into meshes(id, sector, name) values (%d, %d, '%s');" };
  db->update(query, vo->id, vo->sector, *vo->name);
}

void MeshesTable::remove(int id)
{
  db->update("delete from meshes where id = %d");
}

Array<MeshesTableVO*> MeshesTable::getAll()
{
  ResultSet* rs = db->query("select * from meshes;");
  if (!rs) return Array<MeshesTableVO*>();
  Array<MeshesTableVO*> vo = parseMultiResultSet(rs);
  delete rs;
  return vo;
}

