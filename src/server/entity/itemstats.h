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

#ifndef ITEMSTATS_H_
#define ITEMSTATS_H_

#include "stat.h"
#include "server/database/table-itemstats.h"

class ItemStat
{
public:
  int stat_id;
  int level;
};

class Connection;

class ItemStats
{
private:
  Array<ItemStat*> entries;

  ItemStat* findEntry(int stat_id)
  {
    for(unsigned int i = 0; i < entries.getCount(); ++i)
    {
      if (entries.get(i)->stat_id == stat_id)
      {
        return entries.get(i);
      }
    }
    return 0;
  }

  const ItemStat* findEntry(int stat_id) const
  {
    for(unsigned int i = 0; i < entries.getCount(); ++i)
    {
      if (entries.get(i)->stat_id == stat_id)
      {
        return entries.get(i);
      }
    }
    return 0;
  }

  ItemStatsTable* cstab;

  int cs_id;

public:
  ItemStats() : cstab(0) {}
  ~ItemStats() { entries.delAll(); }

  void addStat(Stat* stat, int level)
  {
    ItemStat* entry = findEntry(stat->getId());
    if (!entry)
    {
      entry = new ItemStat();
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
    ItemStat* entry = findEntry(stat->getId());
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

  void setStat(Stat* stat, int level)
  {
    ItemStat* entry = findEntry(stat->getId());
    if (!entry)
    {
      entry = new ItemStat();
      entry->stat_id = stat->getId();
      entry->level = level;
      entries.add(entry);
    }
    else
    {
      entry->level = level;
    }
    if (cstab) cstab->set(cs_id, stat, entry->level);
  }

  unsigned int getAmount(const Stat* stat) const
  {
    const ItemStat* entry = findEntry(stat->getId());
    return (entry?entry->level:0);
  }

  void loadFromDatabase(ItemStatsTable* cst, int id)
  {
    cs_id = id;
    cstab = cst;

    //Load all Stats from Database
    cst->getAllEntries(entries, id);
  }

  void sendAllStats(Connection* conn) const;
};

#endif // ITEMSTATS_H_
