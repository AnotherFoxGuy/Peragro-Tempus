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

#include "table-objects.h"

#include "world.h"

size_t ObjectsTable::GetId(ResultSet* rs, size_t row)
{
  return atoi(rs->GetData(row, 0).c_str());
}

std::string ObjectsTable::GetName(ResultSet* rs, size_t row)
{
  return rs->GetData(row, 1);
}

std::string ObjectsTable::GetFactoryFile(ResultSet* rs, size_t row)
{
  return rs->GetData(row, 2);
}

std::string ObjectsTable::GetFactoryName(ResultSet* rs, size_t row)
{
  return rs->GetData(row, 3);
}

Geom::Vector3 ObjectsTable::GetPosition(ResultSet* rs, size_t row, size_t offset)
{
  float x = (float)atof(rs->GetData(row, offset+0).c_str());
  float y = (float)atof(rs->GetData(row, offset+1).c_str());
  float z = (float)atof(rs->GetData(row, offset+2).c_str());
  return Geom::Vector3(x, y, z);
}

std::string ObjectsTable::GetSectorName(ResultSet* rs, size_t row)
{
  return rs->GetData(row, 7);
}

Geom::Box ObjectsTable::GetWorldBB(ResultSet* rs, size_t row)
{
  Geom::Vector3 min = GetPosition(rs, row, 8);
  Geom::Vector3 max = GetPosition(rs, row, 11);
  return Geom::Box(min, max);
}

ObjectsTable::ObjectsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from objects;");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

void ObjectsTable::CreateTable()
{
  printf("Creating Table objects...\n");
  db->update("create table objects ("
    "id INTEGER, "
    "name TEXT, "
    "factoryFile TEXT, "
    "factoryName TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "BB_min_x FLOAT, "
    "BB_min_y FLOAT, "
    "BB_min_z FLOAT, "
    "BB_max_x FLOAT, "
    "BB_max_y FLOAT, "
    "BB_max_z FLOAT, "
    "PRIMARY KEY (id) );");

  // TODO test data, remove.
  Common::World::Object object;
  object.name = "test";
  object.factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
  object.factoryName = "genscythe001";
  object.position = Geom::Vector3(642, 14, 371);
  object.sector = "World";
  object.worldBB = Geom::Box(object.position, object.position+Geom::Vector3(2, 2, 2));
  Insert(object, true);
}

int getMaxId(Database* db)
{
  ResultSet* rs = db->query("select max(id) from objects");
  if (rs == 0 || rs->GetRowCount() == 0) return 0;
  int id = atoi(rs->GetData(0,0).c_str());
  delete rs;
  return id;
}

void ObjectsTable::Insert(const Common::World::Object& object, bool unique)
{
  const char* query = { "insert into objects("
    "id, name, factoryFile, factoryName, "
    "pos_x, pos_y, pos_z, "
    "sector, "
    "BB_min_x, BB_min_y, BB_min_z, BB_max_x, BB_max_y, BB_max_z"
    ") values ("
    "%d, '%s', '%s', '%s',"
    "%.2f, %.2f, %.2f, "
    "'%s', "
    "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f"
    ");" };

  int id = (int)object.id;
  if (unique)
    id = getMaxId(db);

  db->update(query, id, object.name.c_str(), object.factoryFile.c_str(), object.factoryName.c_str(),
    object.position.x, object.position.y, object.position.z,
    object.sector.c_str(),
    object.worldBB.Min().x, object.worldBB.Min().y, object.worldBB.Min().z,
    object.worldBB.Max().x, object.worldBB.Max().y, object.worldBB.Max().z);
}

void ObjectsTable::DropTable()
{
  db->update("drop table objects;");
}

void ObjectsTable::GetObjects(Array<Common::World::Object>& objects)
{
  ResultSet* rs = db->query("select * from objects;");
  if (!rs) return;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    Common::World::Object object;
    object.id = GetId(rs, i);
    object.name = GetName(rs, i);
    object.factoryFile = GetFactoryFile(rs, i);
    object.factoryName = GetFactoryName(rs, i);
    object.position = GetPosition(rs, i);
    object.sector = GetSectorName(rs, i);
    object.worldBB = GetWorldBB(rs, i);
    objects.add(object);
  }
  delete rs;
}
