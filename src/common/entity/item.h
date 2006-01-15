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

#ifndef ITEM_H_
#define ITEM_H_

#include <string.h>

class Item
{
private:
  int id;
  char* name;
  char* mesh;

public:
  Item() : id(-1), name(0), mesh(0) {}
  ~Item() {}

  void setId(int id) { this->id = id; }
  int getId() { return id; }

  const char* getName() { return name; }
  void setName(const char* name) { setName(name, strlen(name)); }
  void setName(const char* name, size_t namelen)
  {
    delete [] this->name;
    this->name = new char[namelen+1];
    strncpy(this->name, name, namelen+1);
  }

  const char* getMesh() { return mesh; }
  void setMesh(const char* name) { setMesh(name, strlen(name)); }
  void setMesh(const char* name, size_t namelen)
  {
    delete [] this->mesh;
    this->mesh = new char[namelen+1];
    strncpy(this->mesh, name, namelen+1);
  }
};

#endif // ITEM_H_
