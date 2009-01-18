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

#include "table-stats.h"
#include "server/entity/stat.h"

#define ADD_TO_DB(name) insert(ptString(name, strlen(name)));

StatTable::StatTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from stats;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void StatTable::createTable()
{
  printf("Creating Table stats...\n");
  db->update("create table stats ("
    "id INTEGER, "
    "name TEXT, "
    "PRIMARY KEY (id) );");

  //Example Data!
  insert(ptString("Health", strlen("Health")));
  insert(ptString("Mana", strlen("Mana")));
  insert(ptString("Strength", strlen("Strength")));
  insert(ptString("Intelligence", strlen("Intelligence")));
  insert(ptString("Dexterty", strlen("Dexterty")));
  insert(ptString("Endurance", strlen("Endurance")));
  insert(ptString("Agility", strlen("Agility")));
  insert(ptString("AgilityXP", strlen("AgilityXP")));
  insert(ptString("Concentration", strlen("Concentration")));
  insert(ptString("Witness", strlen("Witness")));
  insert(ptString("Speed", strlen("Speed")));
  insert(ptString("Sapience", strlen("Sapience")));
  ADD_TO_DB("Reach");
  ADD_TO_DB("Stamina");
  // Skills (attack)
  ADD_TO_DB("OneHandedAxe")
  ADD_TO_DB("OneHandedBlunt")
  ADD_TO_DB("OneHandedFlail")
  ADD_TO_DB("OneHandedSword")
  ADD_TO_DB("OneHandedUnarmed")
  ADD_TO_DB("TwoHandedAxe")
  ADD_TO_DB("TwoHandedBlunt")
  ADD_TO_DB("TwoHandedPolearm")
  ADD_TO_DB("TwoHandedSword")
  ADD_TO_DB("RangedBow")
  ADD_TO_DB("RangedCrossbow")
  ADD_TO_DB("RangedThrown")
  // Skills (defence)
  ADD_TO_DB("ArmorProficiency")
  ADD_TO_DB("Block")
  ADD_TO_DB("Dodge")
  ADD_TO_DB("Parry")
  // Skills (styles)
  ADD_TO_DB("DirtyTricks")
  ADD_TO_DB("SwiftnessOfTheWind")
  ADD_TO_DB("StrengthOfTheMountain")
  ADD_TO_DB("TwoWeapons")
  // Skills (rogue)
  ADD_TO_DB("Sneak")
  ADD_TO_DB("PickLocks")
  ADD_TO_DB("PickPockets")
  ADD_TO_DB("Perception")

}

void StatTable::insert(ptString name)
{
  db->update("insert into stats (name) values ('%q');", *name);
}

void StatTable::dropTable()
{
  db->update("drop table stats;");
}

bool StatTable::existsStat(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  ResultSet* rs = db->query("select id from stats where name = '%q';", *name);
  if (!rs) {
    return false;
  }
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

Stat* StatTable::getStat(ptString name)
{
  ResultSet* rs = db->query("select * from stats where name = '%q';", *name);

  Stat* stat = 0;

  if (rs && rs->GetRowCount() == 1)
  {
    stat = new Stat();
    stat->setId(atoi(rs->GetData(0,0).c_str()));
    stat->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
  }
  delete rs;
  return stat;
}

void StatTable::getAllStats(Array<Stat*>& stats)
{
  ResultSet* rs = db->query("select * from stats;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Stat* stat = new Stat();
    stat->setId(atoi(rs->GetData(i,0).c_str()));
    stat->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
    stats.add(stat);
  }
  delete rs;
}
