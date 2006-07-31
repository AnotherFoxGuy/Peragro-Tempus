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

#ifndef CHARACTERSTATS_H_
#define CHARACTERSTATS_H_

#include "stat.h"
#include "server/database/table-characterstats.h"

class CharStat
{
public:
  int stat_id;
  int level;
};

class Connection;

class CharacterStats
{
private:
  Array<CharStat*> entries;

  CharStat* findEntry(int stat_id)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i)->stat_id == stat_id)
      {
        return entries.get(i);
      }
    }
    return 0;
  }

  CharacterStatsTable* cstab;

  int cs_id;

public:
  CharacterStats() : cstab(0) {}
  ~CharacterStats() {}

  void addStat(Stat* stat, int level)
  {
    CharStat* entry = findEntry(stat->getId());
    if (!entry)
    {
      entry = new CharStat();
      entry->stat_id = stat->getId();
      entry->level = level;
      entries.add(entry);
    }
    else
    {
      entry->level += level;
    }
    if (cstab) cstab->set(cs_id, stat, entry->level);
  }

  bool takeStat(Stat* stat, int level)
  {
    CharStat* entry = findEntry(stat->getId());
    if (!entry) 
      return false;
    if (entry->level < level)
    {
      entry->level = 0;
      return false;
    }

    entry->level -= level;

    if (cstab) cstab->set(cs_id, stat, entry->level);

    return true;
  }

  unsigned int getAmount(Stat* stat)
  {
    CharStat* entry = findEntry(stat->getId());
    return (entry?entry->level:0);
  }

  void loadFromDatabase(CharacterStatsTable* cst, int id)
  {
    cs_id = id;
    cstab = cst;

    //Load all Stats from Database
    cst->getAllEntries(entries, id);
  }

  void sendAllStats(Connection* conn);
};

#endif // CHARACTERSTATS_H_
