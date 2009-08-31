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

#include "table-resourcetypes.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

ResourceTypesTable::ResourceTypesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->Query("select count(*) from " PT_GetTableName(DB_TABLE_RESOURCETYPES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(1, "Hit Points", 2, 1); // 2 == Endurance
    Insert(2, "Stamina", 2, 10); // 2 == Endurance
    Insert(3, "Willpower", 5, 10); // 5 == Resolve
  }
  delete rs;
}

PT_DEFINE_CreateTable(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
PT_DEFINE_DropTable(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
PT_DEFINE_Insert(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
PT_DEFINE_ParseSingleResultSet(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
PT_DEFINE_ParseMultiResultSet(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
PT_DEFINE_GetAll(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
//PT_DEFINE_Get(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
//PT_DEFINE_GetSingle(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)

