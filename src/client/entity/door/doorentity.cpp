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

#include "common/data/doordatamanager.h"
#include "common/data/door.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{
  namespace Entity
  {

    DoorEntity::DoorEntity(const Events::EntityAddEvent& ev) : Entity(ev)
    {
      open = ev.open;
      locked = ev.locked;
      doorId = ev.typeId;

      Create();
    }

    void DoorEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
      PT::Data::Door* door =
        PointerLibrary::getInstance()->getDoorDataManager()->GetDoorById(doorId);

      if (door)
      {
        name = door->GetName();
        meshName = door->GetMeshName();
        CreateCelEntity();

        celEntity->SetName(name.c_str());

        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
        csRef<iMeshWrapper> doormesh = engine->FindMeshObject(meshName.c_str());

        if (doormesh) pcmesh->SetMesh(doormesh);
        else
        {
          Report(PT::Warning, "PtDoorEntity: Couldn't find mesh for door %s!", name.c_str());
          return;
        }

        csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celEntity,
          iPcProperties);
        pcprop->SetProperty("Door Open", open);
        pcprop->SetProperty("Door Locked", locked);

        //Use CEL's quest system for door state and simple animating.
        pl->CreatePropertyClass(celEntity, "pcquest");
        csRef<iPcQuest> pcquest = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcQuest);
        celQuestParams parameters;
        pcquest->NewQuest(door->GetQuestName().c_str(),parameters);
        pcquest->GetQuest()->SwitchState("closed");
      }
      else
        Report(PT::Error, "PtDoorEntity: Couldn't find mesh for door %d!\n", id);
    }

    void DoorEntity::UpdatePcProp(const UpdatePcPropData& update_pcprop)
    {
      csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celEntity,
        iPcProperties);

      switch(update_pcprop.value.type)
      {
      case CEL_DATA_BOOL:
        pcprop->SetProperty(update_pcprop.pcprop, update_pcprop.value.value.bo);
        break;
      case CEL_DATA_LONG:
        pcprop->SetProperty(update_pcprop.pcprop,
          (long) update_pcprop.value.value.l);
        break;
      case CEL_DATA_STRING:
        pcprop->SetProperty(update_pcprop.pcprop, update_pcprop.value.value.s);
        break;
      default:
        Report(PT::Error, "celData type not supported by updatePcProp!");
      }

      if (update_pcprop.value.type == CEL_DATA_BOOL)
      {
        if (update_pcprop.pcprop.CompareNoCase("Door Open"))
          this->SetOpen(update_pcprop.value.value.bo);

        if (update_pcprop.pcprop.CompareNoCase("Door Locked"))
          this->SetLocked(update_pcprop.value.value.bo);
      }
    }

    void DoorEntity::Interact()
    {
      using namespace PT::Events;

      InterfaceInteract* interfaceEvent = new InterfaceInteract();

      interfaceEvent->entityId = id;
      if (GetLocked()) interfaceEvent->actions = "Door, Unlock";
      else interfaceEvent->actions = "Door, Lock";
      PointerLibrary::getInstance()->getEventManager()->AddEvent(interfaceEvent);
    }
  }
}
