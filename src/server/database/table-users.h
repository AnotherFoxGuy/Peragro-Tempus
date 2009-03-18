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

#ifndef TABLE_USERS_H
#define TABLE_USERS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_USERS Users
#define DB_TABLE_USERS_FIELDS \
  ((login,              std::string,        1,            0)) \
  ((passwordHash,       std::string,        0,            0))

PT_DECLARE_VO(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)

/**
 * Provides an interface to the database to handle storage of user logins.
 */
class UsersTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)

  PT_DECLARE_CreateTable(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)

public:
  UsersTable(Database* db);

  PT_DECLARE_DropTable(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)

  PT_DECLARE_Insert(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)
  PT_DECLARE_GetAll(UsersTable, DB_TABLE_USERS, DB_TABLE_USERS_FIELDS)

  UsersTableVOp GetUser(const std::string& login);
};

#endif //TABLE_USERS_H
