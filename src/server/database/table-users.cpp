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

#include "table-users.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

UsersTable::UsersTable(Database* db) : Table(db)
{
  ResultSet* rs = db->Query("select count(*) from " PT_GetTableName(DB_TABLE_USERS) ";");
  if (rs == 0)
  {
    CreateTable();
    Insert("admin","admin");
  }
  delete rs;
}

PT_DEFINE_CreateTable(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
PT_DEFINE_DropTable(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
PT_DEFINE_Insert(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
PT_DEFINE_ParseSingleResultSet(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
PT_DEFINE_ParseMultiResultSet(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
PT_DEFINE_GetAll(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)

PT_DEFINE_GetSingle(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
