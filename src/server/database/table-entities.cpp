/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/database/database.h"

#include "table-entities.h"

#include "server/entity/entity.h"

EntityTable::EntityTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from entities;");
  delete rs;
  if (rs == 0)
  {
    createTable();
  }
  ResultSet* rs2 = db->query("select count(*) from entities;");
  delete rs2;
}

void EntityTable::createTable()
{
  printf("Creating Table entities...\n");
  db->update("create table entities ("
    "id INTEGER, "
    "name TEXT, "
    "type INTEGER, "
    "item INTEGER, "
    "mesh INTEGER, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "rot FLOAT, "
    "sector TEXT, "
    "variation INTEGER, "
    "PRIMARY KEY (id) );");
}

void EntityTable::insert(int id, const std::string& name, int type, int item,
                         unsigned int variation, unsigned int mesh,
                         const WFMath::Point<3>& pos, float rot, const std::string& sector)
{
  if (item == -1) return;

  db->update("insert into entities (id, name, type, item, variation, mesh, "
    "pos_x, pos_y, pos_z, rot, sector) values "
    "('%d', '%q',%d,%d,%d,'%d',%.2f,%.2f,%.2f,%.2f,'%q');",
    id, name.c_str(), type, item, variation, mesh, pos[0], pos[1], pos[2], rot, sector.c_str());
}

int EntityTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from entities");
  if (rs == 0 || rs->GetRowCount() == 0)
    return 0;

  int id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

void EntityTable::dropTable()
{
  db->update("drop table entities;");
}

void EntityTable::remove(int id)
{
  db->update("delete from entities where id = %d;", id);
}

void EntityTable::update(EntitiesTableVO* entity)
{
  db->update("update entities set pos_x=%.2f, pos_y=%.2f, pos_z=%.2f, "
    "rot=%.2f, sector=%q where id = %d;", entity->pos_x, entity->pos_y,
    entity->pos_z, entity->rotation, *entity->sector, entity->id);
}

bool EntityTable::existsEntity(const ptString& name)
{
  ResultSet* rs = db->query("select id from entities where name = '%q';",
    *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

EntitiesTableVO* EntityTable::getEntity(const ptString& name)
{
  ResultSet* rs = db->query("select * from entities where name = '%q';",
    *name);
  if (rs->GetRowCount() == 0)
    return 0;

  EntitiesTableVO* entity = parseSingleResultSet(rs, 0);

  delete rs;
  return entity;
}

Array<EntitiesTableVO*> EntityTable::getAllEntities()
{
  ResultSet* rs = db->query("select * from entities;");
  Array<EntitiesTableVO*> entities = parseMultiResultSet(rs);
  delete rs;
  return entities;
}

EntitiesTableVO* EntityTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  EntitiesTableVO* vo = new EntitiesTableVO();
  vo->id = atoi(rs->GetData(row,0).c_str());
  vo->name = ptString(rs->GetData(row,1).c_str(), rs->GetData(row,1).length());
  vo->type = atoi(rs->GetData(row,2).c_str());
  vo->item = atoi(rs->GetData(row,3).c_str());
  vo->mesh = atoi(rs->GetData(row,4).c_str());
  vo->pos_x = (float) atof(rs->GetData(row,5).c_str());
  vo->pos_y = (float) atof(rs->GetData(row,6).c_str());
  vo->pos_z = (float) atof(rs->GetData(row,7).c_str());
  vo->rotation = (float) atof(rs->GetData(row,8).c_str());
  vo->sector = ptString(rs->GetData(row,9).c_str(), rs->GetData(row,9).length());
  vo->variation = atoi(rs->GetData(row,10).c_str());
  return vo;
}

Array<EntitiesTableVO*> EntityTable::parseMultiResultSet(ResultSet* rs)
{
  Array<EntitiesTableVO*> arr;
  for (size_t i = 0; rs && i < rs->GetRowCount(); i++)
  {
    EntitiesTableVO* obj = parseSingleResultSet(rs, i);    arr.add(obj);
  }
  return arr;
}
