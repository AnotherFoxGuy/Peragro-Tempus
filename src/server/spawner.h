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

#ifndef _SPAWNER_H_
#define _SPAWNER_H_

#include "common/entity/entity.h"
#include "common/entity/item.h"
#include "common/util/array.h"
#include "common/util/thread.h"
#include "common/util/sleep.h"
#include "server/server.h"

class Spawner : public Timer
{
private:
  class SpawnPoint
  {
  public:
    float x, y, z;

    ptString sector_id;

    Item* item;

    unsigned int spawnInterval;
    unsigned int pickTime;

    int entity_id;

    SpawnPoint() : pickTime(0), entity_id(0) {}
    ~SpawnPoint() { }
  };

  size_t timeCounter;

  Array<SpawnPoint*> spawnpoints;

  // Thread implementation
  void timeOut()
  {
    for (size_t i=0; i<spawnpoints.getCount(); i++)
    {
      checkSpawnPoint(spawnpoints.get(i));
    }
    timeCounter++;
  }

  void checkSpawnPoint(SpawnPoint* sp)
  {
    Entity* entity = Server::getServer()->getEntityManager()->findById(sp->entity_id);
    if (!entity)
    {
      if ( sp->pickTime == 0)
      {
        sp->pickTime = timeCounter;
      }
      if (timeCounter - sp->pickTime > sp->spawnInterval)
      {
        entity = new ItemEntity();
        ((ItemEntity*)entity)->createFromItem(sp->item);
        entity->setPos(sp->x, sp->y, sp->z);
        entity->setSector(sp->sector_id);
        Server::getServer()->addEntity(entity, false);
        sp->entity_id = entity->getId();
        sp->pickTime = 0;
      }
    }
  }

public:
  Spawner() : timeCounter(0) { this->setInverval(10); }
  ~Spawner() { spawnpoints.delAll(); }

  void addSpawnPoint(float x, float y, float z, ptString sector, int item_id, unsigned int spawnInterval)
  {
    Item* item = Server::getServer()->getItemManager()->findById(item_id);
    if (!item) return;

    SpawnPoint* sp = new SpawnPoint();
    sp->sector_id = sector;
    sp->x = x;
    sp->y = y;
    sp->z = z;
    sp->item = item;
    sp->spawnInterval = spawnInterval;
    spawnpoints.add(sp);
  }
};

#endif // _SPAWNER_H_
