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

#ifndef TABLE_ABILITYTYPES_H
#define TABLE_ABILITYTYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name    | C++ type name     | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ABILITYTYPES AbilityTypes
#define DB_TABLE_ABILITYTYPES_FIELDS \
  ((id,       size_t,             1,            0)) \
  ((name,     std::string,        0,            0))

PT_DECLARE_VO(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)

class AbilityTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)

  PT_DECLARE_CreateTable(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)

  PT_DECLARE_Insert(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)

public:
  AbilityTypesTable(Database* db);

  PT_DECLARE_DropTable(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)
  PT_DECLARE_GetAll(AbilityTypesTable, DB_TABLE_ABILITYTYPES, DB_TABLE_ABILITYTYPES_FIELDS)
};

#endif // TABLE_ABILITYTYPES_H
