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

#include <string>

#include "inventory.h"

class User;

class Character
{
private:
  int id;
  std::string name;
  std::string mesh;
  std::string sector;

  User* user;

  float pos[3];

  Inventory inventory;

public:
  Character() : id(-1)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;
  }

  ~Character()
  {
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
  
  void setSector(const std::string& name)
  {
    this->sector = name;
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

