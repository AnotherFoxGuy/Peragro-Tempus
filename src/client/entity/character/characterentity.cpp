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

#include "characterentity.h"

bool PtCharacterEntity::MoveTo(MoveToData* moveTo)
{
  csRef<iVirtualClock> vc = CS_QUERY_REGISTRY (obj_reg, iVirtualClock);
  csTicks ticks = vc->GetElapsedTicks ();
  if (!ticks) return true;

  float elapsed = ticks/1000.0;

  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcActorMove);

  if (pclinmove.IsValid() && pcactormove.IsValid())
  {
    csVector3 angular_vel;
    pclinmove->GetAngularVelocity(angular_vel);

    if (moveTo->elapsed_time == 0 && !moveTo->walking)
    {
      pcactormove->SetRotationSpeed(moveTo->turn_speed);
      pcactormove->RotateTo(moveTo->dest_angle);
    }
    else if (angular_vel.IsZero() && !moveTo->walking)
    {
      pcactormove->SetMovementSpeed(moveTo->walk_speed);
      pcactormove->Forward(true);
      moveTo->walking = true;
      moveTo->elapsed_time = 0;
    }
    else if (moveTo->elapsed_time >= moveTo->walk_duration && moveTo->walking)
    {
      pcactormove->Forward(false);
    }

    if (moveTo->walking)
    {
      if (moveTo->elapsed_time >= moveTo->walk_duration)
      {
        pcactormove->Forward(false);
        return true;
      }
    }

    moveTo->elapsed_time += elapsed;
    return false;
  }

  return true;
}

void PtCharacterEntity::DrUpdate(DrUpdateData* drupdate)
{
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);
  if (pclinmove.IsValid())
  {
    iSector* sector = engine->FindSector(drupdate->sector.GetData());
    pclinmove->SetDRData(drupdate->on_ground, drupdate->speed, drupdate->pos,
      drupdate->rot, sector, drupdate->vel, drupdate->wvel, drupdate->avel);
  }
}