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

#include <iengine/engine.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{
  namespace Entity
  {

    ItemEntity::ItemEntity(const iEvent& ev) : Entity(ev)
    {
      ev.Retrieve("itemId", itemId);

      Create();
    }

    void ItemEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      CreateCelEntity();

      csString buffer;
      buffer.Format("%s:%d:%d", name.c_str(), type, id);
      celEntity->SetName(buffer);

      // Load and assign the mesh to the entity.
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      if (!pcmesh->SetMesh(meshName.c_str(), fileName.c_str()))
      {
        Report(PT::Error,  "PtItemEntity: Failed to load mesh: %s",
          meshName.c_str());
        pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
      }

      pl->CreatePropertyClass(celEntity, "pcmove.linear");
      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);

      pclinmove->InitCD(csVector3(0.5f,0.8f,0.5f), csVector3(0.5f,0.8f,0.5f),
        csVector3(0,0,0));

      SetFullPosition(pos, rot, sectorName.c_str());
    }

    void ItemEntity::Interact()
    {
      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact");
      interfaceEvent->Add("entityId", id);
      std::string actions = "Pickup";
      interfaceEvent->Add("actions", actions.c_str());
      evmgr->AddEvent(interfaceEvent);
    }
  }
}
