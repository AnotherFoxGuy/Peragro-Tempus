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

class User;

class Character
{
private:
  int id;
  char* name;
  char* mesh;
  char* sector;

  User* user;

  float pos[3];

  Inventory inventory;

public:
  Character() : id(-1), name(0), mesh(0), sector(0)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  ~Character()
  {
    delete [] name;
    delete [] mesh;
    delete [] sector;
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
