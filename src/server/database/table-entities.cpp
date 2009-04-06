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

#include "table-entities.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

#include "common/entity/entity.h"

EntityTable::EntityTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from " PT_GetTableName(PT_DB_TABLE_ENTITIES) ";");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

PT_DEFINE_CreateTable(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

PT_DEFINE_DropTable(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

PT_DEFINE_Insert(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
PT_DEFINE_Delete(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

PT_DEFINE_ParseSingleResultSet(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

PT_DEFINE_ParseMultiResultSet(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

PT_DEFINE_GetAll(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

size_t EntityTable::GetMaxId()
{
  ResultSet* rs = db->query("select max(id) from " PT_GetTableName(PT_DB_TABLE_ENTITIES));
  if (rs == 0 || rs->GetRowCount() == 0)
    return 0;

  size_t id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

void EntityTable::Remove(size_t id)
{
  db->update("delete from " PT_GetTableName(PT_DB_TABLE_ENTITIES) " where id = %d;", id);
}

EntitiesTableVOArray EntityTable::GetWorldEntities()
{
  // Get all worldentities: doors, items(with a position), NPCs, mounts.
  ResultSet* rs = db->query(
    "select e.id, e.EntityTypes_id " \
    "from Entities as e, EntityPositions as p " \
    "where e.id = p.entity_id AND e.EntityTypes_id <> %d;",
    Common::Entity::PCEntityType);
  EntitiesTableVOArray arr;
  if (rs == 0 || rs->GetRowCount() == 0)
    return arr;
  arr = ParseMultiResultSet(rs);
  delete rs;
  return arr;
}

