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

#include "table-factories.h"

#include "world.h"

FactoriesTable::FactoriesTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from factories;");
  if (rs == 0)
  {
    CreateTable();
  }
  delete rs;
}

void FactoriesTable::CreateTable()
{
  printf("Creating Table factories...\n");
  db->update("create table factories ("
    "factoryFile TEXT, "
    "factoryName TEXT, "
    "BB_min_x FLOAT, "
    "BB_min_y FLOAT, "
    "BB_min_z FLOAT, "
    "BB_max_x FLOAT, "
    "BB_max_y FLOAT, "
    "BB_max_z FLOAT, "
    "PRIMARY KEY (factoryFile, factoryName) );");
}

void FactoriesTable::Insert(const Factory& factory)
{
  const char* query = { "insert into factories("
    "factoryFile, factoryName, "
    "BB_min_x, BB_min_y, BB_min_z, BB_max_x, BB_max_y, BB_max_z"
    ") values ("
    "'%s', '%s',"
    "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f"
    ");" };

  db->update(query, factory.factoryFile.c_str(), factory.factoryName.c_str(),
    factory.boundingBox.Min().x, factory.boundingBox.Min().y, factory.boundingBox.Min().z,
    factory.boundingBox.Max().x, factory.boundingBox.Max().y, factory.boundingBox.Max().z);
}

Geom::Box FactoriesTable::GetBB(const std::string& factoryFile, const std::string& factoryName)
{
  const char* query = {"select * 
                       "from factories"
                       "where factoryFile='%s' AND factoryName='%s';"};
  ResultSet* rs = db->query(query, factoryFile.c_str(), factoryName.c_str());
  if (!rs)
  {
    printf("E: No such factory '%s' - '%s'\n", factoryFile.c_str(), factoryName.c_str());
    return Geom::Box();
  }
  delete rs;
  return GetBB(rs, O);
}

void FactoriesTable::DropTable()
{
  db->update("drop table factories;");
}

std::string GetFactoryFile(ResultSet* rs, size_t row)
{
  return rs->GetData(row, 0);
}

std::string GetFactoryName(ResultSet* rs, size_t row)
{
  return rs->GetData(row, 1);
}

Geom::Vector3 GetPosition(ResultSet* rs, size_t row, size_t offset = 2)
{
  float x = atof(rs->GetData(row, offset+0));
  float y = atof(rs->GetData(row, offset+1));
  float z = atof(rs->GetData(row, offset+2));
  return Geom::Vector3(x, y, z);
}

Geom::Box GetBB(ResultSet* rs, size_t row)
{
  Geom::Vector3 min = GetPosition(rs, row, 2);
  Geom::Vector3 max = GetPosition(rs, row, 5);
  return Geom::Box(min, max);
}

void ObjectsTable::GetAll(Array<Factory>& factories)
{
  ResultSet* rs = db->query("select * from factories;");
  if (!rs) return;
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    Factory factory;
    factory.factoryFile = GetFactoryFile(rs, i);
    factory.factoryName = GetFactoryName(rs, i);
    factory.worldBB = GetWorldBB(rs, i);
    factories.add(factory);
  }
  delete rs;
}
