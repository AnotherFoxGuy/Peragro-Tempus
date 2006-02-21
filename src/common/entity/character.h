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

#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <string.h>
#include <time.h>
#include <math.h>

#include "inventory.h"
#include "common/util/stringstore.h"

class User;

class Character
{
private:
  int id;
  //char* name;
  size_t name_id;
  //char* mesh;
  size_t mesh_id;
  //char* sector;
  size_t sector_id;

  User* user;

  float pos[3];

  Inventory inventory;

public:
  Character() : id(-1), name_id(-1), mesh_id(-1), sector_id(-1)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  virtual ~Character()
  {
    //delete [] name;
    //delete [] mesh;
    //delete [] sector;
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

  User* getUser()
  {
    return user;
  }

  void setUser(User* user)
  {
    this->user = user;
  }

  Inventory* getInventory()
  {
    return &inventory;
  }
};

#endif // _CHARACTER_H_
