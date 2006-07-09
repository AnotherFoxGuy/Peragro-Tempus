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

#include "database.h"

#include "table-entities.h"

#include "common/entity/entity.h"
#include "common/entity/itementity.h"
#include "common/entity/pcentity.h"
#include "common/entity/npcentity.h"

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
  db->update("create table entities ("
    "id INTEGER, "
    "name TEXT, "
    "type INTEGER, "
    "item INTEGER, "
    "mesh TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");

  //Example Data!
  float pos1[3] = { -2, 0, 0};

  ptString dummy("test-dummy", 10);
  ptString test("test", 4);
  ptString room("room", 4);
  ptString apple("apple", 5);

  insert(1, dummy, 0, 0, test, pos1, room);
  float pos2[3] = { 0, 0, 1};
  insert(2, apple, 3, 1,apple, pos2, room);
  float pos3[3] = { -1, 0, 1};
  insert(3, apple, 3, 1,apple, pos3, room);
  float pos4[3] = { 1, 0, 0};
  insert(4, apple, 3, 1,apple, pos4, room);
  float pos5[3] = { 1, 0, -1};
  insert(5, apple, 3, 1,apple, pos5, room);
  float pos6[3] = { -1, 0, -1};
  insert(6, apple, 3, 1,apple, pos6, room);
}

void EntityTable::insert(int id, ptString name, int type, int item, ptString mesh, float pos[3], ptString sector)
{
  if (strlen(*name) + strlen(*mesh) + strlen(*sector) > 512) assert("Strings too long");
  char query[1024];
  if (item == -1)
    return;
  sprintf(query, "insert into entities (id, name, type, item, mesh, pos_x, pos_y, pos_z, sector) values "
    "('%d', '%s',%d,%d,'%s',%.2f,%.2f,%.2f,'%s');", id, *name, type, item, *mesh, pos[0], pos[1], pos[2], *sector);
  db->update(query);
}

int EntityTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from entities");
  if (rs->GetRowCount() == 0) 
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
  char query[1024];
  sprintf(query, "delete from entities where id = %d;", id);
  db->update(query);
}

bool EntityTable::existsEntity(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select id from entities where name = '%s';", *name);
  ResultSet* rs = db->query(query);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Entity* EntityTable::getEntity(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select * from entities where name = '%s';", *name);
  ResultSet* rs = db->query(query);
  if (rs->GetRowCount() == 0) 
    return 0;

  Entity* entity = parseEntity(rs, 0);

  delete rs;
  return entity;
}

void EntityTable::getAllEntities(Array<Entity*>& entities)
{
  ResultSet* rs = db->query("select * from entities;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Entity* entity = parseEntity(rs, i);
    if (entity)
    {
      entities.add(entity);
    }
  }
  delete rs;
}  

Entity* EntityTable::parseEntity(ResultSet* rs, size_t i)
{
  Entity* entity = 0;

  switch (atoi(rs->GetData(i,2).c_str()))
  {
    case Entity::ItemEntity :
    {
      ItemEntity* ent = new ItemEntity();
      ent->setItem(atoi(rs->GetData(i,3).c_str()));
      entity = ent;
      break;
    }
    case Entity::PlayerEntity :
    {
      entity = new PcEntity();
      break;
    }
    default:
    { 
      // Unknown Entity!
      return 0;
    }
  };
  entity->setId(atoi(rs->GetData(i,0).c_str()));
  entity->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
  entity->setMesh(ptString(rs->GetData(i,4).c_str(), rs->GetData(i,4).length()));
  entity->setPos((float)atof(rs->GetData(i,5).c_str()), (float)atof(rs->GetData(i,6).c_str()), (float)atof(rs->GetData(i,7).c_str()));
  entity->setSector(ptString(rs->GetData(i,8).c_str(), rs->GetData(i,8).length()));

  return entity;
}
