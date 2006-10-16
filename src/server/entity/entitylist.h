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

#ifndef _ENTITYLIST_H_
#define _ENTITYLIST_H_

#include "common/util/array.h"
#include "entity.h"
#include "server/database/table-entities.h"

class EntityManager;

class EntityList
{
private:
  Array<const Entity*> entities;

  friend class EntityManager;

  Mutex mutex;

public:
  EntityList() {}

  size_t getEntityCount()
  {
    return entities.getCount();
  }

  const Entity* getEntity(size_t index)
  {
    return entities.get(index);
  }

  void addEntity(const Entity* entity)
  {
    if (!entity) assert("Invalid Entity");
    entities.add(entity);
  }

  void removeEntity(size_t index)
  {
    entities.remove(index);
  }

  void removeEntity(const Entity* entity)
  {
    if (!entity) return;

    mutex.lock();

    for (size_t i = 0; i<entities.getCount(); i++)
    {
      const Entity* _entity = entities.get(i);
      if (!_entity) 
      {
        entities.remove(i);
        removeEntity(entity);
      }
      if (_entity->compare(entity))
      {
        entities.remove(i);
        mutex.unlock();
        return;
      }
    }
    mutex.unlock();
  }

  bool exists(const Entity* entity)
  {
    if (!entity) return false;

    mutex.lock();

    for (size_t i = 0; i<entities.getCount(); i++)
    {
      const Entity* _entity = entities.get(i);

      if (_entity->compare(entity))
      {
        mutex.unlock();
        return true;
      }
    }
    mutex.unlock();
    return false;
  }

  const Entity* findByName(ptString name)
  {
    if (name.isNull()) return 0;
    mutex.lock();
    for (size_t i = 0; i<entities.getCount(); i++)
    {
      const Entity* entity = entities.get(i);
      if (entity->getName() == name)
      {
        mutex.unlock();
        return entity;
      }
    }
    mutex.unlock();
    return 0;
  }

  const Entity* findById(int id)
  {
    mutex.lock();
    for (size_t i = 0; i<entities.getCount(); i++)
    {
      const Entity* entity = entities.get(i);
      if (entity->getId() == id)
      {
        mutex.unlock();
        return entity;
      }
    }
    mutex.unlock();
    return 0;
  }

  void clear()
  {
    mutex.lock();
    entities.removeAll();
    mutex.unlock();
  }
};

#endif // _ENTITYLIST_H_
