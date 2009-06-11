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

#ifndef TABLE_LOCATIONS_H
#define TABLE_LOCATIONS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key     | Foreign Key
//-----------------------------------------------------------------------------------
#define PT_DB_TABLE_LOCATIONS Locations
#define PT_DB_TABLE_LOCATIONS_FIELDS \
  ((id,                 size_t,             PT_PrimaryKey,   0)) \
  ((name,               std::string,        0,               0)) \
  ((location,           WFMath::Point<3>,   0,               0)) \

PT_DECLARE_VO(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class LocationsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)

  PT_DECLARE_CreateTable(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)

public:
  LocationsTable(Database* db);

  PT_DECLARE_DropTable(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)

  PT_DECLARE_Insert(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
  PT_DECLARE_Delete(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
  PT_DECLARE_GetAll(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
  PT_DECLARE_DeleteAll(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
  PT_DECLARE_Get(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
  PT_DECLARE_GetSingle(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)

  size_t GetMaxId();
  size_t FindByName(const std::string& name);

};

#endif //TABLE_LOCATIONS_H
