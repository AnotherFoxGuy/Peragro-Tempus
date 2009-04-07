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

#ifndef TABLE_ENTITIES_H
#define TABLE_ENTITIES_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name              | C++ type name     | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define PT_DB_TABLE_ENTITIES Entities
#define PT_DB_TABLE_ENTITIES_FIELDS \
  ((id,                 size_t,             PT_PrimaryKey,  0)) \
  ((EntityTypes_id,     size_t,             0,              (EntityTypes, id)))


PT_DECLARE_VO(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

class EntityTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
  PT_DECLARE_ParseMultiResultSet(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

  PT_DECLARE_CreateTable(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

public:
  EntityTable(Database* db);

  PT_DECLARE_DropTable(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

  PT_DECLARE_Insert(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
  PT_DECLARE_Delete(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
  PT_DECLARE_GetAll(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

  size_t GetMaxId();
  EntitiesTableVOArray GetWorldEntities();
};

#endif // TABLE_ENTITIES_H
