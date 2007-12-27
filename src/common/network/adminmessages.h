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

#ifndef _ADMINMESSAGES_H_
#define _ADMINMESSAGES_H_

#include "netmessage.h"

namespace ADMIN
{
  enum MESSAGES
  {
    CREATESECTOR=0,
    CREATEITEM=1,
    REMOVEALL=2
  };
}

class CreateSectorMessage : public NetMessage
{
  unsigned short sectorid;
  ptString name;
  ptString region;

public:
  CreateSectorMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATESECTOR)
  {
  }

  ~CreateSectorMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned short getSectorId() { return sectorid; }
  void setSectorId(unsigned short x) { sectorid = x; }

  ptString getName() { return name; }
  void setName(ptString x) { name = x; }

  ptString getRegion() { return region; }
  void setRegion(ptString x) { region = x; }

};

class CreateItemMessage : public NetMessage
{
  unsigned int itemid;
  ptString name;
  ptString icon;
  ptString description;
  ptString file;
  ptString mesh;
  unsigned int weight;
  ptString equiptype;

public:
  CreateItemMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATEITEM)
  {
  }

  ~CreateItemMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getItemId() { return itemid; }
  void setItemId(unsigned int x) { itemid = x; }

  ptString getName() { return name; }
  void setName(ptString x) { name = x; }

  ptString getIcon() { return icon; }
  void setIcon(ptString x) { icon = x; }

  ptString getDescription() { return description; }
  void setDescription(ptString x) { description = x; }

  ptString getFile() { return file; }
  void setFile(ptString x) { file = x; }

  ptString getMesh() { return mesh; }
  void setMesh(ptString x) { mesh = x; }

  unsigned int getWeight() { return weight; }
  void setWeight(unsigned int x) { weight = x; }

  ptString getEquipType() { return equiptype; }
  void setEquipType(ptString x) { equiptype = x; }

};

class RemoveAllMessage : public NetMessage
{
  ptString datatype;

public:
  RemoveAllMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::REMOVEALL)
  {
  }

  ~RemoveAllMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getDataType() { return datatype; }
  void setDataType(ptString x) { datatype = x; }

};

#endif // _ADMINMESSAGES_H_
