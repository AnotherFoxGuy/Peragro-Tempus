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

#ifndef TABLE_SPECIES_H
#define TABLE_SPECIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIES Species
#define DB_TABLE_SPECIES_FIELDS \
  ((id,                 size_t,             PT_PrimaryKey,   0)) \
  ((name,               std::string,        0,               0)) \
  ((meshes_id,          size_t,             0,              (Meshes, id) )) \
  ((maxAge,             float,              0,               0)) \

PT_DECLARE_VO(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)

  PT_DECLARE_CreateTable(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)

public:
  SpeciesTable(Database* db);

  PT_DECLARE_DropTable(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)

  PT_DECLARE_Insert(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
  //PT_DECLARE_Delete(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
  //PT_DECLARE_GetAll(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)

  //PT_DECLARE_Get(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)
  PT_DECLARE_GetSingle(SpeciesTable, DB_TABLE_SPECIES, DB_TABLE_SPECIES_FIELDS)

  size_t FindByName(const std::string& name);

};

#endif //TABLE_SPECIES_H
