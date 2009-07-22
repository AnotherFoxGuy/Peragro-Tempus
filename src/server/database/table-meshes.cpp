/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "table-meshes.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

MeshesTable::MeshesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_MESHES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(101, 1, "test", "/peragro/art/3d_art/characters/male01/male01");
  }
  delete rs;
}

PT_DEFINE_CreateTable(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
PT_DEFINE_DropTable(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
PT_DEFINE_Insert(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
PT_DEFINE_ParseSingleResultSet(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
PT_DEFINE_ParseMultiResultSet(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
PT_DEFINE_GetAll(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)
PT_DEFINE_GetSingle(MeshesTable, DB_TABLE_MESHES, DB_TABLE_MESHES_FIELDS)

size_t MeshesTable::GetMaxId()
{
  ResultSet* rs = db->query("select max(id) from " PT_GetTableName(DB_TABLE_MESHES));
  if (rs == 0 || rs->GetRowCount() == 0)
    return 0;

  size_t id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

size_t MeshesTable::FindBy(const std::string& factoryName, const std::string& fileName)
{
  ResultSet* rs = db->query("select * from " PT_GetTableName(DB_TABLE_MESHES) " "
    "where factoryName='%s' AND fileName='%s';",
    factoryName.c_str(), fileName.c_str());
  std::vector<MeshesTableVOp> arr;
  if (!rs) return 0;
  arr = ParseMultiResultSet(rs);
  delete rs;
  if (arr.size() != 1)
    return 0;
  return arr[0]->id;
}

size_t MeshesTable::FindByName(const std::string& factoryName)
{
  ResultSet* rs = db->query("select * from " PT_GetTableName(DB_TABLE_MESHES) " "
    "where factoryName='%s';",
    factoryName.c_str());
  std::vector<MeshesTableVOp> arr;
  if (!rs) return 0;
  arr = ParseMultiResultSet(rs);
  delete rs;
  if (arr.size() != 1)
    return 0;
  return arr[0]->id;
}
