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

    CharacterEntity::CharacterEntity(const Events::EntityAddEvent& ev) :
      Entity(ev), equipment(this)
    {
      maxStamina = 100;
      currentStamina = 100;
      sitting = false;
      lastStatUpdate = 0;
      recoverStamina = 0.001f;
      drainStamina = 0.002f;

      //Add the equipment
      for(size_t i = 0; i < ev.equipment.GetSize(); i++)
        equipment.Equip(ev.equipment.Get(i).slotId, ev.equipment.Get(i).itemId);
    }

    void CharacterEntity::Move(const MovementData& movement)
    {
      if(!celEntity.IsValid()) return;

      csRef<iPcActorMove> pcactormove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);

      if (pcactormove.IsValid())
      {
        pcactormove->SetAnimationMapping(CEL_ANIM_IDLE, "idle");
        pcactormove->SetMovementSpeed(abs((int)movement.walk));
        pcactormove->SetRunningSpeed(abs((int)movement.walk));
        pcactormove->SetRotationSpeed(movement.run ? PI : PI);

        pcactormove->RotateLeft(movement.turn < 0.0f);
        pcactormove->RotateRight(movement.turn > 0.0f);

        pcactormove->Forward(movement.walk > 0.0f);
        pcactormove->Backward(movement.walk < 0.0f);

        if (movement.jump) pcactormove->Jump();
        else if (abs((int)movement.walk) > 0) pcactormove->Run(movement.run);
        else pcactormove->Run(false);
      }
    }

    bool CharacterEntity::MoveTo(MoveToData* moveTo)
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      if(!celEntity.IsValid()) return true;

      csRef<iVirtualClock> vc = csQueryRegistry<iVirtualClock> (obj_reg);
      csTicks ticks = vc->GetElapsedTicks ();

      if (!ticks) return true;

      float elapsed = ticks/1000.0;

      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
      csRef<iPcActorMove> pcactormove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);

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
        else if (moveTo->elapsed_time >= moveTo->walk_duration &&
                 moveTo->walking)
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

    void CharacterEntity::DrUpdate(const DrUpdateData& drupdate)
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      if(!celEntity.IsValid()) return;

      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);

      if (pclinmove.IsValid())
      {
        bool onGround = true;
        float speed = 0, rot = 0, avel = 0;
        csVector3 pos, vel, wvel;

        iSector* sector = 0;
        pclinmove->GetDRData(onGround, speed, pos, rot, sector, vel, wvel, avel);

        sector = engine->FindSector(drupdate.sector.GetData());
        ///@bug It seems that CEL interface has some issues. The below method,
        ///SetDRData seems not to take const references/pointers as arguments.
        csVector3 tempPos = drupdate.pos;
        float tempRot = drupdate.rot;
        pclinmove->SetDRData(onGround, speed, tempPos, tempRot, sector, vel,
          wvel, avel);
      }
    }

    void CharacterEntity::Teleport(const csVector3& pos,
                                   const std::string& sector)
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      if (!celEntity.IsValid()) return;

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      iMovable* mov = pcmesh->GetMesh()->GetMovable();

      mov->SetSector(engine->GetSectors()->FindByName(sector.c_str()));
      mov->SetPosition(pos);
      mov->UpdateMove();
    }

    void CharacterEntity::SetCurrentStamina(float x)
    {
      if (x >= 0 && x <= maxStamina) currentStamina = x;
    }

    void CharacterEntity::PlayAnimation(const char* animationName,
                                        float blend_factor, bool loop,
                                        bool stopOthers)
    {
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);

      if (!pcmesh.IsValid()) return;

      csRef<iGeneralMeshState> spstate(scfQueryInterface<iGeneralMeshState>
        (pcmesh->GetMesh()->GetMeshObject()));

      if (!spstate.IsValid()) return;

      csRef<iGenMeshSkeletonControlState> animcontrol(
        scfQueryInterface<iGenMeshSkeletonControlState>
        (spstate->GetAnimationControl()));

      if (!animcontrol.IsValid()) return;

      iSkeleton* skeleton = animcontrol->GetSkeleton ();

      if (!skeleton) return;

      if (stopOthers) skeleton->StopAll();

      iSkeletonAnimation* inst = skeleton->Execute(animationName, blend_factor);

      if (inst) inst->SetLoop(loop);
    }

    void CharacterEntity::Pose(unsigned int poseId)
    {
      ///@todo Get the hard-coded magic numbers out of this.
      if (poseId == 0)
      {
        if (sitting)
        {
          sitting = false;
          PlayAnimation("Sit_up", 1.0f, false, true);
          PlayAnimation("idle", 0.1f, true);
        }
      }
      else if (poseId == 1) PlayAnimation("hit", 0.5f, false, false);
      else if (poseId == 2)
      {
        if (!sitting)
        {
          sitting = true;
          PlayAnimation("Sit_down", 1.0f, false, true);
          PlayAnimation("Sit", 0.1f, true);
        }
      }
    }

    void CharacterEntity::UpdatePlayerStats()
    {
      csTicks time = csGetTicks();
      csRef<iPcActorMove> pcactormove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);
      csTicks diff = time - lastStatUpdate;

      // If lastStatUpdate == 0 then we have never updated the stats,
      // Lets not do this yet, since then we will base our update from
      // the program start, which is not correct.
      if (lastStatUpdate == 0)
      {
        lastStatUpdate = time;
        return;
      }

      if (pcactormove->IsRunning())
      {
        // Decrease stamina
        currentStamina -= diff * drainStamina;
        if (currentStamina < 0) currentStamina = 0;
      }
      else
      {
        // Increase stamina
        currentStamina += diff * recoverStamina;
        if (currentStamina > maxStamina) currentStamina = maxStamina;
      }
      lastStatUpdate = time;
    }
  }
}
