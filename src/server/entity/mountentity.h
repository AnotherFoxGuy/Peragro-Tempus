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

#ifndef MOUNTENTITY_H
#define MOUNTENTITY_H

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
  ptMonitor<PcEntity>* passengers; // can't carry npcs
  size_t num_passengers;
  size_t max_passengers;

  ptMonitor<MountEntity> mount; // one mount can be on another.

  bool isWalking;

  PtVector3 final_dst;
  PtVector3 tmp_pos;
  size_t t_stop;
  size_t t_org;

  // TODO: Add stats
  float speed;

public:
  MountEntity()
  {
    entity = (new Entity(Entity::MountEntityType))->getRef();

    ptScopedMonitorable<Entity> e (entity.get());
    e->setMountEntity(this);

    speed = 7;
    max_passengers = 1;
    num_passengers = 0;

    passengers = new ptMonitor<PcEntity>[max_passengers];

    isWalking = false;
  }

  ~MountEntity()
  {
    for (size_t i = 0; i < max_passengers; i++)
    {
      delete passengers[i].get();
    }
    delete [] passengers;
  }

  const Entity* getEntity() const { return entity.get(); }

  void addPassenger(const PcEntity* e);
  void delPassenger(const PcEntity* e);
  size_t getPassengerCount() const { return num_passengers; }
  size_t getMaxPassengers() const { return max_passengers; }
  const PcEntity* getPassenger(size_t i) const;

  void walkTo(const PtVector3& dst_pos, float speed);
  PtVector3 getPos();

  float getSpeed() const { return speed; }
};

#endif // MOUNTENTITY_H
