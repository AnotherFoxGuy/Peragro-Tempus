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

#include "table-objects.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

ObjectsTable::ObjectsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->Query("select count(*) from " PT_GetTableName(DB_TABLE_OBJECTS) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_DropTable(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_ParseSingleResultSet(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_ParseMultiResultSet(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_Insert(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_Delete(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_GetAll(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
PT_DEFINE_DeleteAll(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
//PT_DEFINE_Get(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)
//PT_DEFINE_GetSingle(ObjectsTable, DB_TABLE_OBJECTS, DB_TABLE_OBJECTS_FIELDS)

size_t ObjectsTable::GetMaxId()
{
  ResultSet* rs = db->Query("select max(id) from Objects");
  if (rs == 0 || rs->GetRowCount() == 0) return 0;
  size_t id = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return id;
}

