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

#include "table-locations.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

LocationsTable::LocationsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->Query("select count(*) from " PT_GetTableName(PT_DB_TABLE_LOCATIONS) ";");
  if (rs == 0)
  {
    CreateTable();
    WFMath::Point<3> defaultPos(900.765,8.26531,12.1211);
    Insert(1, "default", defaultPos );
  }
  delete rs;
}

PT_DEFINE_CreateTable(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_DropTable(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_ParseSingleResultSet(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_ParseMultiResultSet(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_Insert(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_Delete(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_DeleteAll(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_GetAll(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)
PT_DEFINE_GetSingle(LocationsTable, PT_DB_TABLE_LOCATIONS, PT_DB_TABLE_LOCATIONS_FIELDS)

size_t LocationsTable::GetMaxId()
{
  ResultSet* rs = db->Query("select max(id) from " PT_GetTableName(PT_DB_TABLE_LOCATIONS));
  if (rs == 0 || rs->GetRowCount() == 0)
    return 0;

  size_t id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

size_t LocationsTable::FindByName(const std::string& name)
{
  ResultSet* rs = db->Query("select * from " PT_GetTableName(PT_PT_DB_TABLE_LOCATIONS) " "
    "where name='%s';",
    name.c_str());
  std::vector<LocationsTableVOp> arr;
  if (!rs) return 0;
  arr = ParseMultiResultSet(rs);
  delete rs;
  if (arr.size() != 1)
    return 0;
  return arr[0]->id;
}


