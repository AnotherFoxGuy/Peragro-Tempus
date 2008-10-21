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

#include "doorentity.h"

#include <iengine/mesh.h>
#include <iengine/engine.h>
#include <iengine/movable.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
//#include <propclass/linmove.h>
#include <propclass/prop.h>
#include <propclass/quest.h>

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "common/event/eventmanager.h"
#include "common/event/interfaceevent.h"
#include "common/event/entityevent.h"

namespace PT
{
  namespace Entity
  {

    DoorEntity::DoorEntity(const iEvent& ev) : Entity(ev)
    {
      ev.Retrieve("open", open);
      ev.Retrieve("locked", locked);
      ev.Retrieve("typeId", doorId);
      animationName = PT::Events::Helper::GetString(&ev, "animationName");
      Create();

      ///@TODO: Move this to a component?
      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER_ENTITY_ID(DoorEntity, Open, "entity.door.open", doorId)
      PT_REGISTER_LISTENER_ENTITY_ID(DoorEntity, Close, "entity.door.close", doorId)
      PT_REGISTER_LISTENER_ENTITY_ID(DoorEntity, Lock, "entity.door.lock", doorId)
      PT_REGISTER_LISTENER_ENTITY_ID(DoorEntity, Unlock, "entity.door.unlock", doorId)
    }

    DoorEntity::~DoorEntity()
    {
    }

    void DoorEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      CreateCelEntity();
      celEntity->SetName(name.c_str());

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      csRef<iMeshWrapper> doormesh = engine->FindMeshObject(meshName.c_str());

      if (doormesh.IsValid())
      {
        pcmesh->SetMesh(doormesh);
        trans = doormesh->GetMovable()->GetTransform();
        Entity::position = doormesh->GetMovable()->GetFullPosition();
      }
      else
      {
        Report(PT::Warning, "DoorEntity: Couldn't find mesh '%s' for door %s!", meshName.c_str(), name.c_str());
        pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
        // Register listener for WorldLoaded.
        PT_SETUP_HANDLER
        PT_REGISTER_LISTENER(DoorEntity, TileLoaded, "world.loaded")
      }

      csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celEntity,
        iPcProperties);
      pcprop->SetProperty("Door Open", open);
      pcprop->SetProperty("Door Locked", locked);

      //Use CEL's quest system for door state and simple animating.
      pl->CreatePropertyClass(celEntity, "pclogic.quest");
      csRef<iPcQuest> pcquest = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcQuest);
      celQuestParams parameters;
      pcquest->NewQuest(animationName.c_str(), parameters);
      pcquest->GetQuest()->SwitchState("closed");
    }

    void DoorEntity::SetOpen(bool value)
    {
      csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcProperties);
      pcprop->SetProperty("Door Open", value);
      this->open = value;
    } //end SetOpen()

    void DoorEntity::SetLocked(bool value)
    {
      csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcProperties);
      pcprop->SetProperty("Door Locked", value);
      this->locked = value;
    } //end SetLocked()

    bool DoorEntity::Open(iEvent& ev)
    {
      using namespace PT::Events;
      Report(PT::Debug, "DoorEntity: Got open door %d.", doorId);
      if (Helper::HasError(&ev))
      {
        ///@TODO: Open a gui dialog here.
        Report(PT::Error, "Can't open %d! Reason: '%s'.", doorId, Helper::GetError(&ev).c_str());
      }
      else
        this->SetOpen(true);

      return false;
    } //end Open()

    bool DoorEntity::Close(iEvent& ev)
    {
      using namespace PT::Events;
      Report(PT::Debug, "DoorEntity: Got close door %d.", doorId);
      if (Helper::HasError(&ev))
      {
        ///@TODO: Open a gui dialog here.
        Report(PT::Error, "Can't close %d! Reason: '%s'.", doorId, Helper::GetError(&ev).c_str());
      }
      else
        this->SetOpen(false);

      return false;
    } //end Close()

    bool DoorEntity::Lock(iEvent& ev)
    {
      using namespace PT::Events;
      Report(PT::Debug, "DoorEntity: Got lock door %d.", doorId);
      if (Helper::HasError(&ev))
      {
        ///@TODO: Open a gui dialog here.
        Report(PT::Error, "Can't lock %d! Reason: '%s'.", doorId, Helper::GetError(&ev).c_str());
      }
      else
        this->SetLocked(true);

      return false;
    } //end Lock()

    bool DoorEntity::Unlock(iEvent& ev)
    {
      using namespace PT::Events;
      Report(PT::Debug, "DoorEntity: Got unlock door %d.", doorId);
      if (Helper::HasError(&ev))
      {
        ///@TODO: Open a gui dialog here.
        Report(PT::Error, "Can't unlock %d! Reason: '%s'.", doorId, Helper::GetError(&ev).c_str());
      }
      else
        this->SetLocked(false);

      return false;
    } //end Unlock()

    void DoorEntity::Interact()
    {
      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact");
      interfaceEvent->Add("entityId", id);
      std::string actions = "Door";
      if (GetLocked()) actions += ", Unlock";
      else actions += ", Lock";
      interfaceEvent->Add("actions", actions.c_str());
      evmgr->AddEvent(interfaceEvent);
    }

    void DoorEntity::Reset()
    {
      if (!celEntity.IsValid()) return;
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      if (!pcmesh.IsValid() || !pcmesh->GetMesh()) return;
      pcmesh->GetMesh()->GetMovable()->SetTransform(trans);
    }

    bool DoorEntity::TileLoaded(iEvent& ev)
    {
      std::string meshName = GetMeshName();
      csRef<iCelEntity> celEntity = GetCelEntity();
      if (!celEntity) return false;
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iMeshWrapper> doormesh = engine->FindMeshObject(meshName.c_str());

      if (doormesh.IsValid())
      {
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
        if (!pcmesh.IsValid()) return false;
        pcmesh->SetMesh(doormesh);
        csRef<iPcQuest> pcquest = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcQuest);
        pcquest->GetQuest()->SwitchState(open ? "open" : "closed");
      }

      return false;
    }
  }
}
