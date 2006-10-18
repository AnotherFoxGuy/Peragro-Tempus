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

#include "common/network/entitymessages.h"
#include "deserialiser.h"
#include "serialiser.h"

void AddNpcEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(name);
  serial.setString(mesh);
  serial.setFloat(pos[0]);
  serial.setFloat(pos[1]);
  serial.setFloat(pos[2]);
  serial.setString(sector);
  serial.setInt32(entityid);
}

void AddNpcEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  name = serial.getString();
  mesh = serial.getString();
  pos[0] = serial.getFloat();
  pos[1] = serial.getFloat();
  pos[2] = serial.getFloat();
  sector = serial.getString();
  entityid = (unsigned int) serial.getInt32();
}

void AddItemEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(itemid);
  serial.setFloat(pos[0]);
  serial.setFloat(pos[1]);
  serial.setFloat(pos[2]);
  serial.setString(sector);
  serial.setInt32(entityid);
}

void AddItemEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  itemid = (unsigned int) serial.getInt32();
  pos[0] = serial.getFloat();
  pos[1] = serial.getFloat();
  pos[2] = serial.getFloat();
  sector = serial.getString();
  entityid = (unsigned int) serial.getInt32();
}

void AddDoorEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(name);
  serial.setString(mesh);
  serial.setInt8(isopen);
  serial.setInt8(islocked);
  serial.setInt32(entityid);
}

void AddDoorEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  name = serial.getString();
  mesh = serial.getString();
  isopen = (unsigned char) serial.getInt8();
  islocked = (unsigned char) serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
}

void AddPlayerEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(name);
  serial.setString(mesh);
  serial.setFloat(pos[0]);
  serial.setFloat(pos[1]);
  serial.setFloat(pos[2]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setString(sector);
  serial.setInt32(entityid);
  serial.setInt8(equipmentcount);
  for ( size_t i = 0; i < equipmentcount ; i++ )
  {
    serial.setInt32(equipment[i].itemid);
  };

}

void AddPlayerEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  name = serial.getString();
  mesh = serial.getString();
  pos[0] = serial.getFloat();
  pos[1] = serial.getFloat();
  pos[2] = serial.getFloat();
  haircolour[0] = (unsigned char) serial.getInt8();
  haircolour[1] = (unsigned char) serial.getInt8();
  haircolour[2] = (unsigned char) serial.getInt8();
  skincolour[0] = (unsigned char) serial.getInt8();
  skincolour[1] = (unsigned char) serial.getInt8();
  skincolour[2] = (unsigned char) serial.getInt8();
  decalcolour[0] = (unsigned char) serial.getInt8();
  decalcolour[1] = (unsigned char) serial.getInt8();
  decalcolour[2] = (unsigned char) serial.getInt8();
  sector = serial.getString();
  entityid = (unsigned int) serial.getInt32();
  equipmentcount = (unsigned char) serial.getInt8();
  setEquipmentCount(equipmentcount);
  for ( size_t i = 0; i < equipmentcount ; i++ )
  {
    equipment[i].itemid = (unsigned int) serial.getInt32();
  };

}

void RemoveMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
}

void RemoveMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
}

void MoveMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(turn);
  serial.setFloat(walk);
  serial.setInt32(entityid);
}

void MoveMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  turn = serial.getFloat();
  walk = serial.getFloat();
  entityid = (unsigned int) serial.getInt32();
}

void MoveRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(turn);
  serial.setInt8(walk);
}

void MoveRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  turn = (unsigned char) serial.getInt8();
  walk = (unsigned char) serial.getInt8();
}

void PickRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(target);
}

void PickRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  target = (unsigned int) serial.getInt32();
}

void PickResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(name);
  serial.setInt32(itemid);
  serial.setInt32(slot);
  serial.setString(target);
  serial.setString(error);
}

void PickResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  name = serial.getString();
  itemid = (unsigned int) serial.getInt32();
  slot = (unsigned int) serial.getInt32();
  target = serial.getString();
  error = serial.getString();
}

void DropRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(target);
  serial.setInt8(slotid);
}

void DropRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  target = (unsigned int) serial.getInt32();
  slotid = (unsigned char) serial.getInt8();
}

void DropResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(name);
  serial.setString(target);
  serial.setString(error);
}

void DropResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  name = serial.getString();
  target = serial.getString();
  error = serial.getString();
}

void InventoryListMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(inventorycount);
  for ( size_t i = 0; i < inventorycount ; i++ )
  {
    serial.setInt16(inventory[i].itemid);
    serial.setInt8(inventory[i].slotid);
    serial.setString(inventory[i].name);
    serial.setInt16(inventory[i].amount);
  };

}

void InventoryListMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  inventorycount = (unsigned char) serial.getInt8();
  setInventoryCount(inventorycount);
  for ( size_t i = 0; i < inventorycount ; i++ )
  {
    inventory[i].itemid = (unsigned short) serial.getInt16();
    inventory[i].slotid = (unsigned char) serial.getInt8();
    inventory[i].name = serial.getString();
    inventory[i].amount = (unsigned short) serial.getInt16();
  };

}

void StatsListMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(statscount);
  for ( size_t i = 0; i < statscount ; i++ )
  {
    serial.setInt16(stats[i].statid);
    serial.setString(stats[i].name);
    serial.setInt16(stats[i].level);
  };

}

void StatsListMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  statscount = (unsigned char) serial.getInt8();
  setStatsCount(statscount);
  for ( size_t i = 0; i < statscount ; i++ )
  {
    stats[i].statid = (unsigned short) serial.getInt16();
    stats[i].name = serial.getString();
    stats[i].level = (unsigned short) serial.getInt16();
  };

}

void StatsChangeMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt16(statid);
  serial.setInt16(level);
}

void StatsChangeMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  statid = (unsigned short) serial.getInt16();
  level = (unsigned short) serial.getInt16();
}

void SkillsListMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(skillscount);
  for ( size_t i = 0; i < skillscount ; i++ )
  {
    serial.setInt16(skills[i].skillid);
    serial.setString(skills[i].name);
  };

}

void SkillsListMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  skillscount = (unsigned char) serial.getInt8();
  setSkillsCount(skillscount);
  for ( size_t i = 0; i < skillscount ; i++ )
  {
    skills[i].skillid = (unsigned short) serial.getInt16();
    skills[i].name = serial.getString();
  };

}

void EquipRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(oldslotid);
  serial.setInt8(newslotid);
}

void EquipRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  oldslotid = (unsigned char) serial.getInt8();
  newslotid = (unsigned char) serial.getInt8();
}

void EquipMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setInt32(itemid);
  serial.setInt8(oldslotid);
  serial.setInt8(newslotid);
  serial.setString(error);
}

void EquipMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  itemid = (unsigned int) serial.getInt32();
  oldslotid = (unsigned char) serial.getInt8();
  newslotid = (unsigned char) serial.getInt8();
  error = serial.getString();
}

void MoveToMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(from[0]);
  serial.setFloat(from[1]);
  serial.setFloat(from[2]);
  serial.setFloat(to[0]);
  serial.setFloat(to[1]);
  serial.setFloat(to[2]);
  serial.setFloat(speed);
  serial.setInt32(entityid);
}

void MoveToMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  from[0] = serial.getFloat();
  from[1] = serial.getFloat();
  from[2] = serial.getFloat();
  to[0] = serial.getFloat();
  to[1] = serial.getFloat();
  to[2] = serial.getFloat();
  speed = serial.getFloat();
  entityid = (unsigned int) serial.getInt32();
}

void MoveToRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(to[0]);
  serial.setFloat(to[1]);
  serial.setFloat(to[2]);
}

void MoveToRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  to[0] = serial.getFloat();
  to[1] = serial.getFloat();
  to[2] = serial.getFloat();
}

void RelocateMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
}

void RelocateMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
}

void TeleportMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setFloat(pos[0]);
  serial.setFloat(pos[1]);
  serial.setFloat(pos[2]);
  serial.setString(sector);
}

void TeleportMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  pos[0] = serial.getFloat();
  pos[1] = serial.getFloat();
  pos[2] = serial.getFloat();
  sector = serial.getString();
}

void DrUpdateRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(pos[0]);
  serial.setFloat(pos[1]);
  serial.setFloat(pos[2]);
  serial.setFloat(rotation);
  serial.setString(sector);
}

void DrUpdateRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  pos[0] = serial.getFloat();
  pos[1] = serial.getFloat();
  pos[2] = serial.getFloat();
  rotation = serial.getFloat();
  sector = serial.getString();
}

void DrUpdateMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(pos[0]);
  serial.setFloat(pos[1]);
  serial.setFloat(pos[2]);
  serial.setFloat(rotation);
  serial.setString(sector);
  serial.setInt32(entityid);
}

void DrUpdateMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  pos[0] = serial.getFloat();
  pos[1] = serial.getFloat();
  pos[2] = serial.getFloat();
  rotation = serial.getFloat();
  sector = serial.getString();
  entityid = (unsigned int) serial.getInt32();
}

