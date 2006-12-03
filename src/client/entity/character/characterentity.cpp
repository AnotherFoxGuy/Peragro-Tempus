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

PtCharacterEntity::PtCharacterEntity(EntityType type) : PtEntity(type) 
{
  equipment = new Equipment(this);
}

void PtCharacterEntity::Move(MovementData* movement) 
{
  if(!celentity.IsValid()) return;
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);
  if (pclinmove.IsValid())
  {
    pclinmove->SetAngularVelocity(csVector3(0,-movement->turn,0));
    pclinmove->SetVelocity(csVector3(0,0,-movement->walk));
  }
  csRef<iPcMesh> mesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  if (mesh.IsValid())
  {
    csRef<iSpriteCal3DState> sprcal3d =
       scfQueryInterface<iSpriteCal3DState> (mesh->GetMesh()->GetMeshObject());
    if (sprcal3d)
      sprcal3d->SetVelocity(movement->walk);
  }
}

bool PtCharacterEntity::MoveTo(MoveToData* moveTo)
{
  if(!celentity.IsValid()) return true;
  csRef<iVirtualClock> vc =  csQueryRegistry<iVirtualClock> (obj_reg);
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
  if(!celentity.IsValid()) return;
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);
  if (pclinmove.IsValid())
  {
    bool onGround = true;
    float speed = 0, rot = 0, avel = 0;
    csVector3 pos, vel, wvel;

    iSector* sector = 0;
    pclinmove->GetDRData(onGround, speed, pos, rot, sector, vel, wvel, avel);

    sector = engine->FindSector(drupdate->sector.GetData());
    pclinmove->SetDRData(onGround, speed, drupdate->pos, drupdate->rot, sector, vel, wvel, avel);
  }
}

void PtCharacterEntity::Teleport(csVector3 pos, csString sector)
{
  if(!celentity.IsValid()) return;
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  iMovable* mov = pcmesh->GetMesh()->GetMovable();
  mov->SetSector(engine->GetSectors()->FindByName(sector.GetData()));
  mov->SetPosition(pos);
}
