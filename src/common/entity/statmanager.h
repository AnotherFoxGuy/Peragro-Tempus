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

#ifndef _STATMANAGER_H_
#define _STATMANAGER_H_

#include "common/util/array.h"
#include "stat.h"
#include "server/database/table-stats.h"

class StatManager
{
private:
  Array<Stat*> stats;

public:
  StatManager() {}

  ~StatManager()
  {
    for (size_t i=0; i<stats.getCount(); i++)
      delete stats.get(i);
  }

  size_t getStatCount()
  {
    return stats.getCount();
  }

  Stat* getStat(size_t index)
  {
    return stats.get(index);
  }

  void addStat(Stat* stat)
  {
    stats.add(stat);
  }

  void delStat(Stat* stat)
  {
    if (!stat) return;
    //ptString name = stat->getName();
    //if (!name) return;
    for (size_t i = 0; i<stats.getCount(); i++)
    {
      Stat* _stat = stats.get(i);
      if (_stat->getId() == stat->getId())
      {
        stats.remove(i);
        return;
      }
    }
  }

  bool exists(Stat* stat)
  {
    if (!stat) return false;
    for (size_t i = 0; i<stats.getCount(); i++)
    {
      Stat* _stat = stats.get(i);

      if (_stat->getId() == stat->getId())
        return true;
    }
    return false;
  }

  Stat* findByName(ptString name)
  {
    if (name == ptString::Null) return 0;
    for (size_t i = 0; i<stats.getCount(); i++)
    {
      Stat* stat = stats.get(i);
      if (stat->getName() == name)
      {
        return stat;
      }
    }
    return 0;
  }

  Stat* findById(int id)
  {
    for (size_t i = 0; i<stats.getCount(); i++)
    {
      Stat* stat = stats.get(i);
      if (stat->getId() == id)
      {
        return stat;
      }
    }
    return 0;
  }

  void loadFromDB(StatTable* st)
  {
    //Load all Stats from Database
    st->getAllStats(stats);
  }

  void clear()
  {
    stats.removeAll();
  }
};

#endif // _STATMANAGER_H_
