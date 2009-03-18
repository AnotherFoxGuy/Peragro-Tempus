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

#ifndef TABLE_RESOURCESTYPES_H
#define TABLE_RESOURCESTYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_RESOURCESTYPES ResourcesTypes
#define DB_TABLE_RESOURCESTYPES_FIELDS \
  ((id,                 size_t,             1,            0)) \
  ((name,               std::string,        0,            0)) \
  ((abilityType_id,     size_t,             0,            (AbilityTypes, id) )) \
  ((multiplier,         size_t,             0,            0))

PT_DECLARE_VO(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ResourcesTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)

  PT_DECLARE_CreateTable(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)

public:
  ResourcesTypesTable(Database* db);

  PT_DECLARE_DropTable(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)

  PT_DECLARE_Insert(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)
  PT_DECLARE_GetAll(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)

  //PT_DECLARE_Get(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)
  //PT_DECLARE_GetSingle(ResourcesTypesTable, DB_TABLE_RESOURCESTYPES, DB_TABLE_RESOURCESTYPES_FIELDS)
};

#endif //TABLE_RESOURCESTYPES_H
