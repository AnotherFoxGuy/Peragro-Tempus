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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#ifndef COMBATMESSAGES_H
#define COMBATMESSAGES_H

#include "netmessage.h"

namespace COMBAT
{
  enum MESSAGES
  {
    ATTACKREQUEST=1,
    ATTACKNOTIFICATION=2,
    UPDATEARMOR=3,
    UPDATEWEAPON=4,
    ATTACKRESULT=5
  };
}

class AttackRequestMessage : public NetMessage
{
  unsigned int targetid;
  unsigned int attacktype;

public:
  AttackRequestMessage() : NetMessage(MESSAGES::COMBAT,COMBAT::ATTACKREQUEST)
  {
  }

  ~AttackRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getTargetID() const { return targetid; }
  void setTargetID(unsigned int x) { targetid = x; }

  unsigned int getAttackType() const { return attacktype; }
  void setAttackType(unsigned int x) { attacktype = x; }

};

class AttackNotificationMessage : public NetMessage
{
  unsigned int playerid;
  unsigned int health;

public:
  AttackNotificationMessage() : NetMessage(MESSAGES::COMBAT,COMBAT::ATTACKNOTIFICATION)
  {
  }

  ~AttackNotificationMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getPlayerID() const { return playerid; }
  void setPlayerID(unsigned int x) { playerid = x; }

  unsigned int getHealth() const { return health; }
  void setHealth(unsigned int x) { health = x; }

};

class UpdateArmorMessage : public NetMessage
{

public:
  UpdateArmorMessage() : NetMessage(MESSAGES::COMBAT,COMBAT::UPDATEARMOR)
  {
  }

  ~UpdateArmorMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class UpdateWeaponMessage : public NetMessage
{

public:
  UpdateWeaponMessage() : NetMessage(MESSAGES::COMBAT,COMBAT::UPDATEWEAPON)
  {
  }

  ~UpdateWeaponMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class AttackResultMessage : public NetMessage
{
  unsigned int playerid;
  unsigned int health;

public:
  AttackResultMessage() : NetMessage(MESSAGES::COMBAT,COMBAT::ATTACKRESULT)
  {
  }

  ~AttackResultMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getPlayerID() const { return playerid; }
  void setPlayerID(unsigned int x) { playerid = x; }

  unsigned int getHealth() const { return health; }
  void setHealth(unsigned int x) { health = x; }

};

#endif // COMBATMESSAGES_H
