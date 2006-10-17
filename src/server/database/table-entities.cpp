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
#include "table-characters.h"
#include "table-npcentities.h"

#include "server/entity/entity.h"
#include "server/entity/itementity.h"
#include "server/entity/itemmanager.h"
#include "server/entity/pcentity.h"
#include "server/entity/npcentity.h"
#include "server/entity/doorentity.h"

#include "server/server.h"

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
    "mesh TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");


  float pos1[3] = { 29, 2, 106 };
  ptString dummy("test-dummy", 10);
  ptString test("test", 4);
  ptString room("room", 4);
  ptString apple("apple", 5);
  ptString dummy1("Baby Dragonfly", 14);
  ptString test1("test1", 5);
  //ptString gate("gate", 4);

  insert(1, dummy, 1, 0, test, pos1, room);
  float pos2[3] = { 0, 0, 1};
  insert(2, apple, 3, 1,apple, pos2, room);
  float pos3[3] = { -1, 0, 1};
  insert(3, apple, 3, 1,apple, pos3, room);
  float pos4[3] = { 1, 0, 0};
  insert(4, apple, 3, 1,apple, pos4, room);
  float pos5[3] = { 1, 0, -1};
  insert(5, apple, 3, 1,apple, pos5, room);
  float pos6[3] = { -1, 0, -1};
  insert(7, apple, 3, 1,apple, pos6, room);
  float pos7[3] = { 41, 2, 172};
  insert(6, dummy1, 1, 0,test1, pos7, room);
  //float pos8[3] = { 27.9388713837,179.982452393,-6.57791852951 };
  //insert(7, gate, 2, 0,gate, pos8, room);
}

void EntityTable::insert(int id, ptString name, int type, int item, ptString mesh, const float pos[3], ptString sector)
{
  if (item == -1) return;

  db->update("insert into entities (id, name, type, item, mesh, pos_x, pos_y, pos_z, sector) values "
    "('%d', '%q',%d,%d,'%q',%.2f,%.2f,%.2f,'%q');", id, *name, type, item, *mesh, pos[0], pos[1], pos[2], *sector);
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
  db->update("delete from entities where id = %d;", id);
}

void EntityTable::update(Entity* entity)
{
  db->update("update entities set pos_x=%.2f, pos_y=%.2f, pos_z=%.2f, sector=%q where id = %d;",
    entity->getPos()[0], entity->getPos()[1], entity->getPos()[2], *entity->getSector(), entity->getId() );
}

bool EntityTable::existsEntity(ptString name)
{
  ResultSet* rs = db->query("select id from entities where name = '%q';", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

const Entity* EntityTable::getEntity(ptString name)
{
  ResultSet* rs = db->query("select * from entities where name = '%q';", *name);
  if (rs->GetRowCount() == 0) 
    return 0;

  const Entity* entity = parseEntity(rs, 0);

  delete rs;
  return entity;
}

void EntityTable::getAllEntities(Array<const Entity*>& entities)
{
  ResultSet* rs = db->query("select * from entities;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    const Entity* entity = parseEntity(rs, i);
    if (entity)
    {
      entities.add(entity);
    }
  }
  delete rs;
}  

const Entity* EntityTable::parseEntity(ResultSet* rs, size_t i)
{
  const Entity* entity = 0;

  switch (atoi(rs->GetData(i,2).c_str()))
  {
    case Entity::ItemEntityType:
    {
      ItemEntity* ent = new ItemEntity();
      Item* item = Server::getServer()->getItemManager()->findById(atoi(rs->GetData(i,3).c_str()));
      assert(item);
      ent->createFromItem(item);
      entity = ent->getEntity();
      break;
    }
    case Entity::PlayerEntityType:
    {
      entity = (new PcEntity())->getEntity();
      break;
    }
    case Entity::NPCEntityType:
    {
      NpcEntitiesTableVO* npc_vo = 
        db->getNpcEntitiesTable()->getById(atoi(rs->GetData(i,0).c_str()));

      if (!npc_vo) return 0;

      Character* character =
        db->getCharacterTable()->findCharacterById(npc_vo->character, 0);

      if (!character) return 0;

      NpcEntity* npc = new NpcEntity();
      npc->setCharacter(character);
      npc->setAI(AI::createAI(npc_vo->ai));

      entity = npc->getEntity();
      break;
    }
    case Entity::DoorEntityType:
    {
      int packeddata = atoi(rs->GetData(i,3).c_str());
      DoorEntity* ent = new DoorEntity();
      ent->setOpen  ((packeddata & 1) != 0);
      ent->setLocked((packeddata & 2) != 0);
      entity = ent->getEntity();
      break;
    }
    default:
    { 
      // Unknown Entity!
      return 0;
    }
  };

  Entity* l_ent = entity->getLock();
  l_ent->setId(atoi(rs->GetData(i,0).c_str()));
  l_ent->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
  l_ent->setMesh(ptString(rs->GetData(i,4).c_str(), rs->GetData(i,4).length()));
  l_ent->setPos((float)atof(rs->GetData(i,5).c_str()), (float)atof(rs->GetData(i,6).c_str()), (float)atof(rs->GetData(i,7).c_str()));
  l_ent->setSector(ptString(rs->GetData(i,8).c_str(), rs->GetData(i,8).length()));
  l_ent->freeLock();

  return entity;
}
