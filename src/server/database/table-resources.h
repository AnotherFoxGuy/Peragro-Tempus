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

#ifndef TABLE_RESOURCES_H
#define TABLE_RESOURCES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type        | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_RESOURCES Resources
#define DB_TABLE_RESOURCES_FIELDS \
  ((entity_id,         size_t,             PT_PrimaryKeyS,  (Entities, id) )) \
  ((resourceType_id,   size_t,             PT_PrimaryKey,   (ResourcesTypes, id) )) \
  ((value,             float,              0,               0)) \

PT_DECLARE_VO(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)

/**
 * Provides an interface to the database to handle storage of resources.
 */
class ResourcesTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)

  PT_DECLARE_CreateTable(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)

public:
  ResourcesTable(Database* db);

  PT_DECLARE_DropTable(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)

  PT_DECLARE_Insert(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)
  PT_DECLARE_GetAll(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)

  PT_DECLARE_Get(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)
  //PT_DECLARE_GetSingle(ResourcesTable, DB_TABLE_RESOURCES, DB_TABLE_RESOURCES_FIELDS)
};

#endif //TABLE_RESOURCES_H
