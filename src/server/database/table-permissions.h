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

#ifndef TABLE_PERMISSIONS_H
#define TABLE_PERMISSIONS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_PERMISSIONS Permissions
#define DB_TABLE_PERMISSIONS_FIELDS \
  ((users_login,        std::string,       1,            (Users, login) )) \
  ((type,               std::string,       0,            0)) \
  ((level,              size_t,            0,            0))

PT_DECLARE_VO(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)

/**
 * Provides an interface to the database to handle storage of permissions.
 */
class PermissionsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)

  PT_DECLARE_CreateTable(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)

public:
  PermissionsTable(Database* db);

  PT_DECLARE_DropTable(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)

  PT_DECLARE_Insert(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)
  PT_DECLARE_GetAll(PermissionsTable, DB_TABLE_PERMISSIONS, DB_TABLE_PERMISSIONS_FIELDS)

  PermissionsTableVOArray Get(const std::string& login);
};

#endif //TABLE_PERMISSIONS_H
