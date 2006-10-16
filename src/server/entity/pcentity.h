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

#ifndef _PCENTITY_H_
#define _PCENTITY_H_

#include "common/util/monitorable.h"

#include "entity.h"
#include "tradepeer.h"

class Character;
class Entity;
class User;

class PcEntity : public ptMonitorable<PcEntity>
{
private:
  ptMonitor<User> user;
  ptMonitor<Entity> entity;
  ptMonitor<Character> character;

  TradePeer tradepeer;

  bool isWalking;

  float final_dst[3];
  size_t t_stop;

  float tmp_pos[3]; //used only for temporary calculations!

public:
  PcEntity()
  {
    entity = (new Entity(Entity::PlayerEntityType))->getRef();

    Entity* e = entity.get()->getLock();
    e->setPlayerEntity(this);
    e->freeLock();

    isWalking = false;
  }

  void setUser(User* user);
  const User* getUser() const { return this->user.get(); }

  void setEntity(Entity* entity);
  const Entity* getEntity() const { return this->entity.get(); }

  void setCharacter(Character* character);
  const Character* getCharacter() const { return this->character.get(); }

  TradePeer* getTradePeer() { return &tradepeer; }

  void walkTo(float* dst_pos, float speed);
  const float* getPos();
};

#endif // _PCENTITY_H_
