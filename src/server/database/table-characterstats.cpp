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

#include "table-characterstats.h"

#include "server/entity/characterstats.h"

CharacterStatsTable::CharacterStatsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from characterstat;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void CharacterStatsTable::createTable()
{
  printf("Creating Table characterstat...\n");
  db->update("create table characterstat ("
    "id INTEGER, "
    "stat INTEGER, "
    "level INTEGER, "
    "PRIMARY KEY (id, stat) );");
}

void CharacterStatsTable::insert(int characterstat, int stat, int level)
{
  db->update("insert or replace into characterstat (id, stat, level) values ('%d','%d','%d');",
    characterstat, stat, level);
}

void CharacterStatsTable::set(int characterstat, Stat* stat, int level)
{
  insert(characterstat, stat->getId(), level);
}

int CharacterStatsTable::get(int characterstat, Stat* stat)
{
  ResultSet* rs = db->query("select * from characterstat where id = '%d' and stat = '%d';",
    characterstat, stat->getId());

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
    printf("DB inconsistency: multiple equal stats in the characterstat!");
  }

  delete rs;

  return level;
}

void CharacterStatsTable::dropTable()
{
  db->update("drop table characterstat;");
}

void CharacterStatsTable::getAllEntries(Array<CharStat*>& entries, int id)
{
  ResultSet* rs = db->query("select stat, level from characterstat where id = '%d';", id);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    CharStat* entry = new CharStat();
    int stat_id = atoi(rs->GetData(i,0).c_str());
    entry->stat_id = stat_id;
    entry->level = atoi(rs->GetData(i,1).c_str());
    entries.add(entry);
  }
  delete rs;
}  
