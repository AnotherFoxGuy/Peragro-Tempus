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
  Array<Entity*> entities;

  friend class EntityManager;

public:
  EntityList() {}

  size_t getEntityCount()
  {
    return entities.getCount();
  }

  Entity* getEntity(size_t index)
  {
    return entities.get(index);
  }

  void addEntity(Entity* entity)
  {
    entities.add(entity);
  }

  void delEntity(size_t index)
  {
    entities.del(index);
  }

  void delEntity(Entity* entity)
  {
    if (!entity) return;
    const char* name = entity->getName();
    if (!name) return;
    for (size_t i = 0; i<entities.getCount(); i++)
    {
      Entity* _entity = entities.get(i);
      if (_entity->compare(entity))
      {
        entities.remove(i);
        return;
      }
    }
  }

  bool exists(Entity* entity)
  {
    if (!entity) return false;
    const char* name = entity->getName();
    if (!name) return false;
    for (size_t i = 0; i<entities.getCount(); i++)
    {
      Entity* _entity = entities.get(i);

      if (_entity->compare(entity))
        return true;
    }
    return false;
  }

  Entity* findByName(const char* name)
  {
    if (!name) return 0;
    for (size_t i = 0; i<entities.getCount(); i++)
    {
      Entity* entity = entities.get(i);
      if (strlen(entity->getName()) == strlen(name) && !strcmp(entity->getName(), name))
      {
        return entity;
      }
    }
    return 0;
  }

  Entity* findById(int id)
  {
    for (size_t i = 0; i<entities.getCount(); i++)
    {
      Entity* entity = entities.get(i);
      if (entity->getId() == id)
      {
        return entity;
      }
    }
    return 0;
  }

  void clear()
  {
    entities.removeAll();
  }
};

#endif // _ENTITYLIST_H_
