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

#ifndef TABLE_ZONES_H
#define TABLE_ZONES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ZONES Zones
#define DB_TABLE_ZONES_FIELDS \
  ((id,                 size_t,             1,            0 )) \
  ((type,               std::string,        0,            0 )) \

PT_DECLARE_VO(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ZonesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)

  PT_DECLARE_CreateTable(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)

public:
  ZonesTable(Database* db);

  PT_DECLARE_DropTable(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)

  PT_DECLARE_Insert(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)
  PT_DECLARE_Delete(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)
  PT_DECLARE_GetAll(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)
  PT_DECLARE_DeleteAll(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)

  //PT_DECLARE_Get(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)
  PT_DECLARE_GetSingle(ZonesTable, DB_TABLE_ZONES, DB_TABLE_ZONES_FIELDS)
};

#endif //TABLE_ZONES_H
