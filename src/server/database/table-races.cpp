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

#include "common/entity/race.h"
#include "server/user.h"

RaceTable::RaceTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from races;");
  delete rs;
  if (rs == 0)
  {
    createTable();
  }
  ResultSet* rs2 = db->query("select count(*) from races;");
}

void RaceTable::createTable()
{
  db->update("create table races ("
    "id INTEGER, "
    "name TEXT, "
    "mesh TEXT, "
    "pos_x FLOAT, "
    "pos_y FLOAT, "
    "pos_z FLOAT, "
    "sector TEXT, "
    "PRIMARY KEY (id) );");

  float pos[3] = { 0.0f, 0.0f, 0.0f };
  insert(1, "test", "test", pos, "room");
}

void RaceTable::insert(int id, const char* name, const char* mesh, float pos[3], const char* sector)
{
  if (strlen(name) + strlen(mesh) + strlen(sector) > 512) assert("Strings too long");
  char query[1024];
  sprintf(query, "insert into races (id, name, mesh, pos_x, pos_y, pos_z, sector) values "
    "('%d','%s','%s',%.2f,%.2f,%.2f,'%s');", id, name, mesh, pos[0], pos[1], pos[2], sector);
  db->update(query);
}

int RaceTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from races");
  if (rs->GetRowCount() == 0) 
    return 0;

  return atoi(rs->GetData(0,0).c_str());
}

void RaceTable::dropTable()
{
  db->update("drop table races;");
}

void RaceTable::remove(int id)
{
  char query[1024];
  sprintf(query, "delete from races where id = %d;", id);
  db->update(query);
}

bool RaceTable::existsRace(const char* name)
{
  if (strlen(name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select id from races where name = '%s';", name);
  ResultSet* rs = db->query(query);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Race* RaceTable::findRaceById(int id)
{
  char query[1024];
  sprintf(query, "select * from races where id = '%d';", id);
  ResultSet* rs = db->query(query);
  if (rs->GetRowCount() == 0) 
    return 0;

  Race* race = new Race();
  race->setId(atoi(rs->GetData(0,0).c_str()));
  race->setName(rs->GetData(0,1).c_str());
  race->setMesh(rs->GetData(0,2).c_str());
  race->setPos((float)atof(rs->GetData(0,3).c_str()), (float)atof(rs->GetData(0,4).c_str()), (float)atof(rs->GetData(0,5).c_str()));
  race->setSector(rs->GetData(0,6).c_str());
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
    race->setName(rs->GetData(i,1).c_str());
    race->setMesh(rs->GetData(0,2).c_str());
    race->setPos((float)atof(rs->GetData(0,3).c_str()), (float)atof(rs->GetData(0,4).c_str()), (float)atof(rs->GetData(0,5).c_str()));
    race->setSector(rs->GetData(0,6).c_str());
    races.add(race);

    race->getStats()->loadFromDatabase(db->getRaceStatsTable(), race->getId());
  }
}  
