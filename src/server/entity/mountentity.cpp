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


#include <wfmath/point.h>
#include <wfmath/vector.h>

#include "server/entity/user.h"
#include "server/entity/character.h"
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

void MountEntity::walkTo(const WFMath::Point<3>& dst_pos, float speed)
{
  // If we are already walking, lets store how
  // far we have come...
  if (isWalking) {
    ptScopedMonitorable<Entity> ent (entity.get());
    ent->SetPosition(this->GetPosition());
    isWalking = false;
  }

  final_dst = dst_pos;

  const WFMath::Point<3> pos = entity.get()->GetPosition();
  const float dist = Distance(final_dst, pos);

  t_org = (size_t) time(0);
  //v = s / t => t = s / v
  t_stop = (size_t) (dist / speed + time(0));

  isWalking = true;
}

WFMath::Point<3> MountEntity::GetPosition()
{
  if (!isWalking)
  {
    return entity.get()->GetPosition();
  }

  if ((size_t)time(0) >= t_stop)
  {
    ptScopedMonitorable<Entity> e (entity.get());
    e->SetPosition(final_dst);

    isWalking = false;
    return final_dst;
  }

  // pos will be org_pos until target is reached.
  WFMath::Point<3> pos = entity.get()->GetPosition();

  // TODO: Probably fixed now, need to verify though...
  size_t delta = ((size_t)time(0) - t_org) / (t_stop - t_org);
  tmp_pos = WFMath::Point<3>((final_dst - pos) * (float)delta + pos);
  return tmp_pos;
}
