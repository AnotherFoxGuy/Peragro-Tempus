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

#ifndef PCENTITY_H
#define PCENTITY_H

#include "common/util/monitorable.h"

#include "entity.h"
#include "character.h"
#include "tradepeer.h"

class User;
class Entity;
class Character;
class MountEntity;

class PcEntity : public ptMonitorable<PcEntity>
{
private:
  ptMonitor<User> user;
  ptMonitor<Entity> entity;
  ptMonitor<Character> character;
  ptMonitor<MountEntity> mount;

  TradePeer tradepeer;

  bool isWalking;

  float final_dst[3];
  size_t t_stop;

  float tmp_pos[3]; //used only for temporary calculations!

  unsigned char pose_id;

  // Moving to the destination instantly!
  bool flashStep;

public:
  PcEntity()
  {
    entity = (new Entity(Entity::PlayerEntityType))->getRef();

    Entity* e = entity.get()->getLock();
    e->setPlayerEntity(this);
    e->freeLock();

    tradepeer.setEntity(this);

    isWalking = false;
    pose_id = 0;

    flashStep = false;
  }

  ~PcEntity()
  {
    delete character.get();
  }

  void setUser(const User* user);
  const User* getUser() const { return user.get(); }

  void setEntity(const Entity* entity);
  const Entity* getEntity() const { return entity.get(); }

  void setCharacter(const Character* character);
  const Character* getCharacter() const { return character.get(); }

  void setMount(const MountEntity* mount);
  const MountEntity* getMount() const { return mount.get(); }

  TradePeer* getTradePeer() { return &tradepeer; }

  void setPose(unsigned char pose_id) { this->pose_id = pose_id; }
  const unsigned char getPose() const { return pose_id; }

  void walkTo(float* dst_pos, float speed);
  const float* getPos();

  void toggleFlashStep() { flashStep = !flashStep; }
  bool usesFlashStep() const { return flashStep; }
};

#endif // PCENTITY_H
