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
    Insert(2, "Durability");
    Insert(3, "Weight");
    // Weapons.
    Insert(4, "Reach");
    Insert(5, "Damage");
    Insert(6, "Heft");
    // Ranged Weapons.
    Insert(7, "Pull");
    Insert(8, "Range");
    // Armor.
    Insert(9, "Mobility");
    Insert(10, "Hardness");
    Insert(11, "Bulk");
    Insert(12, "Toughness");

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

