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

#include "inventory.h"

class Item;

class Entity
{
private:
  int id;
  char* name;
  char* mesh;
  char* sector;

  float pos[3];

  int item;

  Inventory inventory;

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
  Entity() : id(-1), name(0), mesh(0), sector(0)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  ~Entity()
  {
    delete [] name;
    delete [] mesh;
    delete [] sector;
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
      if (strlen(this->name) != strlen(other->name))
        return false;

      return !strcmp(this->name, other->name);
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
    return name;
  }

  void setName(const char* name, size_t namelen)
  {
    delete [] this->name;
    this->name = new char[namelen+1];
    strncpy(this->name, name, namelen+1);
  }

  void setName(const char* name)
  {
    setName(name, strlen(name));
  }

  const char* getMesh()
  {
    return mesh;
  }

  void setMesh(const char* name, size_t namelen)
  {
    delete [] this->mesh;
    this->mesh = new char[namelen+1];
    strncpy(this->mesh, name, namelen+1);
  }

  void setMesh(const char* name)
  {
    setMesh(name, strlen(name));
  }

  const char* getSector()
  {
    return sector;
  }
  void setSector(const char* name, size_t namelen)
  {
    delete [] this->sector;
    this->sector = new char[namelen+1];
    strncpy(this->sector, name, namelen+1);
  }
  void setSector(const char* name)
  {
    setSector(name, strlen(name));
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

  void createFromItem(Item* item)
  {
    this->setName(item->getName());
    this->setMesh(item->getMesh());
    this->setType(Entity::ItemEntity);
    this->setItem(item->getId());
  }
};

#endif // _ENTITY_H_
