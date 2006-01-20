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

#include <string>

class Item
{
private:
  int id;
  std::string name;
  std::string mesh;

public:
  Item() : id(-1) {}
  ~Item() {}

  void setId(int id)
  {
    this->id = id;
  }
  int getId() const
  {
    return id;
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
};

#endif // ITEM_H_

