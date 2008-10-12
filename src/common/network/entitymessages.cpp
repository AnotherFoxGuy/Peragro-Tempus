/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#include "common/network/entitymessages.h"
#include "deserialiser.h"
#include "serialiser.h"

bool AddNpcEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(entityname);
  serial.setString(meshname);
  serial.setInt16(meshid);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  serial.setInt32(entityid);
  serial.setInt8(equipmentcount);
  for ( size_t i = 0; i < equipmentcount ; i++ )
  {
    serial.setInt32(equipment[i].slotid);
    serial.setInt32(equipment[i].itemid);
    serial.setInt32(equipment[i].variation);
    serial.setString(equipment[i].filename);
    serial.setString(equipment[i].meshname);
  };

  serial.setInt8(poseid);
  return serial.isValid();
}

void AddNpcEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityname = serial.getString();
  meshname = serial.getString();
  meshid = (unsigned short) serial.getInt16();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
  entityid = (unsigned int) serial.getInt32();
  equipmentcount = (unsigned char) serial.getInt8();
  setEquipmentCount(equipmentcount);
  for ( size_t i = 0; i < equipmentcount ; i++ )
  {
    equipment[i].slotid = (unsigned int) serial.getInt32();
    equipment[i].itemid = (unsigned int) serial.getInt32();
    equipment[i].variation = (unsigned int) serial.getInt32();
    equipment[i].filename = serial.getString();
    equipment[i].meshname = serial.getString();
  };

  poseid = (unsigned char) serial.getInt8();
}

bool AddItemEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(itemid);
  serial.setInt32(variation);
  serial.setString(entityname);
  serial.setString(filename);
  serial.setString(meshname);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  serial.setInt32(entityid);
  return serial.isValid();
}

void AddItemEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  itemid = (unsigned int) serial.getInt32();
  variation = (unsigned int) serial.getInt32();
  entityname = serial.getString();
  filename = serial.getString();
  meshname = serial.getString();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
  entityid = (unsigned int) serial.getInt32();
}

bool AddDoorEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt16(doorid);
  serial.setInt8(isopen?1:0);
  serial.setInt8(islocked?1:0);
  serial.setInt32(entityid);
  serial.setString(entityname);
  serial.setString(meshname);
  serial.setInt16(sectorid);
  serial.setString(animationname);
  return serial.isValid();
}

void AddDoorEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  doorid = (unsigned short) serial.getInt16();
  isopen = serial.getInt8() != 0;
  islocked = serial.getInt8() != 0;
  entityid = (unsigned int) serial.getInt32();
  entityname = serial.getString();
  meshname = serial.getString();
  sectorid = (unsigned short) serial.getInt16();
  animationname = serial.getString();
}

bool AddPlayerEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(entityname);
  serial.setString(meshname);
  serial.setInt16(meshid);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[1]);
  serial.setInt8(haircolour[2]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[1]);
  serial.setInt8(skincolour[2]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[1]);
  serial.setInt8(decalcolour[2]);
  serial.setInt16(sectorid);
  serial.setInt32(entityid);
  serial.setInt8(equipmentcount);
  for ( size_t i = 0; i < equipmentcount ; i++ )
  {
    serial.setInt32(equipment[i].slotid);
    serial.setInt32(equipment[i].itemid);
    serial.setInt32(equipment[i].variation);
    serial.setString(equipment[i].filename);
    serial.setString(equipment[i].meshname);
  };

  serial.setInt8(poseid);
  return serial.isValid();
}

void AddPlayerEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityname = serial.getString();
  meshname = serial.getString();
  meshid = (unsigned short) serial.getInt16();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  haircolour[0] = (unsigned char) serial.getInt8();
  haircolour[1] = (unsigned char) serial.getInt8();
  haircolour[2] = (unsigned char) serial.getInt8();
  skincolour[0] = (unsigned char) serial.getInt8();
  skincolour[1] = (unsigned char) serial.getInt8();
  skincolour[2] = (unsigned char) serial.getInt8();
  decalcolour[0] = (unsigned char) serial.getInt8();
  decalcolour[1] = (unsigned char) serial.getInt8();
  decalcolour[2] = (unsigned char) serial.getInt8();
  sectorid = (unsigned short) serial.getInt16();
  entityid = (unsigned int) serial.getInt32();
  equipmentcount = (unsigned char) serial.getInt8();
  setEquipmentCount(equipmentcount);
  for ( size_t i = 0; i < equipmentcount ; i++ )
  {
    equipment[i].slotid = (unsigned int) serial.getInt32();
    equipment[i].itemid = (unsigned int) serial.getInt32();
    equipment[i].variation = (unsigned int) serial.getInt32();
    equipment[i].filename = serial.getString();
    equipment[i].meshname = serial.getString();
  };

  poseid = (unsigned char) serial.getInt8();
}

bool AddMountEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(entityname);
  serial.setString(meshname);
  serial.setInt16(meshid);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  serial.setInt32(entityid);
  serial.setInt32(inventoryid);
  return serial.isValid();
}

void AddMountEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityname = serial.getString();
  meshname = serial.getString();
  meshid = (unsigned short) serial.getInt16();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
  entityid = (unsigned int) serial.getInt32();
  inventoryid = (unsigned int) serial.getInt32();
}

bool RemoveEntityMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  return serial.isValid();
}

void RemoveEntityMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
}

bool MoveMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(turndirection);
  serial.setFloat(walkdirection);
  serial.setInt32(entityid);
  serial.setInt8(run?1:0);
  serial.setInt8(jump?1:0);
  return serial.isValid();
}

void MoveMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  turndirection = serial.getFloat();
  walkdirection = serial.getFloat();
  entityid = (unsigned int) serial.getInt32();
  run = serial.getInt8() != 0;
  jump = serial.getInt8() != 0;
}

bool MoveRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(turndirection);
  serial.setInt8(walkdirection);
  serial.setInt8(run?1:0);
  serial.setInt8(jump?1:0);
  return serial.isValid();
}

void MoveRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  turndirection = (unsigned char) serial.getInt8();
  walkdirection = (unsigned char) serial.getInt8();
  run = serial.getInt8() != 0;
  jump = serial.getInt8() != 0;
}

bool MoveToMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(from.x);
  serial.setFloat(from.y);
  serial.setFloat(from.z);
  serial.setFloat(to.x);
  serial.setFloat(to.y);
  serial.setFloat(to.z);
  serial.setFloat(speed);
  serial.setInt32(entityid);
  serial.setInt8(run?1:0);
  serial.setInt8(turn);
  serial.setInt8(jump?1:0);
  return serial.isValid();
}

void MoveToMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  from.x = serial.getFloat();
  from.y = serial.getFloat();
  from.z = serial.getFloat();
  to.x = serial.getFloat();
  to.y = serial.getFloat();
  to.z = serial.getFloat();
  speed = serial.getFloat();
  entityid = (unsigned int) serial.getInt32();
  run = serial.getInt8() != 0;
  turn = (unsigned char) serial.getInt8();
  jump = serial.getInt8() != 0;
}

bool MoveToRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(to.x);
  serial.setFloat(to.y);
  serial.setFloat(to.z);
  serial.setInt8(run?1:0);
  serial.setInt8(backwards?1:0);
  serial.setInt8(turn);
  serial.setInt8(jump?1:0);
  return serial.isValid();
}

void MoveToRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  to.x = serial.getFloat();
  to.y = serial.getFloat();
  to.z = serial.getFloat();
  run = serial.getInt8() != 0;
  backwards = serial.getInt8() != 0;
  turn = (unsigned char) serial.getInt8();
  jump = serial.getInt8() != 0;
}

bool PickRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(itementityid);
  serial.setInt8(slot);
  return serial.isValid();
}

void PickRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  itementityid = (unsigned int) serial.getInt32();
  slot = (unsigned char) serial.getInt8();
}

bool PickResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(itemid);
  serial.setInt32(variation);
  serial.setInt8(slotid);
  serial.setString(name);
  serial.setString(iconname);
  serial.setString(description);
  serial.setFloat(weight);
  serial.setString(equiptype);
  serial.setString(error);
  return serial.isValid();
}

void PickResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  itemid = (unsigned int) serial.getInt32();
  variation = (unsigned int) serial.getInt32();
  slotid = (unsigned char) serial.getInt8();
  name = serial.getString();
  iconname = serial.getString();
  description = serial.getString();
  weight = serial.getFloat();
  equiptype = serial.getString();
  error = serial.getString();
}

bool DropRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(slot);
  serial.setInt32(inventoryid);
  return serial.isValid();
}

void DropRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  slot = (unsigned char) serial.getInt8();
  inventoryid = (unsigned int) serial.getInt32();
}

bool DropResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(slotid);
  serial.setString(error);
  return serial.isValid();
}

void DropResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  slotid = (unsigned char) serial.getInt8();
  error = serial.getString();
}

bool EquipMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setInt32(itemid);
  serial.setInt32(variation);
  serial.setInt8(slotid);
  serial.setString(filename);
  serial.setString(meshname);
  return serial.isValid();
}

void EquipMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  itemid = (unsigned int) serial.getInt32();
  variation = (unsigned int) serial.getInt32();
  slotid = (unsigned char) serial.getInt8();
  filename = serial.getString();
  meshname = serial.getString();
}

bool RelocateMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  return serial.isValid();
}

void RelocateMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
}

bool TeleportRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  return serial.isValid();
}

void TeleportRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
}

bool TeleportResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  return serial.isValid();
}

void TeleportResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
}

bool DrUpdateRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  return serial.isValid();
}

void DrUpdateRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
}

bool DrUpdateMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setFloat(pos.x);
  serial.setFloat(pos.y);
  serial.setFloat(pos.z);
  serial.setFloat(rotation);
  serial.setInt16(sectorid);
  serial.setInt32(entityid);
  return serial.isValid();
}

void DrUpdateMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  pos.x = serial.getFloat();
  pos.y = serial.getFloat();
  pos.z = serial.getFloat();
  rotation = serial.getFloat();
  sectorid = (unsigned short) serial.getInt16();
  entityid = (unsigned int) serial.getInt32();
}

bool MountRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(mountentityid);
  return serial.isValid();
}

void MountRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  mountentityid = (unsigned int) serial.getInt32();
}

bool MountMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(playerentityid);
  serial.setInt32(mountentityid);
  serial.setInt8(cancontrol?1:0);
  return serial.isValid();
}

void MountMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  playerentityid = (unsigned int) serial.getInt32();
  mountentityid = (unsigned int) serial.getInt32();
  cancontrol = serial.getInt8() != 0;
}

bool UnmountRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(mountentityid);
  return serial.isValid();
}

void UnmountRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  mountentityid = (unsigned int) serial.getInt32();
}

bool UnmountMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(playerentityid);
  serial.setInt32(mountentityid);
  return serial.isValid();
}

void UnmountMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  playerentityid = (unsigned int) serial.getInt32();
  mountentityid = (unsigned int) serial.getInt32();
}

bool PoseRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(poseid);
  return serial.isValid();
}

void PoseRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  poseid = (unsigned char) serial.getInt8();
}

bool PoseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setInt8(poseid);
  return serial.isValid();
}

void PoseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  poseid = (unsigned char) serial.getInt8();
}

