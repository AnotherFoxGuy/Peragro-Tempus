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

#ifndef RACESTATS_H_
#define RACESTATS_H_

#include "stat.h"
#include "server/database/database.h"
#include "server/database/table-racestats.h"
#include "server/database/table-characterstats.h"
#include "server/server.h"

class RaceStat
{
public:
  int stat_id;
  int level;
};

class Connection;

class RaceStats
{
private:
  Array<RaceStat*> entries;

  RaceStat* findEntry(int stat_id)
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

  RaceStatsTable* estab;

  int es_id;

public:
  RaceStats() : estab(0) {}
  ~RaceStats() { entries.delAll(); }

  unsigned int getAmount(Stat* stat)
  {
    RaceStat* entry = findEntry(stat->getId());
    return (entry?entry->level:0);
  }

  void copyToCharacter(int char_id)
  {
    CharacterStatsTable* cst = Server::getServer()->getDatabase()->getCharacterStatTable();
    for (size_t i=0; i<entries.getCount(); i++)
    {
      RaceStat* stat = entries.get(i);
      cst->insert(char_id, stat->stat_id, stat->level);
    }
  }

  void loadFromDatabase(RaceStatsTable* est, int id)
  {
    es_id = id;
    estab = est;

    //Load all Stats from Database
    est->getAllEntries(entries, id);
  }

  void sendAllStats(Connection* conn);
};

#endif // RACESTATS_H_
