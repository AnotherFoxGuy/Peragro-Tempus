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

#ifndef _MOUNTENTITY_H_
#define _MOUNTENTITY_H_

#include "common/util/ptstring.h"
#include "common/util/monitorable.h"

#include "entity.h"

#include "server/ai/ai.h"

class Character;
class PcEntity;

class MountEntity : public ptMonitorable<MountEntity>
{
private:
  ptMonitor<Entity> entity; // own entity
  Array<ptMonitor<PcEntity> > passengers; // can't carry npcs
  size_t max_passengers;

  ptMonitor<MountEntity> mount; // one mount can be on another.

  bool isWalking;

  float final_dst[3];
  size_t t_stop;

  // TODO: Add stats
  float speed;

public:
  MountEntity()
  {
    entity = (new Entity(Entity::MountEntityType))->getRef();

    Entity* e = entity.get()->getLock();
    e->setMountEntity(this);
    e->freeLock();

    speed = 10;
    max_passengers = 1;
  }

  ~MountEntity() { }

  const Entity* getEntity() const { return entity.get(); }

  void addPassenger(const PcEntity* e);
  size_t getPassengerCount() const { return passengers.getCount(); }
  size_t getMaxPassengers() const { return max_passengers; }
  const PcEntity* getPassenger(size_t i) const;

  void walkTo(float* dst_pos, float speed);
  const float* getPos();

  float getSpeed() const { return speed; }
};

#endif // _MOUNTENTITY_H_
