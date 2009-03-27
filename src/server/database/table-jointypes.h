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

#ifndef TABLE_JOINTYPES_H
#define TABLE_JOINTYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type     | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_JOINTYPES JoinTypes
#define DB_TABLE_JOINTYPES_FIELDS \
  ((id,                 size_t,             PT_PrimaryKey,  0)) \
  ((name,               std::string,        0,              0))

PT_DECLARE_VO(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)

/**
 * Provides an interface to the database to handle storage of join types for
 * chat channels.
 */
class JoinTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)

  PT_DECLARE_CreateTable(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)

public:
  JoinTypesTable(Database* db);

  PT_DECLARE_DropTable(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)

  PT_DECLARE_Insert(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)
  PT_DECLARE_GetAll(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)

  //PT_DECLARE_Get(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)
  //PT_DECLARE_GetSingle(JoinTypesTable, DB_TABLE_JOINTYPES, DB_TABLE_JOINTYPES_FIELDS)
};

#endif //TABLE_JOINTYPES_H
