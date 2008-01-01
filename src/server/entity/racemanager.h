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

#ifndef RACEMANAGER_H
#define RACEMANAGER_H

#include "common/util/array.h"
#include "race.h"
#include "racelist.h"
#include "server/database/table-races.h"

class RaceManager
{
private:
  RaceList list;
  unsigned int ent_id;

public:
  RaceManager() : ent_id(0) {}

  ~RaceManager()
  {
    for (size_t i = 0; i < list.getRaceCount(); i++)
    {
      delete list.getRace(i);
    }
  }

  size_t getRaceCount()
  {
    return list.getRaceCount();
  }

  Race* getRace(size_t index)
  {
    return list.getRace(index);
  }

  void addRace(Race* race)
  {
    ent_id++;
    race->setId(ent_id);
    list.addRace(race);
  }

  void delRace(size_t index)
  {
    list.delRace(index);
  }

  void delRace(Race* race)
  {
    list.delRace(race);
  }

  bool exists(Race* race)
  {
    return list.exists(race);
  }

  Race* findByName(ptString name)
  {
    return list.findByName(name);
  }

  Race* findById(int id)
  {
    return list.findById(id);
  }

  void loadFromDB(RaceTable* rt)
  {
    ent_id = rt->getMaxId();
    //Load all Races from Database
    rt->getAllRaces(list.races);
  }
};

#endif // RACEMANAGER_H
