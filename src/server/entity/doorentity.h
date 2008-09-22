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

#ifndef DOORENTITY_H
#define DOORENTITY_H

#include <string.h>
#include <time.h>
#include <math.h>

#include "common/util/ptstring.h"
#include "common/util/monitorable.h"

#include "entity.h"
#include "inventory.h"
#include "characterstats.h"

class DoorEntity : public ptMonitorable<DoorEntity>
{
private:
  unsigned short doorid;
  ptMonitor<Entity> entity;

  bool open;
  bool locked;

  ptString animation;

public:
  DoorEntity() : open(false), locked(false)
  {
    entity = (new Entity(Entity::DoorEntityType))->getRef();

    ptScopedMonitorable<Entity> e (entity.get());
    e->setDoorEntity(this);
  }

  ~DoorEntity()
  {
  }

  //void setEntity(Entity* entity);
  const Entity* getEntity() { return this->entity.get(); }

  void setDoorId(unsigned short doorid) { this->doorid = doorid; }
  unsigned short getDoorId() const { return doorid; }

  void setOpen(bool open) { this->open = open; }
  bool getOpen() const { return open; }

  void setLocked(bool locked) { this->locked = locked; }
  bool getLocked() const { return locked; }

  const ptString& getAnimation() const { return animation; }
  void setAnimation(ptString id) { animation = id; }

};

#endif // DOORENTITY_H
