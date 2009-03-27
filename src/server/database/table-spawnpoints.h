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

#ifndef TABLE_SPAWNPOINTS_H
#define TABLE_SPAWNPOINTS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_SPAWNPOINTS SpawnPoints
#define DB_TABLE_SPAWNPOINTS_FIELDS \
  ((id,                 size_t,             PT_PrimaryKey,  0)) \
  ((ItemTemplate_id,    size_t,             0,              (ItemTemplates, id) )) \
  ((position,           WFMath::Point<3>,   0,              0)) \
  ((interval,           size_t,             0,              0)) \

PT_DECLARE_VO(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)

/**
 * Provides an interface to the database to handle storage of reputations.
 */
class SpawnPointsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)

  PT_DECLARE_CreateTable(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)

public:
  SpawnPointsTable(Database* db);

  size_t GetMaxId();

  PT_DECLARE_DropTable(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)

  PT_DECLARE_Insert(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)
  //PT_DECLARE_Delete(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)
  PT_DECLARE_GetAll(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)
  PT_DECLARE_DeleteAll(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)

  //PT_DECLARE_Get(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)
  //PT_DECLARE_GetSingle(SpawnPointsTable, DB_TABLE_SPAWNPOINTS, DB_TABLE_SPAWNPOINTS_FIELDS)
};

#endif //TABLE_SPAWNPOINTS_H
