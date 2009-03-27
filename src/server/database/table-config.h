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

#ifndef TABLE_CONFIG_H
#define TABLE_CONFIG_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_CONFIG Config
#define DB_TABLE_CONFIG_FIELDS \
  ((name,               std::string,        PT_PrimaryKey,  0 )) \
  ((value,              std::string,        0,              0 )) \

PT_DECLARE_VO(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class ConfigTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)

  PT_DECLARE_CreateTable(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)

  //PT_DECLARE_Get(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)
  //PT_DECLARE_GetAll(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)

public:
  ConfigTable(Database* db);

  PT_DECLARE_DropTable(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)

  PT_DECLARE_Insert(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)
  PT_DECLARE_Delete(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)
  
  PT_DECLARE_GetSingle(ConfigTable, DB_TABLE_CONFIG, DB_TABLE_CONFIG_FIELDS)
};

#endif //TABLE_CONFIG_H
