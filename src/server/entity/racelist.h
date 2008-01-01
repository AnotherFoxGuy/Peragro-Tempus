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

#ifndef RACELIST_H
#define RACELIST_H

#include "common/util/array.h"
#include "race.h"
#include "server/database/table-races.h"

class RaceManager;

class RaceList
{
private:
  Array<Race*> races;

  friend class RaceManager;

public:
  RaceList() {}
  ~RaceList() {}

  size_t getRaceCount()
  {
    return races.getCount();
  }

  Race* getRace(size_t index)
  {
    return races.get(index);
  }

  void addRace(Race* race)
  {
    if (!race) assert("Invalid Race");
    races.add(race);
  }

  void delRace(size_t index)
  {
    races.del(index);
  }

  void delRace(Race* race)
  {
    if (!race) return;
    //ptString name = race->getName();
    //if (!name) return;
    for (size_t i = 0; i<races.getCount(); i++)
    {
      Race* _race = races.get(i);
      if (!_race) 
      {
        races.remove(i);
        delRace(race);
      }
      if (_race->compare(race))
      {
        races.remove(i);
        return;
      }
    }
  }

  bool exists(Race* race)
  {
    if (!race) return false;
    for (size_t i = 0; i<races.getCount(); i++)
    {
      Race* _race = races.get(i);

      if (_race->compare(race))
        return true;
    }
    return false;
  }

  Race* findByName(ptString name)
  {
    if (name.isNull()) return 0;
    for (size_t i = 0; i<races.getCount(); i++)
    {
      Race* race = races.get(i);
      if (race->getName() == name)
      {
        return race;
      }
    }
    return 0;
  }

  Race* findById(int id)
  {
    for (size_t i = 0; i<races.getCount(); i++)
    {
      Race* race = races.get(i);
      if (race->getId() == id)
      {
        return race;
      }
    }
    return 0;
  }

  void clear()
  {
    races.removeAll();
  }
};

#endif // RACELIST_H
