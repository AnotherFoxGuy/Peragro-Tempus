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

#include "table-avatartemplates.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

AvatarTemplatesTable::AvatarTemplatesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->Query("select count(*) from " PT_GetTableName(DB_TABLE_AVATARTEMPLATES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(1,101,1,"Human Knight");
  }
  delete rs;
}

PT_DEFINE_CreateTable(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_DropTable(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_Insert(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_Delete(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_ParseSingleResultSet(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_ParseMultiResultSet(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_GetAll(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_Get(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)
PT_DEFINE_GetSingle(AvatarTemplatesTable, DB_TABLE_AVATARTEMPLATES, DB_TABLE_AVATARTEMPLATES_FIELDS)

AvatarTemplatesTableVOArray AvatarTemplatesTable::GetAvatarList()
{
  ResultSet* rs = db->Query(
    "SELECT * " \
    "FROM Meshes INNER JOIN avatartemplates ON Meshes.Id=avatartemplates.mesh_id; ");

  AvatarTemplatesTableVOArray arr;
  if (!rs) return arr;
  arr = ParseMultiResultSet(rs);
  delete rs;
  return arr;
}

