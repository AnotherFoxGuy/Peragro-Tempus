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

#include "table-racestats.h"
#include "table-stats.h"

#include "server/entity/racestats.h"

RaceStatsTable::RaceStatsTable(Database* db, StatTable* stats) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from racestat;");
  if (rs == 0)
  {
    createTable(stats);
  }
  delete rs;
}

void RaceStatsTable::createTable(StatTable* stats)
{
  printf("Creating Table racestat...\n");
  db->update("create table racestat ("
    "id INTEGER, "
    "stat INTEGER, "
    "level INTEGER, "
    "PRIMARY KEY (id, stat) );");

  Stat* stat = 0;

  stat = stats->getStat(ptString("Health", strlen("Health")));
  set(1, stat, 60);
  delete stat;

  stat = stats->getStat(ptString("Mana", strlen("Mana")));
  set(1, stat, 60);
  delete stat;

  stat = stats->getStat(ptString("Strength", strlen("Strength")));
  set(1, stat, 35);
  delete stat;

  stat = stats->getStat(ptString("Intelligence", strlen("Intelligence")));
  set(1, stat, 40);
  delete stat;

  stat = stats->getStat(ptString("Dexterty", strlen("Dexterty")));
  set(1, stat, 50);
  delete stat;

  stat = stats->getStat(ptString("Endurance", strlen("Endurance")));
  set(1, stat, 40);
  delete stat;

  stat = stats->getStat(ptString("Agility", strlen("Agility")));
  set(1, stat, 45);
  delete stat;

  stat = stats->getStat(ptString("Concentration", strlen("Concentration")));
  set(1, stat, 25);
  delete stat;

  stat = stats->getStat(ptString("Witness", strlen("Witness")));
  set(1, stat, 65);
  delete stat;

  stat = stats->getStat(ptString("Speed", strlen("Speed")));
  set(1, stat, 4);
  delete stat;

  stat = stats->getStat(ptString("Dodge", strlen("Dodge")));
  set(1, stat, 40);
  delete stat;

  stat = stats->getStat(ptString("Sapience", strlen("Sapience")));
  set(1, stat, 40);
  delete stat;

  stat = stats->getStat(ptString("Parry", strlen("Parry")));
  set(1, stat, 40);
  delete stat;

  stat = stats->getStat(ptString("Block", strlen("Block")));
  set(1, stat, 40);
  delete stat;
}

void RaceStatsTable::insert(int race, int stat, int level)
{
  db->update("insert or replace into racestat (id, stat, level) values ('%d','%d','%d');",
    race, stat, level);
}

void RaceStatsTable::set(int race, Stat* stat, int level)
{
  if (!stat)
  {
    return;
  }
  insert(race, stat->getId(), level);
}

int RaceStatsTable::get(int race, Stat* stat)
{
  if (!stat)
  {
    return 0;
  }
  ResultSet* rs = db->query("select * from racestat where id = '%d' and stat = '%d';",
    race, stat->getId());

  int level = 0;

  if (rs->GetRowCount() == 1)
  {
    level = atoi(rs->GetData(0,0).c_str());
  }
  else if (rs->GetRowCount() == 0)
  {
    level = 0;
  }
  else
  {
    printf("DB inconsistency: multiple equal stats in the racestat!");
  }

  delete rs;

  return level;
}

void RaceStatsTable::dropTable()
{
  db->update("drop table racestat;");
}

void RaceStatsTable::getAllEntries(Array<RaceStat*>& entries, int id)
{
  ResultSet* rs = db->query("select stat, level from racestat where id = '%d';", id);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    RaceStat* entry = new RaceStat();
    int stat_id = atoi(rs->GetData(i,0).c_str());
    entry->stat_id = stat_id;
    entry->level = atoi(rs->GetData(i,1).c_str());
    entries.add(entry);
  }
  delete rs;
}
