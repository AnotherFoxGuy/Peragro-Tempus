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

#include "table-entitytypes.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

EntityTypesTable::EntityTypesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(PT_DB_TABLE_ENTITYTYPES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(1, "PCEntityType");
    Insert(2, "NPCEntityType");
    Insert(3, "DoorEntityType");
    Insert(4, "ItemEntityType");
    Insert(5, "MountEntityType");
  }
  delete rs;
}

PT_DEFINE_CreateTable(EntityTypesTable, PT_DB_TABLE_ENTITYTYPES, PT_DB_TABLE_ENTITYTYPES_FIELDS)
PT_DEFINE_DropTable(EntityTypesTable, PT_DB_TABLE_ENTITYTYPES, PT_DB_TABLE_ENTITYTYPES_FIELDS)
PT_DEFINE_Insert(EntityTypesTable, PT_DB_TABLE_ENTITYTYPES, PT_DB_TABLE_ENTITYTYPES_FIELDS)
PT_DEFINE_ParseSingleResultSet(EntityTypesTable, PT_DB_TABLE_ENTITYTYPES, PT_DB_TABLE_ENTITYTYPES_FIELDS)
PT_DEFINE_ParseMultiResultSet(EntityTypesTable, PT_DB_TABLE_ENTITYTYPES, PT_DB_TABLE_ENTITYTYPES_FIELDS)
PT_DEFINE_GetAll(EntityTypesTable, PT_DB_TABLE_ENTITYTYPES, PT_DB_TABLE_ENTITYTYPES_FIELDS)
