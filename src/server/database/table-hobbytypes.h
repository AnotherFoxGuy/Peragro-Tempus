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

#ifndef TABLE_HOBBYTYPES_H
#define TABLE_HOBBYTYPES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name    | C++ type name     | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_HOBBYTYPES HobbyTypes
#define DB_TABLE_HOBBYTYPES_FIELDS \
  ((id,       size_t,             PT_PrimaryKey,  0)) \
  ((name,     std::string,        0,              0))

PT_DECLARE_VO(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)

class HobbyTypesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)

  PT_DECLARE_CreateTable(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)

  PT_DECLARE_Insert(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)

public:
  HobbyTypesTable(Database* db);

  PT_DECLARE_DropTable(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)
  PT_DECLARE_GetAll(HobbyTypesTable, DB_TABLE_HOBBYTYPES, DB_TABLE_HOBBYTYPES_FIELDS)
};

#endif // TABLE_HOBBYTYPES_H
