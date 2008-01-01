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

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "common/util/array.h"
#include "entity.h"
#include "pcentity.h"
#include "npcentity.h"
#include "itementity.h"
#include "doorentity.h"
#include "mountentity.h"
#include "entitylist.h"

class EntityManager
{
private:
  EntityList entity_list;
  unsigned int ent_id;

  Mutex mutex;

public:
  EntityManager() : ent_id(0) {}
  ~EntityManager()
  {
    for (size_t i = 0; i < entity_list.getEntityCount(); i++)
    {
      delete entity_list.getEntity(i);
    }
  }

  size_t getEntityCount()
  {
    return entity_list.getEntityCount();
  }

  const Entity* getEntity(size_t index)
  {
    return entity_list.getEntity(index);
  }

  void addEntity(const Entity* entity)
  {
    mutex.lock();
    ent_id++;
    
    Entity* e = entity->getLock();
    e->setId(ent_id);
    e->freeLock();

    entity_list.addEntity(entity);
    mutex.unlock();
  }

  void removeEntity(const Entity* entity)
  {
    mutex.lock();
    entity_list.removeEntity(entity);
    mutex.unlock();
  }

  bool exists(const Entity* entity)
  {
    return entity_list.exists(entity);
  }

  const Entity* findByName(ptString name)
  {
    return entity_list.findByName(name);
  }

  const Entity* findById(int id)
  {
    return entity_list.findById(id);
  }

  void lock() { mutex.lock(); }
  void unlock() { mutex.unlock(); }

  void loadFromDB(EntityTable* et);
};

#endif // ENTITYMANAGER_H
