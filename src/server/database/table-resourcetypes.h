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

#ifndef TABLE_RESOURCETYPES_H
#define TABLE_RESOURCETYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_RESOURCETYPES ResourceTypes
#define DB_TABLE_RESOURCETYPES_FIELDS \
  ((id,                 size_t,             1,            0)) \
  ((name,               std::string,        0,            0)) \
  ((abilityType_id,     size_t,             0,            (AbilityTypes, id) )) \
  ((multiplier,         size_t,             0,            0))

PT_DECLARE_VO(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ResourceTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)

  PT_DECLARE_CreateTable(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)

public:
  ResourceTypesTable(Database* db);

  PT_DECLARE_DropTable(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)

  PT_DECLARE_Insert(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
  PT_DECLARE_GetAll(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)

  //PT_DECLARE_Get(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
  //PT_DECLARE_GetSingle(ResourceTypesTable, DB_TABLE_RESOURCETYPES, DB_TABLE_RESOURCETYPES_FIELDS)
};

#endif //TABLE_RESOURCETYPES_H
