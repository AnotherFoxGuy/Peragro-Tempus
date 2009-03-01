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

#include "pcentity.h"

#include <iengine/engine.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "common/event/eventmanager.h"
#include "common/event/interfaceevent.h"
#include "common/event/entityevent.h"

#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"

#include "client/component/componentmanager.h"
#include "include/client/component/entity/mesh/mesh.h"

namespace PT
{
  namespace Entity
  {

    PcEntity::PcEntity(const iEvent& ev) : CharacterEntity(Common::Entity::PCEntityType, ev)
    {
      Create();
    }

    void PcEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
      csRef<iVFS> vfs =  csQueryRegistry<iVFS> (obj_reg);

      CreateCelEntity();

      csString buffer;
      buffer.Format("player_%d", id);
      celEntity->SetName(buffer);

      // Load and assign the mesh to the entity.
      PT::Component::ComponentManager* componentManager =
        PointerLibrary::getInstance()->getComponentManager();
      ADD_COMPONENT(componentManager, iMesh, "peragro.entity.mesh")

      pl->CreatePropertyClass(celEntity, "pcmove.actor.standard");
      pl->CreatePropertyClass(celEntity, "pcmove.linear");

      // Forcing into idle animation.
      PlayAnimation("idle", 1.0f, true, true);

      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);

      pclinmove->InitCD(csVector3(0.5f,0.8f,0.5f), csVector3(0.5f,0.8f,0.5f),
        csVector3(0,0,0));

      SetFullPosition();

      GetEquipment().ConstructMeshes();
    }

    void PcEntity::Interact()
    {
      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact");
      interfaceEvent->Add("entityId", id);
      std::string actions = "Trade, Attack, Party";
      interfaceEvent->Add("actions", actions.c_str());
      evmgr->AddEvent(interfaceEvent);
    }
  }
}
