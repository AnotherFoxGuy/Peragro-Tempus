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
#include "server/entity/character.h"
#include "server/entity/entity.h"
#include "server/entity/pcentity.h"
#include "server/entity/mountentity.h"

#include <time.h>

void MountEntity::addPassenger(const PcEntity* e)
{
  passengers[num_passengers] = e->getRef(); 
  num_passengers++;
}

void MountEntity::delPassenger(const PcEntity* e)
{
  for (size_t i = 0; i < num_passengers; i++)
  {
    if (passengers[i].get() == e) 
    {
      passengers[i].clear();
      num_passengers--;
    }
  }
}

const PcEntity* MountEntity::getPassenger(size_t i) const
{ 
  if (i > num_passengers) return 0;
  return passengers[i].get(); 
}

void MountEntity::walkTo(float* dst_pos, float speed)
{
  // If we are already walking, lets store how
  // far we have come...
  if (isWalking) {
    const float *pos = getPos();
    Entity* ent = entity.get()->getLock();
    ent->setPos(pos);
    ent->freeLock();
    isWalking = false;
  }

  final_dst[0] = dst_pos[0];
  final_dst[1] = dst_pos[1];
  final_dst[2] = dst_pos[2];

  const float* pos = entity.get()->getPos();

  float dist_x = fabsf(final_dst[0] - pos[0]);
  float dist_y = fabsf(final_dst[1] - pos[1]);
  float dist_z = fabsf(final_dst[2] - pos[2]);
  float dist = sqrtf(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z);
  
  t_org = (size_t) time(0);
  //v = s / t => t = s / v
  t_stop = (size_t) (dist / speed + time(0));

  isWalking = true;
}

const float* MountEntity::getPos()
{
  if (!isWalking)
  {
    return entity.get()->getPos();
  }

  if ((size_t)time(0) >= t_stop)
  {
    Entity* ent = entity.get()->getLock();
    ent->setPos(final_dst);
    ent->freeLock();

    isWalking = false;
    return final_dst;
  }

  // pos will be org_pos until target is reached.
  const float* pos = entity.get()->getPos();

  // TODO: Probably fixed now, need to verify though... 
  size_t delta = (time(0) - t_org) / (t_stop - t_org);
  tmp_pos[0] = (final_dst[0] - pos[0]) * delta + pos[0];
  tmp_pos[1] = (final_dst[1] - pos[1]) * delta + pos[1];
  tmp_pos[2] = (final_dst[2] - pos[2]) * delta + pos[2];
  return tmp_pos; 
}
