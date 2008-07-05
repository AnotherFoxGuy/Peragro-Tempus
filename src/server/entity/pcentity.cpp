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

#include "server/entity/user.h"
#include "server/entity/pcentity.h"
#include "server/entity/mountentity.h"
#include "server/group/charchats.h"

#include <time.h>

void PcEntity::setEntity(const Entity* entity)
{
  if (!entity) this->entity.clear();
  this->entity = entity->getRef();
}

void PcEntity::setUser(const User* user)
{
  this->user = user->getRef();
}

void PcEntity::setCharacter(const Character* character)
{
  this->character = character->getRef();
  Entity* e = entity.get()->getLock();
  Character* c = character->getLock();
  c->setEntity(e);
  c->freeLock();
  e->freeLock();
}

void PcEntity::walkTo(float* dst_pos, float speed)
{
  if (mount.get()) return;

  final_dst[0] = dst_pos[0];
  final_dst[1] = dst_pos[1];
  final_dst[2] = dst_pos[2];

  const float* pos = entity.get()->getPos();

  float dist_x = fabsf(final_dst[0] - pos[0]);
  float dist_y = fabsf(final_dst[1] - pos[1]);
  float dist_z = fabsf(final_dst[2] - pos[2]);
  float dist = sqrtf(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z);

  //v = s / t => t = s / v
  t_stop = (size_t) (dist / speed + time(0));

  isWalking = true;
}

const float* PcEntity::getPos()
{
  if (mount.get()) 
  {
    MountEntity* e = mount.get()->getLock();
    const float* p = e->getPos();
    e->freeLock();
    return p;
  }

  if (!isWalking)
  {
    return entity.get()->getPos();
  }
  else
  {
    if ((size_t)time(0) >= t_stop)
    {
      Entity* ent = entity.get()->getLock();
      ent->setPos(final_dst);
      ent->freeLock();

      isWalking = false;
      return final_dst;
    }
    else
    {
      const float* pos = entity.get()->getPos();
      //Not sure that's correct...
      size_t delta = t_stop - (size_t) time(0);
      tmp_pos[0] = (final_dst[0] - pos[0]) * delta;
      tmp_pos[1] = (final_dst[1] - pos[1]) * delta;
      tmp_pos[2] = (final_dst[2] - pos[2]) * delta;
      return tmp_pos;
    }
  }
}

void PcEntity::setMount(const MountEntity* mount)
{
  if (!mount) 
    this->mount.clear();
  else
    this->mount = mount->getRef();
}

void PcEntity::setCharChats(const CharChats* charchats)
{
  if (!charchats)
    this->charchats.clear();
  else
    this->charchats = charchats->getRef();
}
