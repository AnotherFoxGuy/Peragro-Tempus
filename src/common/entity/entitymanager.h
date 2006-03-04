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

#ifndef _ENTITYMANAGER_H_
#define _ENTITYMANAGER_H_

#include "common/util/array.h"
#include "entity.h"
#include "characterentity.h"
#include "pcentity.h"
#include "npcentity.h"
#include "itementity.h"
#include "entitylist.h"
#include "server/database/table-entities.h"

class EntityManager
{
private:
  EntityList entity_list;
  EntityList character_list;
  unsigned int ent_id;

public:
  EntityManager() : ent_id(0) {}

  ~EntityManager()
  {
    for (size_t i=0; i<entity_list.getEntityCount(); i++)
      delete entity_list.getEntity(i);
  }

  size_t getCharEntityCount()
  {
    return character_list.getEntityCount();
  }

  CharacterEntity* getCharEntity(size_t index)
  {
    return (CharacterEntity*) character_list.getEntity(index);
  }

  size_t getEntityCount()
  {
    return entity_list.getEntityCount();
  }

  Entity* getEntity(size_t index)
  {
    return entity_list.getEntity(index);
  }

  void addEntity(Entity* entity)
  {
    ent_id++;
    entity->setId(ent_id);
    entity_list.addEntity(entity);
    if (entity->getType() == Entity::PlayerEntity || entity->getType() == Entity::NPCEntity)
    {
      character_list.addEntity(entity);
    }
  }

  void removeEntity(Entity* entity)
  {
    if (entity->getType() == Entity::PlayerEntity || entity->getType() == Entity::NPCEntity)
    {
      character_list.removeEntity(entity);
    }
    entity_list.removeEntity(entity);
  }

  bool exists(Entity* entity)
  {
    return entity_list.exists(entity);
  }

  Entity* findByName(ptString name)
  {
    return entity_list.findByName(name);
  }

  Entity* findById(int id)
  {
    return entity_list.findById(id);
  }

  void loadFromDB(EntityTable* et)
  {
    ent_id = et->getMaxId();
    //Load all Entities from Database
    et->getAllEntities(entity_list.entities);
  }
};

#endif // _ENTITYMANAGER_H_
