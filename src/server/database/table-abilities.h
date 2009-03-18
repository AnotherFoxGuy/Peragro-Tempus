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

#ifndef TABLE_ABILITIES_H
#define TABLE_ABILITIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name            | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ABILITIES Abilities
#define DB_TABLE_ABILITIES_FIELDS \
  ((entity_id,      size_t,             1,            (Entities, id) )) \
  ((AbilityTypes_id,size_t,             0,            (AbilityTypes, id) )) \
  ((xp,             float,              0,            0))

PT_DECLARE_VO(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)

/**
 * Provides an interface to the database to handle storage of Abilities
 */
class AbilitiesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)

  PT_DECLARE_CreateTable(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)

public:
  AbilitiesTable(Database* db);

  PT_DECLARE_DropTable(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)

  PT_DECLARE_Insert(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)
  PT_DECLARE_GetAll(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)

  PT_DECLARE_Get(AbilitiesTable, DB_TABLE_ABILITIES, DB_TABLE_ABILITIES_FIELDS)
};

#endif //TABLE_ABILITIES_H
