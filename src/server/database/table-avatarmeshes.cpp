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

#include "table-avatarmeshes.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

AvatarMeshesTable::AvatarMeshesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_AVATARMESHES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(1,101,1,"Human Knight");
  }
  delete rs;
}

PT_DEFINE_CreateTable(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_DropTable(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_Insert(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_Delete(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_ParseSingleResultSet(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_ParseMultiResultSet(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_GetAll(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_Get(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)
PT_DEFINE_GetSingle(AvatarMeshesTable, DB_TABLE_AVATARMESHES, DB_TABLE_AVATARMESHES_FIELDS)

AvatarMeshesTableVOArray AvatarMeshesTable::GetAvatarList()
{
  ResultSet* rs = db->query(
    "SELECT * " \
    "FROM Meshes INNER JOIN avatarmeshes ON Meshes.Id=avatarmeshes.mesh_id; ");

  AvatarMeshesTableVOArray arr; 
  if (!rs) return arr;
  arr = ParseMultiResultSet(rs);
  delete rs;
  return arr;
}

