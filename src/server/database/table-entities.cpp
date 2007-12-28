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
    "mesh TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "rot FLOAT, "
    "sector TEXT, "
    "variation INTEGER, "
    "PRIMARY KEY (id) );");


  ptString dummy("test-dummy", 10);
  ptString test("test", 4);
  ptString room("room", 4);
  ptString apple("apple", 5);
  ptString dummy1("Baby Dragonfly", 14);
  ptString test1("test1", 5);
  ptString skel("skeleton", 8);
  ptString skel_trader("Forgotten Trader", 16);
  ptString horse("horse", 5);

  float pos1[3] = { 29, 2, 106 };
  insert(1, dummy, Entity::NPCEntityType, 0, 0, test, pos1, 0, room);

  float pos2[3] = { 41, 2, 172};
  insert(2, dummy1, Entity::NPCEntityType, 0, 0, test1, pos2, 0, room);

  float pos3[3] = { 51.75f, 2.03f, 90.95f };
  insert(3, skel_trader, Entity::NPCEntityType, 0, 0, skel, pos3, 0, room);


  // Creating Undead Squad.
  for (int i = 0; i < 100; i++)
  {
    std::string name("Evil Undead");
    ptString skel2("skeleton2", 9);

    int x = i % 6;
    int y = i / 6;

    // Bridge 100 = 
    // x = 27 - 33 = 6
    // y = 137 - 148 = 11
    float pos[3] = { 27 + x, 2.2f, 137 + y * 0.6};

    insert(4 + i, ptString(name.c_str(), name.length()), Entity::NPCEntityType, 0, 0, skel2, pos, 0, room);
  }

  float pos4[3] = { 82, 2.2f, 37.2f };
  insert(104, horse, Entity::MountEntityType, 0, 0, horse, pos4, 0, room);
  float pos5[3] = { -110, 0.2f, 5.05f };
  insert(105, horse, Entity::MountEntityType, 0, 0, horse, pos5, 0, room);
  float pos6[3] = { -112, 0.2f, 4.8f };
  insert(106, horse, Entity::MountEntityType, 0, 0, horse, pos6, 0, room);

  ptString canyon("Canyon", 6);

  float pos7[3] = { 892.575256,17.702299,474.827728 };
  insert(107, horse, Entity::MountEntityType, 0, 0, horse, pos7, 0, canyon);
  float pos8[3] = { 899.218506,17.665253,473.588654 };
  insert(108, horse, Entity::MountEntityType, 0, 0, horse, pos8, 0, canyon);
  float pos9[3] = { 905.094604,17.629965,473.702515 };
  insert(109, horse, Entity::MountEntityType, 0, 0, horse, pos9, 0, canyon);
  float pos10[3] = { 911.103638,17.593407,474.046356 };
  insert(110, horse, Entity::MountEntityType, 0, 0, horse, pos10, 0, canyon);

  float pos11[3] = { 830.781616,17.534599,608.832093 };
  insert(111, horse, Entity::MountEntityType, 0, 0, horse, pos11, 0, canyon);
  float pos12[3] = { 824.341431,17.534599,608.410400 };
  insert(112, horse, Entity::MountEntityType, 0, 0, horse, pos12, 0, canyon);
  float pos13[3] = { 818.504272,17.534559,607.860107 };
  insert(113, horse, Entity::MountEntityType, 0, 0, horse, pos13, 0, canyon);
  float pos14[3] = { 811.655457,17.534599,607.679810 };
  insert(114, horse, Entity::MountEntityType, 0, 0, horse, pos14, 0, canyon);

  float pos15[3] = { 868.223511,17.529799,684.598511 };
  insert(115, horse, Entity::MountEntityType, 0, 0, horse, pos15, 0, canyon);
  float pos16[3] = { 868.412720,17.529799,678.824524 };
  insert(116, horse, Entity::MountEntityType, 0, 0, horse, pos16, 0, canyon);
  float pos17[3] = { 868.227600,17.529799,671.650208 };
  insert(117, horse, Entity::MountEntityType, 0, 0, horse, pos17, 0, canyon);
}

void EntityTable::insert(int id, ptString name, int type, int item, unsigned int variation, ptString mesh, const float pos[3], float rot, ptString sector)
{
  if (item == -1) return;

  db->update("insert into entities (id, name, type, item, variation, mesh, pos_x, pos_y, pos_z, rot, sector) values "
    "('%d', '%q',%d,%d,%d,'%q',%.2f,%.2f,%.2f,%.2f,'%q');", id, *name, type, item, variation, *mesh, pos[0], pos[1], pos[2], rot, *sector);
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

void EntityTable::update(EntitiesTableVO* entity)
{
  db->update("update entities set pos_x=%.2f, pos_y=%.2f, pos_z=%.2f, rot=%.2f, sector=%q where id = %d;",
    entity->pos_x, entity->pos_y, entity->pos_z, entity->rotation, *entity->sector, entity->id );
}

bool EntityTable::existsEntity(ptString name)
{
  ResultSet* rs = db->query("select id from entities where name = '%q';", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

EntitiesTableVO* EntityTable::getEntity(ptString name)
{
  ResultSet* rs = db->query("select * from entities where name = '%q';", *name);
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
  vo->mesh = ptString(rs->GetData(row,4).c_str(), rs->GetData(row,4).length());
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
