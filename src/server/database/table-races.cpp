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

#include "table-races.h"

#include "server/entity/race.h"
#include "server/entity/user.h"

RaceTable::RaceTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from races;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void RaceTable::createTable()
{
  printf("Creating Table races...\n");
  db->update("create table races ("
    "id INTEGER, "
    "name TEXT, "
    "mesh TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");

  float pos[3] = { 642.9f, 14.7f, 371.2f };
  ptString test("test", 4);
  ptString room("World", 5);

  insert(1, test, test, pos, room);
}

void RaceTable::insert(int id, ptString name, ptString mesh, float pos[3], ptString sector)
{
  const char* query = "insert into races (id, name, mesh, pos_x, pos_y, pos_z, sector) values ('%d','%q','%q',%.2f,%.2f,%.2f,'%q');";
  db->update(query, id, *name, *mesh, pos[0], pos[1], pos[2], *sector);
}

int RaceTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from races");
  if (!rs)
  {
    return 0;
  }
  if (rs->GetRowCount() == 0) 
    return 0;

  int id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

void RaceTable::dropTable()
{
  db->update("drop table races;");
}

void RaceTable::remove(int id)
{
  db->update("delete from races where id = %d;", id);
}

bool RaceTable::existsRace(ptString name)
{
  ResultSet* rs = db->query("select id from races where name = '%q';", *name);
  if (!rs)
  {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Race* RaceTable::findRaceById(int id)
{
  ResultSet* rs = db->query("select * from races where id = '%d';", id);
  if (!rs || rs->GetRowCount() == 0) 
    return 0;

  Race* race = new Race();
  race->setId(atoi(rs->GetData(0,0).c_str()));
  race->setName(ptString(rs->GetData(0,1).c_str(),rs->GetData(0,1).length()));
  race->setMesh(ptString(rs->GetData(0,2).c_str(),rs->GetData(0,2).length()));
  race->setPos((float)atof(rs->GetData(0,3).c_str()), (float)atof(rs->GetData(0,4).c_str()), (float)atof(rs->GetData(0,5).c_str()));
  race->setSector(ptString(rs->GetData(0,6).c_str(),rs->GetData(0,6).length()));
  delete rs;

  return race;
}

void RaceTable::getAllRaces(Array<Race*>& races)
{
  ResultSet* rs = db->query("select * from races;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Race* race = new Race();
    race->setId(atoi(rs->GetData(i,0).c_str()));
    race->setName(ptString(rs->GetData(i,1).c_str(),rs->GetData(0,1).length()));
    race->setMesh(ptString(rs->GetData(0,2).c_str(),rs->GetData(0,2).length()));
    race->setPos((float)atof(rs->GetData(0,3).c_str()), (float)atof(rs->GetData(0,4).c_str()), (float)atof(rs->GetData(0,5).c_str()));
    race->setSector(ptString(rs->GetData(0,6).c_str(),rs->GetData(0,6).length()));
    races.add(race);

    race->getStats()->loadFromDatabase(db->getTables()->getRaceStatsTable(), race->getId());
    race->getSkills()->loadFromDatabase(db->getTables()->getRaceSkillsTable(), race->getId());
  }
  delete rs;
}  
