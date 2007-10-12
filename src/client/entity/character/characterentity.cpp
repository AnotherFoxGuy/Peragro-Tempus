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

#include <imesh/genmesh.h>
#include <imesh/gmeshskel2.h>
#include <imesh/skeleton.h>

#include "client/event/entityevent.h"

#include "client/reporter/reporter.h"

namespace PT
{
  namespace Entity
  {

    CharacterEntity::CharacterEntity(const Events::EntityAddEvent& ev) : Entity(ev), equipment(this)
    {
      maxStamina = 100;
      currentStamina = 100;

      //Add the equipment
      for(size_t i = 0; i < ev.equipment.GetSize(); i++)
        equipment.Equip(ev.equipment.Get(i).slotId, ev.equipment.Get(i).itemId);
    }

    void CharacterEntity::Move(MovementData* movement)
    {
      if(!celentity.IsValid()) return;
      csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcActorMove);

      if (pcactormove.IsValid())
      {
        pcactormove->SetAnimationMapping(CEL_ANIM_IDLE, "idle");
        pcactormove->SetMovementSpeed(abs((int)movement->walk));
        pcactormove->SetRunningSpeed(abs((int)movement->walk));
        pcactormove->SetRotationSpeed(movement->run ? PI : PI);

        pcactormove->RotateLeft(movement->turn < 0.0f);
        pcactormove->RotateRight(movement->turn > 0.0f);

        pcactormove->Forward(movement->walk > 0.0f);
        pcactormove->Backward(movement->walk < 0.0f);

        if (movement->jump) pcactormove->Jump();
        else if (abs((int)movement->walk) > 0)
          pcactormove->Run(movement->run);
        else
          pcactormove->Run(false);
      }
      if (movement->run)
      {
        SetCurrentStamina(GetCurrentStamina() - 1);
      }
      else
      {
        SetCurrentStamina(GetCurrentStamina() + 1);
      }
    }

    bool CharacterEntity::MoveTo(MoveToData* moveTo)
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

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
        pcactormove->SetAnimationMapping(CEL_ANIM_IDLE, "idle");

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

    void CharacterEntity::DrUpdate(DrUpdateData* drupdate)
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

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

    void CharacterEntity::Teleport(csVector3 pos, csString sector)
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      if(!celentity.IsValid()) return;
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
      iMovable* mov = pcmesh->GetMesh()->GetMovable();
      mov->SetSector(engine->GetSectors()->FindByName(sector.GetData()));
      mov->SetPosition(pos);
    }

    void CharacterEntity::SetCurrentStamina(unsigned int x)
    {
      if (x >= 0 && x <= maxStamina) currentStamina = x;
    }

    void CharacterEntity::Pose(unsigned int poseId)
    {
      //When you implement this, remove the include for reporter if you're not using it
      Report(PT::Error, "Implement the bloody pose No. '%d' for mesh '%s', you silly twat!", poseId, meshname.GetData());
      if (poseId == 1)
      {
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
        csRef<iGeneralMeshState> spstate (
          scfQueryInterface<iGeneralMeshState> (pcmesh->GetMesh()->GetMeshObject ()));
        csRef<iGenMeshSkeletonControlState> animcontrol (
          scfQueryInterface<iGenMeshSkeletonControlState> (spstate->GetAnimationControl ()));

        iSkeleton* skeleton = animcontrol->GetSkeleton ();

        skeleton->Execute("hit", 0.5f);
      }

    }
  }
}
