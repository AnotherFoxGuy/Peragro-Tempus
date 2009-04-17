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

#include "table-attributetypes.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

AttributeTypesTable::AttributeTypesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_ATTRIBUTETYPES) ";");
  if (rs == 0)
  {
    CreateTable();
    // General.
    Insert(1, "Quality");
    Insert(1, "Durability");
    Insert(1, "Weight");
    // Weapons.
    Insert(1, "Reach");
    Insert(1, "Damage");
    Insert(1, "Heft");
    // Ranged Weapons.
    Insert(1, "Pull");
    Insert(1, "Range");
    // Armor.
    Insert(1, "Mobility");
    Insert(1, "Hardness");
    Insert(1, "Bulk");
    Insert(1, "Toughness");

  }
  delete rs;
}

PT_DEFINE_CreateTable(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
PT_DEFINE_DropTable(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
PT_DEFINE_ParseSingleResultSet(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
PT_DEFINE_ParseMultiResultSet(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
PT_DEFINE_Insert(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
//PT_DEFINE_Delete(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
PT_DEFINE_GetAll(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
//PT_DEFINE_DeleteAll(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
//PT_DEFINE_Get(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)
//PT_DEFINE_GetSingle(AttributeTypesTable, DB_TABLE_ATTRIBUTETYPES, DB_TABLE_ATTRIBUTETYPES_FIELDS)