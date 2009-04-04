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

#include "table-speciesabilities.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

SpeciesAbilitiesTable::SpeciesAbilitiesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_SPECIESABILITIES) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
PT_DEFINE_DropTable(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
PT_DEFINE_ParseSingleResultSet(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
PT_DEFINE_ParseMultiResultSet(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
PT_DEFINE_Insert(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
//PT_DEFINE_Delete(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
//PT_DEFINE_GetAll(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
//PT_DEFINE_DeleteAll(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
//PT_DEFINE_Get(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
PT_DEFINE_GetSingle(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)