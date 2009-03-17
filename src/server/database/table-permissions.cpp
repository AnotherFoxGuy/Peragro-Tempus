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

#include "table-permissions.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

PermissionsTable::PermissionsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(DB_TABLE_PERMISSIONS) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
PT_DEFINE_DropTable(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
PT_DEFINE_Insert(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
PT_DEFINE_ParseSingleResultSet(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
PT_DEFINE_ParseMultiResultSet(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
PT_DEFINE_GetAll(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)

PermissionsTableVOArray PermissionsTable::Get(const std::string& login)
{
  ResultSet* rs = db->query("select * from " PT_GetTableName(DB_TABLE_PERMISSIONS) " where users_login='%s';", login.c_str());
  PermissionsTableVOArray arr;
  if (!rs) return arr;
  arr = ParseMultiResultSet(rs);
  delete rs;
  return arr;
}