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

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <string.h>
#include <time.h>
#include <math.h>

#include "common/util/stringstore.h"

#include "inventory.h"
#include "characterstats.h"

class Item;

class Entity
{
private:
  int id;
  //char* name;
  size_t name_id;
  //char* mesh;
  size_t mesh_id;
  //char* sector;
  size_t sector_id;

  float pos[3];

  int item;

  Inventory inventory;
  CharacterStats stats;

public:
  enum EntityType
  {
    PlayerEntity=0,
    NPCEntity=1,
    DoorEntity=2,
    ItemEntity=3
  };

private:
  EntityType type;

public:
  Entity() : id(-1)//, name(0), mesh(0), sector(0)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  ~Entity()
  {
    //delete [] name;
    //delete [] mesh;
    //delete [] sector;
  }

  bool compare(Entity* other)
  {
    if (this == other)
      return true;

    if (this->type != other->type)
      return false;

    if (this->type == Entity::ItemEntity)
    {
      return this->id == other->id;
    }
    else if (this->type == Entity::PlayerEntity)
    {
      return (this->name_id == other->name_id);

      //if (strlen(this->name) != strlen(other->name))
        //return false;

      //return !strcmp(this->name, other->name);
    }

    return false;
  }

  void setId(int id)
  {
    this->id = id;
  }
  int getId()
  {
    return id;
  }

  void setPos(float x, float y, float z)
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
  }
  void setPos(float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  float* getPos()
  {
    return pos;
  }

  const char* getName()
  {
    return StringStore::getStore()->lookupString(name_id);
  }
  size_t getNameId()
  {
    return name_id;
  }
  void setName(const char* name)
  {
    name_id = StringStore::getStore()->lookupId(name);
  }
  void setName(size_t id)
  {
    name_id = id;
  }

  const char* getMesh()
  {
    return StringStore::getStore()->lookupString(mesh_id);
  }
  size_t getMeshId()
  {
    return mesh_id;
  }
  void setMesh(const char* name)
  {
    mesh_id = StringStore::getStore()->lookupId(name);
  }
  void setMesh(size_t id)
  {
    mesh_id = id;
  }

  const char* getSector()
  {
    return StringStore::getStore()->lookupString(sector_id);
  }
  void setSector(const char* name)
  {
    sector_id = StringStore::getStore()->lookupId(name);
  }
  void setSector(size_t id)
  {
    sector_id = id;
  }

  void setType(int e)
  {
    type = EntityType(e);
  }

  EntityType getType()
  {
    return type;
  }

  void setItem(int i)
  {
    item = i;
  }

  int getItem()
  {
    return item;
  }

  Inventory* getInventory()
  {
    return &inventory;
  }

  CharacterStats* getStats()
  {
    return &stats;
  }

  void createFromItem(Item* item)
  {
    this->setName(item->getNameId());
    this->setMesh(item->getMeshId());
    this->setType(Entity::ItemEntity);
    this->setItem(item->getId());
  }
};

#endif // _ENTITY_H_
