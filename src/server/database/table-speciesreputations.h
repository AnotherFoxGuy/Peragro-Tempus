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

#ifndef TABLE_SPECIESREPUTATIONS_H
#define TABLE_SPECIESREPUTATIONS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIESREPUTATIONS SpeciesReputations
#define DB_TABLE_SPECIESREPUTATIONS_FIELDS \
  ((species_id,         size_t,             PT_PrimaryKeyS,  (Species, id) )) \
  ((ReputationTypes_id, size_t,             PT_PrimaryKey,   (ReputationTypes, id) )) \
  ((minLevel,           size_t,              0,                0)) \
  ((maxLevel,           size_t,              0,                0))

PT_DECLARE_VO(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesReputationsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)

  PT_DECLARE_CreateTable(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)

public:
  SpeciesReputationsTable(Database* db);

  PT_DECLARE_DropTable(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)

  PT_DECLARE_Insert(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)
  //PT_DECLARE_Delete(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)
  //PT_DECLARE_GetAll(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)

  //PT_DECLARE_Get(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)
  //PT_DECLARE_GetSingle(SpeciesReputationsTable, DB_TABLE_SPECIESREPUTATIONS, DB_TABLE_SPECIESREPUTATIONS_FIELDS)
};

#endif //TABLE_SPECIESREPUTATIONS_H
