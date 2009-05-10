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

#include "table-species.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

SpeciesTable::SpeciesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_SPECIES) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert(1, "Human", 1, 60.0f);
  }
  delete rs;
}

PT_DEFINE_CreateTable(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
PT_DEFINE_DropTable(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
PT_DEFINE_ParseSingleResultSet(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
PT_DEFINE_ParseMultiResultSet(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
PT_DEFINE_Insert(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
//PT_DEFINE_Delete(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
//PT_DEFINE_GetAll(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
//PT_DEFINE_DeleteAll(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
//PT_DEFINE_Get(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
PT_DEFINE_GetSingle(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)


size_t SpeciesTable::FindByName(const std::string& name)
{
  ResultSet* rs = db->query("select * from " PT_GetTableName(DB_TABLE_SPECIES) " "
    "where name='%s';",
    name.c_str());
  std::vector<SpeciesTableVOp> arr;
  if (!rs) return 0;
  arr = ParseMultiResultSet(rs);
  delete rs;
  if (arr.size() != 1)
    return 0;
  return arr[0]->id;
}


