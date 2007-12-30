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
  ptString icon;
  ptString description;
  ptString file;
  ptString mesh;

  //unsigned int stackmax;

  unsigned int weight;

  ptString equiptype;

public:
  enum ItemType
  {
    DefaultType=0,
    WeaponType=1,
    KeyType=2,
    BookType=3
  };

private:
  ItemType type;

public:
  Item() : id(-1), weight(0) {}
  ~Item() {}

  void setId(unsigned int id) { this->id = id; }
  unsigned int getId() const { return id; }

  ptString getName() const { return name_id; }
  void setName(ptString id) { name_id = id; }

  ptString getIcon() const { return icon; }
  void setIcon(ptString x) { icon = x; }

  ptString getDescription() const { return description; }
  void setDescription(ptString x) { description = x; }

  ptString getFile() const { return file; }
  void setFile(ptString x) { file = x; }

  ptString getMesh() const { return mesh; }
  void setMesh(ptString id) { mesh = id; }

  //void setStackMax(unsigned int stackmax) { this->stackmax = stackmax; }
  //unsigned int getStackMax() const { return stackmax; }

  void setWeight(unsigned int weight) { this->weight = weight; }
  unsigned int getWeight() const { return weight; }

  ptString getEquiptype() const { return equiptype; }
  void setEquiptype(ptString x) { equiptype = x; }

  bool isEquipable() const { return false; }
};

#endif // ITEM_H_
