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

#ifndef TABLE_SPECIESABILITIES_H
#define TABLE_SPECIESABILITIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIESABILITIES SpeciesAbilities
#define DB_TABLE_SPECIESABILITIES_FIELDS \
  ((species_id,         size_t,             PT_PrimaryKeyS,  (Species, id) )) \
  ((AbilityTypes_id,    size_t,             PT_PrimaryKey,   (AbilityTypes, id) )) \
  ((minXP,              float,              0,                0)) \
  ((maxXP,              float,              0,                0))

PT_DECLARE_VO(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesAbilitiesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)

  PT_DECLARE_CreateTable(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)

public:
  SpeciesAbilitiesTable(Database* db);

  PT_DECLARE_DropTable(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)

  PT_DECLARE_Insert(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
  //PT_DECLARE_Delete(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
  //PT_DECLARE_GetAll(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)

  //PT_DECLARE_Get(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
  PT_DECLARE_GetSingle(SpeciesAbilitiesTable, DB_TABLE_SPECIESABILITIES, DB_TABLE_SPECIESABILITIES_FIELDS)
};

#endif //TABLE_SPECIESABILITIES_H
