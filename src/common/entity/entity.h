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

#include "common/util/ptstring.h"

#include "inventory.h"
#include "characterstats.h"

class Item;

class Entity
{
private:
  int id;

  ptString name_id;
  ptString mesh_id;
  ptString sector_id;

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

protected:
  EntityType type;

  float pos[3];

public:
  Entity() : id(-1)//, name(0), mesh(0), sector(0)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  Entity(EntityType type) : id(-1), type(type)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  virtual ~Entity() {}

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
  virtual float* getPos()
  {
    return pos;
  }

  ptString& getName()
  {
    return name_id;
  }
  void setName(ptString id)
  {
    name_id = id;
  }

  ptString& getMesh()
  {
    return mesh_id;
  }
  void setMesh(ptString id)
  {
    mesh_id = id;
  }

  ptString& getSector()
  {
    return sector_id;
  }
  void setSector(ptString id)
  {
    sector_id = id;
  }

  EntityType getType()
  {
    return type;
  }

};

#endif // _ENTITY_H_
