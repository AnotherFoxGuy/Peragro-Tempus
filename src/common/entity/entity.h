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

#include <string>

#include <time.h>
#include <math.h>

#include "inventory.h"

class Item;

class Entity
{
private:
  int id;
  std::string name;
  std::string mesh;
  std::string sector;

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
  Entity() : id(-1)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  ~Entity()
  {
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
      return this->name == other->name;
    }

    return false;
  }

  void setId(int id)
  {
    this->id = id;
  }
  int getId() const
  {
    return id;
  }

  void setPos(float x, float y, float z)
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
  }
  void setPos(const float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  const float* getPos() const
  {
    return pos;
  }

  std::string getName() const
  {
    return name;
  }

  void setName(const std::string& name)
  {
    this->name = name;
  }

  std::string getMesh() const
  {
    return mesh;
  }

  void setMesh(const std::string& name)
  {
    this->mesh = name;
  }

  std::string getSector() const
  {
    return sector;
  }
  void setSector(std::string name)
  {
    this->sector = name;
  }

  void setType(int e)
  {
    type = EntityType(e);
  }

  EntityType getType() const
  {
    return type;
  }

  void setItem(int i)
  {
    item = i;
  }

  int getItem() const
  {
    return item;
  }

  Inventory* getInventory()
  {
    return &inventory;
  }
};

#endif // _ENTITY_H_
