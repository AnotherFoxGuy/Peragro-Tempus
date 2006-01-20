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

#include <vector>

#include <assert.h>

#include "entity.h"
#include "server/database/table-entities.h"

class EntityManager;

class EntityList
{
private:
  std::vector<Entity*> entities;

  friend class EntityManager;

public:
  EntityList() {}

  size_t getEntityCount() const
  {
    return entities.size();
  }

  Entity* getEntity(size_t index) const
  {
    return entities[index];
  }

  void addEntity(Entity* entity)
  {
    if (!entity) assert("Invalid Entity");
    entities.push_back(entity);
  }

  void delEntity(size_t index)
  {
    entities.erase(entities.begin()+index);
  }

  void delEntity(Entity* entity)
  {
    if (!entity) 
    {
      printf("Removing entity: Not a entity!\n");
      return;
    }

    
    for (size_t i = 0; i < entities.size(); i++)
    {
      Entity* tmp_entity = entities[i];
      if (tmp_entity->getName() == entity->getName())
      {
        entities.erase(entities.begin()+i);
        printf("Removing entity: Entity removed!\n");
        return;
      }
    }
    printf("Removing entity: Entity not found!\n");
  }

  Entity* findByName(const std::string& name) const
  {
    if (name.length() == 0) return 0;
    for (size_t i = 0; i < entities.size(); i++)
    {
      Entity* entity = entities[i];
      if (entity->getName() == name)
      {
        return entity;
      }
    }
    return 0;
  }

  bool exists(Entity* entity) const
  {
    if (!entity) return false;
    std::string name = entity->getName();
    if (name.length() == 0) return false;
    for (size_t i = 0; i < entities.size(); i++)
    {
      Entity* _entity = entities[i];

      if (_entity->compare(entity))
        return true;
    }
    return false;
  }

  Entity* findById(int id) const
  {
    for (size_t i = 0; i < entities.size(); i++)
    {
      Entity* entity = entities[i];
      if (entity->getId() == id)
      {
        return entity;
      }
    }
    return 0;
  }

  void clear()
  {
    entities.clear();
  }
};

#endif // _ENTITYLIST_H_
