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

#include <time.h>

#include <wfmath/point.h>
#include <wfmath/vector.h>

#include "server/entity/user.h"
#include "server/entity/pcentity.h"
#include "server/entity/mountentity.h"
#include "server/group/charchats.h"


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
  ptScopedMonitorable<Entity> e (entity.get());
  ptScopedMonitorable<Character> c (character);
  c->setEntity(e);
}

void PcEntity::walkTo(const WFMath::Point<3>& dst_pos, float speed)
{
  if (mount.get()) return;

  final_dst = dst_pos;

  const WFMath::Point<3>& pos = entity.get()->GetPosition();

  const float dist = Distance(final_dst, pos);

  //v = s / t => t = s / v
  t_stop = (size_t) (dist / speed + time(0));

  isWalking = true;
}

WFMath::Point<3> PcEntity::GetPosition()
{
  if (mount.get())
  {
    ptScopedMonitorable<MountEntity> e (mount.get());
    return e->GetPosition();
  }

  if (!isWalking)
  {
    return entity.get()->GetPosition();
  }
  else
  {
    if ((size_t)time(0) >= t_stop)
    {
      ptScopedMonitorable<Entity> ent (entity.get());
      ent->SetPosition(final_dst);

      isWalking = false;
      return final_dst;
    }
    else
    {
      const WFMath::Point<3> pos = entity.get()->GetPosition();
      //Not sure that's correct...
      size_t delta = t_stop - (size_t) time(0);
      tmp_pos = WFMath::Point<3>((final_dst - pos) * (float)delta);
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
