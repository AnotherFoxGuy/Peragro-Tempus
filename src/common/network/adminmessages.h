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

#ifndef ADMINMESSAGES_H
#define ADMINMESSAGES_H

#include "netmessage.h"

namespace ADMIN
{
  enum MESSAGES
  {
    SETCONFIG = 0,
    CREATEMESH = 1,
    CREATESPAWNPOINT = 2,
    SPAWNITEM = 3,
    SPAWNMOUNT = 4,
    SPAWNDOOR = 5,
    REMOVESPAWNEDENTITY = 6,
    TOGGLEFLASHSTEP = 7,
    CREATEZONE = 8,
    SETDATE = 9,
    CREATECHANDEFAULT = 10,
    CREATECHANSPACE = 11,
    CREATELOCATION = 12,
    TELEPORTLOCATION = 13
  };
}

class SetConfigMessage : public NetMessage
{
  ptString option;
  ptString value;

public:
  SetConfigMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::SETCONFIG)
  {
  }

  ~SetConfigMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getOption() const { return option; }
  void setOption(ptString x) { option = x; }

  ptString getValue() const { return value; }
  void setValue(ptString x) { value = x; }

};

class CreateMeshMessage : public NetMessage
{
  unsigned int revision;
  ptString file;
  ptString factory;

public:
  CreateMeshMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATEMESH)
  {
  }

  ~CreateMeshMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getRevision() const { return revision; }
  void setRevision(unsigned int x) { revision = x; }

  ptString getFile() const { return file; }
  void setFile(ptString x) { file = x; }

  ptString getFactory() const { return factory; }
  void setFactory(ptString x) { factory = x; }

};

class CreateSpawnPointMessage : public NetMessage
{
  unsigned int itemtemplateid;
  WFMath::Point<3> position;
  unsigned int interval;

public:
  CreateSpawnPointMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATESPAWNPOINT)
  {
  }

  ~CreateSpawnPointMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getItemTemplateId() const { return itemtemplateid; }
  void setItemTemplateId(unsigned int x) { itemtemplateid = x; }

  WFMath::Point<3> getPosition() { return position; }
  void setPosition(const WFMath::Point<3>& x)
  {
    position = x;
  }
  void setPosition(float x, float y, float z)
  {
    position[0] = x;
    position[1] = y;
    position[2] = z;
  }

  unsigned int getInterval() const { return interval; }
  void setInterval(unsigned int x) { interval = x; }

};

class SpawnItemMessage : public NetMessage
{
  unsigned int itemtemplateid;
  WFMath::Point<3> position;

public:
  SpawnItemMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::SPAWNITEM)
  {
  }

  ~SpawnItemMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getItemTemplateId() const { return itemtemplateid; }
  void setItemTemplateId(unsigned int x) { itemtemplateid = x; }

  WFMath::Point<3> getPosition() { return position; }
  void setPosition(const WFMath::Point<3>& x)
  {
    position = x;
  }
  void setPosition(float x, float y, float z)
  {
    position[0] = x;
    position[1] = y;
    position[2] = z;
  }

};

class SpawnMountMessage : public NetMessage
{
  ptString name;
  ptString species;
  WFMath::Point<3> position;
  float rotation;

public:
  SpawnMountMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::SPAWNMOUNT)
  {
  }

  ~SpawnMountMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  ptString getSpecies() const { return species; }
  void setSpecies(ptString x) { species = x; }

  WFMath::Point<3> getPosition() { return position; }
  void setPosition(const WFMath::Point<3>& x)
  {
    position = x;
  }
  void setPosition(float x, float y, float z)
  {
    position[0] = x;
    position[1] = y;
    position[2] = z;
  }

  float getRotation() const { return rotation; }
  void setRotation(float x) { rotation = x; }

};

class SpawnDoorMessage : public NetMessage
{
  unsigned int entityid;
  ptString name;
  ptString mesh;
  WFMath::Point<3> position;
  bool isopen;
  bool islocked;
  ptString animation;

public:
  SpawnDoorMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::SPAWNDOOR)
  {
  }

  ~SpawnDoorMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getEntityId() const { return entityid; }
  void setEntityId(unsigned int x) { entityid = x; }

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  ptString getMesh() const { return mesh; }
  void setMesh(ptString x) { mesh = x; }

  WFMath::Point<3> getPosition() { return position; }
  void setPosition(const WFMath::Point<3>& x)
  {
    position = x;
  }
  void setPosition(float x, float y, float z)
  {
    position[0] = x;
    position[1] = y;
    position[2] = z;
  }

  bool getIsOpen() const { return isopen; }
  void setIsOpen(bool x) { isopen = x; }

  bool getIsLocked() const { return islocked; }
  void setIsLocked(bool x) { islocked = x; }

  ptString getAnimation() const { return animation; }
  void setAnimation(ptString x) { animation = x; }

};

class RemoveSpawnedEntityMessage : public NetMessage
{
  unsigned int entityid;

public:
  RemoveSpawnedEntityMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::REMOVESPAWNEDENTITY)
  {
  }

  ~RemoveSpawnedEntityMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getEntityId() const { return entityid; }
  void setEntityId(unsigned int x) { entityid = x; }

};

class ToggleFlashStepMessage : public NetMessage
{

public:
  ToggleFlashStepMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::TOGGLEFLASHSTEP)
  {
  }

  ~ToggleFlashStepMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class CreateZoneMessage : public NetMessage
{
  unsigned short zoneid;
  ptString zonetype;
  class ListNodes
  {
  public:
    float x;
    float z;
  };

  unsigned int nodescount;
  ListNodes* nodes;


public:
  CreateZoneMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATEZONE)
  {
    nodes = 0;
  }

  ~CreateZoneMessage()
  {
    delete [] nodes;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned short getZoneId() const { return zoneid; }
  void setZoneId(unsigned short x) { zoneid = x; }

  ptString getZoneType() const { return zonetype; }
  void setZoneType(ptString x) { zonetype = x; }

  unsigned char getNodesCount() const { return nodescount; }
  void setNodesCount(unsigned char ic)
  {
    nodescount = ic;
    delete [] nodes;
    nodes = new ListNodes[ic];
  }

  // --- begin ListNodes Getter and Setter ---

  float getX(size_t i) { return nodes[i].x; }
  void setX(size_t i, float x) { nodes[i].x = x; }

  float getZ(size_t i) { return nodes[i].z; }
  void setZ(size_t i, float x) { nodes[i].z = x; }

  // --- end ListNodes Getter and Setter ---

};

class SetDateMessage : public NetMessage
{
  unsigned int seconds;

public:
  SetDateMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::SETDATE)
  {
  }

  ~SetDateMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getSeconds() const { return seconds; }
  void setSeconds(unsigned int x) { seconds = x; }

};

class CreateChanDefaultMessage : public NetMessage
{
  bool isinitchan;
  ptString group;
  bool permanent;

public:
  CreateChanDefaultMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATECHANDEFAULT)
  {
  }

  ~CreateChanDefaultMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  bool getIsInitChan() const { return isinitchan; }
  void setIsInitChan(bool x) { isinitchan = x; }

  ptString getGroup() const { return group; }
  void setGroup(ptString x) { group = x; }

  bool getPermanent() const { return permanent; }
  void setPermanent(bool x) { permanent = x; }

};

class CreateChanSpaceMessage : public NetMessage
{
  ptString name;
  ptString form;
  ptString createtype;
  unsigned short createval;
  bool createperm;
  ptString jointype;
  unsigned short joinval;
  bool joinperm;
  ptString invitetype;
  unsigned short inviteval;
  bool vischannel;
  bool vismembers;

public:
  CreateChanSpaceMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATECHANSPACE)
  {
  }

  ~CreateChanSpaceMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  ptString getForm() const { return form; }
  void setForm(ptString x) { form = x; }

  ptString getCreateType() const { return createtype; }
  void setCreateType(ptString x) { createtype = x; }

  unsigned short getCreateVal() const { return createval; }
  void setCreateVal(unsigned short x) { createval = x; }

  bool getCreatePerm() const { return createperm; }
  void setCreatePerm(bool x) { createperm = x; }

  ptString getJoinType() const { return jointype; }
  void setJoinType(ptString x) { jointype = x; }

  unsigned short getJoinVal() const { return joinval; }
  void setJoinVal(unsigned short x) { joinval = x; }

  bool getJoinPerm() const { return joinperm; }
  void setJoinPerm(bool x) { joinperm = x; }

  ptString getInviteType() const { return invitetype; }
  void setInviteType(ptString x) { invitetype = x; }

  unsigned short getInviteVal() const { return inviteval; }
  void setInviteVal(unsigned short x) { inviteval = x; }

  bool getVisChannel() const { return vischannel; }
  void setVisChannel(bool x) { vischannel = x; }

  bool getVisMembers() const { return vismembers; }
  void setVisMembers(bool x) { vismembers = x; }

};

class CreateLocationMessage : public NetMessage
{
  ptString name;

public:
  CreateLocationMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::CREATELOCATION)
  {
  }

  ~CreateLocationMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

};

class TeleportLocationMessage : public NetMessage
{
  ptString name;

public:
  TeleportLocationMessage() : NetMessage(MESSAGES::ADMIN,ADMIN::TELEPORTLOCATION)
  {
  }

  ~TeleportLocationMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

};

#endif // ADMINMESSAGES_H
