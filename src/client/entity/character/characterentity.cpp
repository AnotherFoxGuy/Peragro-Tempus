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

#include <imesh/object.h>
#include <iengine/mesh.h>
#include <imesh/genmesh.h>
#include <imesh/gmeshskel2.h>
#include <imesh/skeleton.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/actormove.h>

#include "client/entity/entitymanager.h"

#include "common/event/entityevent.h"

#include "common/reporter/reporter.h"

#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"

#include "client/component/componentmanager.h"
#include "include/client/component/entity/move/networkmove.h"

namespace PT
{
  namespace Entity
  {

    CharacterEntity::CharacterEntity(const iEvent& ev) :
      Entity(ev), equipment(this)
    {
      maxStamina = 100;
      currentStamina = 100;
      sitting = false;
      lastStatUpdate = 0;
      recoverStamina = 0.001f;
      drainStamina = 0.002f;
      hasMount = false;


      //Add the equipment
      using namespace Events;
      csRef<iEvent> evequipment = EntityHelper::GetEquipment(&ev);
      if (evequipment)
      {
        csRef<iEventAttributeIterator> items = evequipment->GetAttributeIterator();
        while (items->HasNext())
        {
          csRef<iEvent> item; evequipment->Retrieve(items->Next(), item);
          unsigned int slotId, itemId;
          std::string mesh, file;
          item->Retrieve("slotId", slotId);
          item->Retrieve("itemId", itemId);
          mesh = Helper::GetString(item, "meshName");
          file = Helper::GetString(item, "fileName");
          equipment.Equip(slotId, itemId, mesh, file);
        }
      }
      else
        Report(PT::Error, "CharacterEntity failed to get equipment!");

      //--------------------------------------------------------------
      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      if (object_reg == 0)
        Report(PT::Error, "object_reg!");

      PT::Component::ComponentManager* componentManager =
        PointerLibrary::getInstance()->getComponentManager();

      ADD_COMPONENT(componentManager, iNetworkMove,
        "peragro.entity.move.networkmove")
    }

    void CharacterEntity::Teleport(const csVector3& pos,
                                   float rotation,
                                   const std::string& sector)
    {
      Report(PT::Warning, "CharacterEntity: teleport\n");

      SetFullPosition(pos, rotation, sector.c_str());
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
      if (!pcmesh->GetMesh()) return;
      if (!pcmesh->GetMesh()->GetMeshObject()) return;

      csRef<iGeneralMeshState> spstate(scfQueryInterface<iGeneralMeshState>
        (pcmesh->GetMesh()->GetMeshObject()));

      if (!spstate.IsValid()) return;
      if (!spstate->GetAnimationControl()) return;

      csRef<iGenMeshSkeletonControlState> animcontrol(
        scfQueryInterface<iGenMeshSkeletonControlState>
        (spstate->GetAnimationControl()));

      if (!animcontrol.IsValid()) return;

      csRef<iSkeleton> skeleton = animcontrol->GetSkeleton ();

      if (!skeleton) return;

      iSkeletonAnimation* script;
      if (stopOthers) script = 0;
      else script  = skeleton->FindAnimation (animationName);
      if (script)
      {
        if (script->GetLoop () != loop)
          script->SetLoop (loop);
      }
      else
      {
        skeleton->StopAll ();
        script = skeleton->Execute (animationName);
        if (script) script->SetLoop (loop);
      }
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
        }
        PlayAnimation("idle", 0.1f, true, false);
      }
      else if (poseId == 1)
      {
        PlayAnimation("hit", 0.5f, false, false);
      }
      else if (poseId == 2)
      {
        if (!sitting)
        {
          sitting = true;
          PlayAnimation("Sit_down", 0.5f, false, true);
        }
        PlayAnimation("Sit", 0.1f, false, false);
      }
    }

    void CharacterEntity::UpdatePlayerStats()
    {
      csTicks time = csGetTicks();

      if (!celEntity.IsValid())
      {
        Report(PT::Error, "Failed to player stats!");
        return;
      }

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
