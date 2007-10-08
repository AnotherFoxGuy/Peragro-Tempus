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

#include "itementity.h"

#include "client/data/item/itemmanager.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{
  namespace Entity
  {

    ItemEntity::ItemEntity(const Events::EntityAddEvent& ev) : Entity(ev)
    {
      itemid = ev.typeId;

      Create();
    }

    void ItemEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      PT::Data::Item* item = PointerLibrary::getInstance()->getItemManager()->GetItemById(itemid);
      if(item)
      {
        name = item->GetName();
        meshname = item->GetMeshName();

        CreateCelEntity();

        char buffer[1024];
        sprintf(buffer, "%s:%d:%d", name.GetData(), type, id);
        celentity->SetName(buffer);

        // Load and assign the mesh to the entity.
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
        if (!pcmesh->SetMesh(meshname.GetData(), item->GetFileName().GetData()))
        {
          Report(PT::Error,  "PtItemEntity: Failed to load mesh: %s", meshname.GetData());
          pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
        }
        pl->CreatePropertyClass(celentity, "pcmove.linear");
        csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);

        pclinmove->InitCD(
          csVector3(0.5f,0.8f,0.5f),
          csVector3(0.5f,0.8f,0.5f),
          csVector3(0,0,0));

        iSector* sector = engine->FindSector(sectorname);
        pclinmove->SetPosition(pos,0,sector);
      }
      else
        Report(PT::Error, "PtItemEntity: Couldn't find mesh for item %d!\n", id);
    }

    void ItemEntity::Interact()
    {
      using namespace PT::Events;
      InterfaceInteract* interfaceEvent = new InterfaceInteract();
      interfaceEvent->entityId              = id;
      interfaceEvent->actions               = "Pickup";
      PointerLibrary::getInstance()->getEventManager()->AddEvent(interfaceEvent);
    }
  }
}
