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

#include "common/util/monitorable.h"
#include "common/util/ptstring.h"

class Item : public ptMonitorable<Item>
{
public:
  static const unsigned char NoItem = 0;

private:
  unsigned int id;

  ptString name_id;
  ptString mesh_id;

  //unsigned int stackmax;

  unsigned int weight;

public:
  Item() : id(-1), weight(0) {}
  ~Item() {}

  void setId(unsigned int id) { this->id = id; }
  unsigned int getId() const { return id; }

  ptString& getName() { return name_id; }
  void setName(ptString id) { name_id = id; }

  ptString& getMesh() { return mesh_id; }
  void setMesh(ptString id) { mesh_id = id; }

  //void setStackMax(unsigned int stackmax) { this->stackmax = stackmax; }
  //unsigned int getStackMax() const { return stackmax; }

  void setWeight(unsigned int weight) { this->weight = weight; }
  unsigned int getWeight() const { return weight; }

  bool isEquipable() { return false; }
};

#endif // ITEM_H_
