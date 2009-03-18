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

#ifndef TABLE_ENTITYPOSITIONS_H
#define TABLE_ENTITYPOSITIONS_H

#include "common/database/table.h"
#include "common/database/tablehelper.h"

class Database;
class ResultSet;

//-----------------------------------------------------------------------------------
//| Name               | C++ type name    | Primary Key  | Foreign Key
//-----------------------------------------------------------------------------------
#define DB_TABLE_ENTITYPOSITIONS EntityPositions
#define DB_TABLE_ENTITYPOSITIONS_FIELDS \
  ((entity_id,         size_t,             1,            (Entities, id) )) \
  ((position,          WFMath::Point<3>,   0,            0)) \
  ((rotation,          WFMath::Point<3>,   0,            0)) \

PT_DECLARE_VO(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)

/**
 * Provides an interface to the database to handle storage of the location of
 * each entity.
 */
class EntityPositionsTable : public Table
{
private:
  PT_DECLARE_ParseSingleResultSet(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)
  PT_DECLARE_ParseMultiResultSet(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)

  PT_DECLARE_CreateTable(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)

public:
  EntityPositionsTable(Database* db);

  PT_DECLARE_DropTable(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)

  PT_DECLARE_Insert(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)
  PT_DECLARE_GetAll(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)

  PT_DECLARE_GetSingle(EntityPositionsTable, DB_TABLE_ENTITYPOSITIONS, DB_TABLE_ENTITYPOSITIONS_FIELDS)

  void Remove(size_t id);
};

#endif //TABLE_ENTITYPOSITIONS_H
