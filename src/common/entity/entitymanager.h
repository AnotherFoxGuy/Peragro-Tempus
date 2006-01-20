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
#include "entitylist.h"
#include "server/database/table-entities.h"

class EntityManager
{
private:
  EntityList list;
  unsigned int ent_id;

public:
  EntityManager() : ent_id(0) {}

  size_t getEntityCount()
  {
    return list.getEntityCount();
  }

  Entity* getEntity(size_t index)
  {
    return list.getEntity(index);
  }

  void addEntity(Entity* entity)
  {
    ent_id++;
    entity->setId(ent_id);
    list.addEntity(entity);
  }

  void delEntity(size_t index)
  {
    list.delEntity(index);
  }

  void delEntity(Entity* entity)
  {
    list.delEntity(entity);
  }

  bool exists(Entity* entity)
  {
    return list.exists(entity);
  }

  Entity* findByName(const char* name)
  {
    return list.findByName(name);
  }

  Entity* findById(int id)
  {
    return list.findById(id);
  }

  void loadFromDB(EntityTable* et)
  {
    ent_id = et->getMaxId();
    //Load all Entities from Database
    et->getAllEntities(list.entities);
  }
};

#endif // _ENTITYMANAGER_H_
