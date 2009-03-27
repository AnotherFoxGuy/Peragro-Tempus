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

#ifndef TABLE_DOORS_H
#define TABLE_DOORS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type        | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_DOORS Doors
#define DB_TABLE_DOORS_FIELDS \
  ((entity_id,          size_t,             PT_PrimaryKey,  (Entities, id) )) \
  ((name,               std::string,        0,              0 )) \
  ((meshes_id,          size_t,             0,              (Meshes, id) )) \
  ((isLocked,           bool,               0,              0 )) \
  ((isOpen,             bool,               0,              0 )) \
  ((animationName,      std::string,        0,              0 )) \

PT_DECLARE_VO(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)

/**
 * Provides an interface to the database to handle storage of doors.
 */
class DoorsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)

  PT_DECLARE_CreateTable(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)

public:
  DoorsTable(Database* db);

  PT_DECLARE_DropTable(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)

  PT_DECLARE_Insert(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)
  PT_DECLARE_GetAll(DoorsTable, DB_TABLE_DOORS, DB_TABLE_DOORS_FIELDS)
};

#endif //TABLE_DOORS_H
