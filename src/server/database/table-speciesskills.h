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

#ifndef TABLE_SPECIESSKILLS_H
#define TABLE_SPECIESSKILLS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPECIESSKILLS SpeciesSkills
#define DB_TABLE_SPECIESSKILLS_FIELDS \
  ((species_id,         size_t,             PT_PrimaryKeyS,  (Species, id) )) \
  ((SkillTypes_id,      size_t,             PT_PrimaryKey,   (SkillTypes, id) )) \
  ((minXP,              float,              0,                0)) \
  ((maxXP,              float,              0,                0))

PT_DECLARE_VO(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpeciesSkillsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)

  PT_DECLARE_CreateTable(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)

public:
  SpeciesSkillsTable(Database* db);

  PT_DECLARE_DropTable(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)

  PT_DECLARE_Insert(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)
  //PT_DECLARE_Delete(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)
  //PT_DECLARE_GetAll(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)
  //PT_DECLARE_DeleteAll(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)

  //PT_DECLARE_Get(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)
  //PT_DECLARE_GetSingle(SpeciesSkillsTable, DB_TABLE_SPECIESSKILLS, DB_TABLE_SPECIESSKILLS_FIELDS)
};

#endif //TABLE_SPECIESSKILLS_H
