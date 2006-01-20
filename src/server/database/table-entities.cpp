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

#include <sstream>
#include <iomanip>

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "database.h"

#include "table-entities.h"

#include "common/entity/entity.h"

EntityTable::EntityTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from entities;");
  delete rs;
  if (rs == 0)
  {
    createTable();
  }
  /*ResultSet* rs2 = */ db->query("select count(*) from entities;");
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
  float pos1[3] = { 0, 0, 0};
  insert(1, "test-dummy", 0, 0, "test", pos1, "room");
  float pos2[3] = { 0, 0, 1};
  insert(2, "apple-test1", 3, 1,"apple", pos2, "room");
  float pos3[3] = { -1, 0, 1};
  insert(3, "apple-test2", 3, 1,"apple", pos3, "room");
  float pos4[3] = { 1, 0, 0};
  insert(4, "apple-test3", 3, 1,"apple", pos4, "room");
  float pos5[3] = { 1, 0, -1};
  insert(5, "apple-test4", 3, 1,"apple", pos5, "room");
  float pos6[3] = { -1, 0, -1};
  insert(6, "apple-test5", 3, 1,"apple", pos6, "room");
}

void EntityTable::insert(int id, const std::string& name, int type, int item,
  const std::string& mesh, const float pos[3], const std::string& sector)
{
  std::ostringstream query;
  if (item == -1)
    return;
  query << "insert into entities (id, name, type, item, mesh, pos_x, pos_y, pos_z, sector) values "
      << "('" << id << "','" << db->escape(name) << "'," << type << ","
      << item << ",'"
      << db->escape(mesh) << "',"
      << std::setprecision(2)
      << pos[0] << "," << pos[1] << "," << pos[2] << ",'"
      << db->escape(sector) << "');";
  db->update(query.str());
}

int EntityTable::getMaxId() const
{
  ResultSet* rs = db->query("select max(id) from entities");
  if (rs->GetRowCount() == 0) 
    return 0;

  return atoi(rs->GetData(0,0).c_str());
}

void EntityTable::dropTable()
{
  db->update("drop table entities;");
}

void EntityTable::remove(int id)
{
  std::ostringstream query;
  query << "delete from entities where id = " << id << ";";
  db->update(query.str());
}

bool EntityTable::existsEntity(const std::string& name)
{
  std::ostringstream query;
  query << "select id from entities where name = '" << db->escape(name) << "';";
  ResultSet* rs = db->query(query.str());
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Entity* EntityTable::getEntity(const std::string& name) const
{
  std::ostringstream query;
  query << "select * from entities where name = '" << db->escape(name) << "';";
  ResultSet* rs = db->query(query.str());
  if (rs->GetRowCount() == 0) 
    return 0;

  Entity* entity = new Entity();
  entity->setId(atoi(rs->GetData(0,0).c_str()));
  entity->setName(rs->GetData(0,1));
  entity->setType(atoi(rs->GetData(0,2).c_str()));
  entity->setItem(atoi(rs->GetData(0,3).c_str()));
  entity->setMesh(rs->GetData(0,4));
  entity->setPos((float)atof(rs->GetData(0,5).c_str()),
    (float)atof(rs->GetData(0,6).c_str()),
    (float)atof(rs->GetData(0,7).c_str()));
  entity->setSector(rs->GetData(0,8));
  delete rs;
  return entity;
}

void EntityTable::getAllEntities(std::vector<Entity*>& entities)
{
  ResultSet* rs = db->query("select * from entities;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Entity* entity = new Entity();
    entity->setId(atoi(rs->GetData(i,0).c_str()));
    entity->setName(rs->GetData(i,1));
    entity->setType(atoi(rs->GetData(i,2).c_str()));
    entity->setItem(atoi(rs->GetData(i,3).c_str()));
    entity->setMesh(rs->GetData(i,4));
    entity->setPos((float)atof(rs->GetData(i,5).c_str()),
      (float)atof(rs->GetData(i,6).c_str()),
      (float)atof(rs->GetData(i,7).c_str()));
    entity->setSector(rs->GetData(i,8));
    entities.push_back(entity);
  }
}  
