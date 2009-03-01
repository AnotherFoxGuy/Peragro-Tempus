/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "table-itemstats.h"

#include "server/entity/itemstats.h"

ItemStatsTable::ItemStatsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from itemstat;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void ItemStatsTable::createTable()
{
  printf("Creating Table itemstat...\n");
  db->update("create table itemstat ("
    "id INTEGER, "
    "stat INTEGER, "
    "level INTEGER, "
    "PRIMARY KEY (id, stat) );");
}

void ItemStatsTable::insert(int itemstat, int stat, int level)
{
  db->update("insert or replace into itemstat (id, stat, level) values ('%d','%d','%d');",
    itemstat, stat, level);
}

void ItemStatsTable::set(int itemstat, Stat* stat, int level)
{
  insert(itemstat, stat->GetId(), level);
}

int ItemStatsTable::get(int itemstat, Stat* stat)
{
  ResultSet* rs = db->query("select * from itemstat where id = '%d' and stat = '%d';",
    itemstat, stat->GetId());

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
    printf("DB inconsistency: multiple equal stats in the itemstat!");
  }

  delete rs;

  return level;
}

void ItemStatsTable::dropTable()
{
  db->update("drop table itemstat;");
}

void ItemStatsTable::getAllEntries(Array<ItemStat*>& entries, int id)
{
  ResultSet* rs = db->query("select stat, level from itemstat where id = '%d';", id);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    ItemStat* entry = new ItemStat();
    int stat_id = atoi(rs->GetData(i,0).c_str());
    entry->stat_id = stat_id;
    entry->level = atoi(rs->GetData(i,1).c_str());
    entries.add(entry);
  }
  delete rs;
}
