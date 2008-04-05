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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{
  namespace Entity
  {

    DoorEntity::DoorEntity(const iEvent& ev) : Entity(ev)
    {
      ev.Retrieve("open", open);
      ev.Retrieve("locked", locked);
      ev.Retrieve("typeId", doorId);
      animationName = PT::Events::EntityHelper::GetString(&ev, "animationName");
      Create();
    }

    DoorEntity::~DoorEntity()
    {
     csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      engine->RemoveEngineSectorCallback(cb);
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
        Entity::pos = doormesh->GetMovable()->GetFullPosition();
      }
      else
      {
        Report(PT::Warning, "DoorEntity: Couldn't find mesh '%s' for door %s!", meshName.c_str(), name.c_str());
        pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
        cb.AttachNew(new SectorCallBack(this));
        engine->AddEngineSectorCallback(cb);
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

    void DoorEntity::UpdatePcProp(const UpdatePcPropData& update_pcprop)
    {
      csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celEntity,
        iPcProperties);

      switch(update_pcprop.value.type)
      {
      case CEL_DATA_BOOL:
        pcprop->SetProperty(update_pcprop.pcprop.c_str(), update_pcprop.value.value.bo);
        break;
      case CEL_DATA_LONG:
        pcprop->SetProperty(update_pcprop.pcprop.c_str(),
          (long) update_pcprop.value.value.l);
        break;
      case CEL_DATA_STRING:
        pcprop->SetProperty(update_pcprop.pcprop.c_str(), update_pcprop.value.value.s);
        break;
      default:
        Report(PT::Error, "celData type not supported by updatePcProp!");
      }

      if (update_pcprop.value.type == CEL_DATA_BOOL)
      {
        csString state = update_pcprop.pcprop.c_str();
        if (state.CompareNoCase("Door Open"))
          this->SetOpen(update_pcprop.value.value.bo);

        if (state.CompareNoCase("Door Locked"))
          this->SetLocked(update_pcprop.value.value.bo);
      }
    }

    void DoorEntity::Interact()
    {
      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact", true);
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

    void DoorEntity::SectorCallBack::NewSector(iEngine* engine, iSector* sector)
    {
      if (!entity) return;
      std::string meshName = entity->GetMeshName();
      iCelEntity* celEntity = entity->GetCelEntity();
      if (!celEntity) return;
      csRef<iMeshWrapper> doormesh = engine->FindMeshObject(meshName.c_str());

      if (doormesh.IsValid())
      {
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
        if (!pcmesh.IsValid()) return;
        pcmesh->SetMesh(doormesh);
        engine->RemoveEngineSectorCallback(this);
      }

    }
  }
}
