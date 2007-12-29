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

#ifndef _SECTORMANAGER_H_
#define _SECTORMANAGER_H_

#include "common/util/array.h"
#include "common/util/ptstring.h"
#include "common/util/printhelper.h"

#include "server/database/database.h"
#include "server/database/table-sectors.h"
#include "server/server.h"

namespace Sector
{
  static unsigned short NoSector = 0;
}

class SectorManager
{
private:
  Array<ptString> sectors;
  Array<ptString> regions;

  void loadSector(unsigned short id, ptString name, ptString region)
  {
    if (sectors.getCount()+1 == id)
    {
      sectors.add(name);
      regions.add(region);
    }
    else
      printf("Sectors out of order! Expected sector id %" SIZET 
             " but got %d!\n", sectors.getCount(), id);
  }

public:
  SectorManager() {}
  ~SectorManager() {}

  unsigned short getSectorCount()
  {
    return (unsigned short) sectors.getCount()+1;
  }

  unsigned short getSectorId(ptString name)
  {
    for (size_t i = 0; i < sectors.getCount(); i++)
    {
      if (sectors.get(i) == name) return (unsigned short) i + 1;
    }
    return Sector::NoSector;
  }

  const ptString& getSectorName(unsigned short id)
  {
    if (id > sectors.getCount()) return ptString::Null;
    return sectors.get(id-1);
  }

  const ptString& getRegionName(unsigned short id)
  {
    if (id > regions.getCount()) return ptString::Null;
    return regions.get(id-1);
  }

  void addSector(unsigned short id, ptString name, ptString region)
  {
    if (sectors.getCount()+1 == id)
    {
      SectorsTable* dt = Server::getServer()->getDatabase()->getSectorsTable();
      SectorsTableVO vo;
      vo.id = id;
      vo.name = name;
      vo.region = region;
      dt->insert(&vo);
      loadSector(id, name, region);
    }
    else
      printf("Sectors out of order! Expected sector id %" SIZET 
             " but got %d!\n", sectors.getCount(), id);
  }

  void delAll()
  {
    SectorsTable* dt = Server::getServer()->getDatabase()->getSectorsTable();
    Array<SectorsTableVO*> loadedSectors=dt->getAll();
    for (size_t i = 0; i<loadedSectors.getCount(); i++)
    {
      dt->remove(loadedSectors[i]->id);
    }
    sectors.removeAll();
    regions.removeAll();
  }

  void loadFromDB(SectorsTable* dt)
  {
    //Load all Sectors from Database
    Array<SectorsTableVO*> loadedSectors = dt->getAll();
    for (size_t i = 0; i < loadedSectors.getCount(); i++)
    {
      loadSector(loadedSectors[i]->id, loadedSectors[i]->name, loadedSectors[i]->region);
    }
  }
};

#endif // _SECTORMANAGER_H_
