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

#include <imesh/animesh.h>
#include <imesh/skeleton2.h>
#include <imesh/skeleton2anim.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/actormove.h>

#include "common/event/entityevent.h"

#include "common/reporter/reporter.h"

#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"

#include "client/component/componentmanager.h"
#include "include/client/component/entity/move/networkmove.h"
#include "include/client/component/entity/mesh/mesh.h"

namespace PT
{
  namespace Entity
  {
    CharacterEntity::CharacterEntity(Common::Entity::EntityType type)
      : ::Client::Entity::Entity(type), equipment(this), resourcesFact(new ResourcesFactory())
    {
      sitting = false;
      hasMount = false;

      pl->CreatePropertyClass(celEntity, "pcmove.actor.standard");
      pl->CreatePropertyClass(celEntity, "pcmove.linear");

      resources = resourcesFact->Create(this);

      PT::Component::ComponentManager* componentManager =
        PointerLibrary::getInstance()->getComponentManager();

      ADD_COMPONENT(componentManager, iNetworkMove,
        "peragro.entity.move.networkmove")
      ADD_COMPONENT(componentManager, iMesh, "peragro.entity.mesh")
    }

    void CharacterEntity::Initialize(const iEvent& ev)
    {
      Entity::Initialize(ev);

      resources->Initialize();

      csRef<iMesh> mesh = GetComponent<iMesh>("peragro.entity.mesh");
      mesh->Load();

      csRef<iNetworkMove> move = GetComponent<iNetworkMove>("peragro.entity.move.networkmove");
      move->Initialize(PointerLibrary::getInstance(), this);

      equipment.ClearAll();

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

      GetEquipment().ConstructMeshes();
    }

    void CharacterEntity::Teleport(const WFMath::Point<3>& pos,
      float rotation,
      const std::string& sector)
    {
      Report(PT::Warning, "CharacterEntity: teleport\n");

      SetFullPosition();
    }

    void CharacterEntity::PlayAnimationGenMesh(const char* animationName, float blend_factor,
                         bool loop, bool stopOthers )
    {
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);

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

    void CharacterEntity::PlayAnimationAniMesh(const char* animationName, float blend_factor,
                         bool loop, bool stopOthers)
    {
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);

      csRef<iAnimatedMesh> animesh = scfQueryInterface<iAnimatedMesh> (pcmesh->GetMesh()->GetMeshObject ());

      iSkeletonAnimNode2* root = animesh->GetSkeleton ()->GetAnimationPacket ()->GetAnimationRoot ();
      csRef<iSkeletonAnimNode2> anim;

      if (root)
      {
        anim = root->FindNode("standard");

        bool played = false;
        csRef<iSkeletonFSMNode2> fsm = scfQueryInterfaceSafe<iSkeletonFSMNode2> (anim);
        if (fsm)
        {
          csRef<iSkeletonFSMNodeFactory2> fsmfact = scfQueryInterface<iSkeletonFSMNodeFactory2>(anim->GetFactory());
          CS::Animation::StateID wanted_state = fsmfact->FindState(animationName);
          if (wanted_state != CS::Animation::InvalidStateID)
          {
            fsm->SwitchToState(wanted_state);
            played = true;
            root->Play();
          }
        }

        if (!played)
        {
          anim = root->FindNode(animationName);
          if (anim)
          {
            anim->Play();
          }
        } // end if (!played)
      } // end if (root)
    } // end PlayAnimationAniMesh()

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

      if (spstate && spstate->GetAnimationControl())
      {
        PlayAnimationGenMesh(animationName, blend_factor, loop, stopOthers);
        return;
      }

      csRef<iAnimatedMesh> animesh = scfQueryInterface<iAnimatedMesh> (pcmesh->GetMesh()->GetMeshObject ());
      if (animesh)
      {
        PlayAnimationAniMesh(animationName, blend_factor, loop, stopOthers);
        return;
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

  } // namespace Entity
} // namespace PT
