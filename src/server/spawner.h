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

#include "server/entity/entity.h"
#include "server/entity/item.h"
#include "common/util/array.h"
#include "common/util/thread.h"
#include "common/util/sleep.h"
#include "server/server.h"

#include "server/database/table-spawnpoints.h"

class Spawner : public Timer
{
private:
  class SpawnPoint
  {
  public:
    float x, y, z;

    ptString sector_id;

    Item* item;
    unsigned int variation;

    size_t spawnInterval;
    size_t pickTime;

    int entity_id;

    SpawnPoint() : variation(0), pickTime(0), entity_id(0) {}
    ~SpawnPoint() { }
  };

  size_t timeCounter;

  Array<SpawnPoint*> spawnpoints;

  Mutex mutex;

  // Thread implementation
  void timeOut()
  {
    mutex.lock();
    for (size_t i=0; i<spawnpoints.getCount(); i++)
    {
      checkSpawnPoint(spawnpoints.get(i));
    }
    timeCounter++;
    mutex.unlock();
  }

  void checkSpawnPoint(SpawnPoint* sp)
  {
    const Entity* entity = Server::getServer()->getEntityManager()->findById(sp->entity_id);
    if (!entity)
    {
      if ( sp->pickTime == 0)
      {
        sp->pickTime = timeCounter;
      }
      if (timeCounter - sp->pickTime > sp->spawnInterval)
      {
        ItemEntity* item_ent = new ItemEntity();
        item_ent->createFromItem(sp->item->getId(), sp->variation);

        Entity* e = item_ent->getEntity()->getLock();
        e->setPos(sp->x, sp->y, sp->z);
        e->setSector(sp->sector_id);
        e->freeLock();

        Server::getServer()->addEntity(e, false);
        sp->entity_id = e->getId();
        sp->pickTime = 0;
      }
    }
  }

  void addSpawnPoint(float x, float y, float z, ptString sector, unsigned int item_id, unsigned int spawnInterval)
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

    mutex.lock();
    spawnpoints.add(sp);
    mutex.unlock();
  }

  int max_id;

public:
  Spawner() : timeCounter(0), max_id(-1) { this->setInterval(10); }
  ~Spawner() { spawnpoints.delAll(); }

  void loadFromDB(SpawnPointsTable* table)
  {
    Array<SpawnPointsTableVO*> points = table->getAll();
    for (size_t i = 0; i < points.getCount(); i++)
    {
      SpawnPointsTableVO* p = points.get(i);
      addSpawnPoint(p->pos_x, p->pos_y, p->pos_z, p->sector, p->item, p->interval);

      if (p->id > max_id) max_id = p->id;
    }
  }

  size_t getSpawnPointCount() const { return spawnpoints.getCount(); }

  void createSpawnPoint(float x, float y, float z, ptString sector, unsigned int item_id, unsigned int spawnInterval)
  {
    SpawnPointsTable* table = Server::getServer()->getDatabase()->getSpawnPointsTable();
    SpawnPointsTableVO p;
    p.pos_x = x;
    p.pos_y = y;
    p.pos_z = z;
    p.sector = sector;
    p.item = item_id;
    p.interval = spawnInterval;
    p.id = ++max_id;
    table->insert(&p);
    addSpawnPoint(p.pos_x, p.pos_y, p.pos_z, p.sector, p.item, p.interval);
  }

  void removeAllSpawnPoints()
  {
    SpawnPointsTable* table = Server::getServer()->getDatabase()->getSpawnPointsTable();
    table->truncate();

    mutex.lock();
    spawnpoints.delAll();
    max_id = 0;
    mutex.unlock();
  }
};

#endif // _SPAWNER_H_
