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

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/interfaceevent.h"
#include "common/eventcs/entityevent.h"

#include "client/component/componentmanager.h"
#include "include/client/component/entity/mesh/mesh.h"

namespace PT
{
  namespace Entity
  {

    ItemEntity::ItemEntity() : ::Client::Entity::Entity(Common::Entity::ItemEntityType)
    {
      pl->CreatePropertyClass(celEntity, "pcmove.linear");

      // Load and assign the mesh to the entity.
      PT::Component::ComponentManager* componentManager =
        PointerLibrary::getInstance()->getComponentManager();
      ADD_COMPONENT(componentManager, iMesh, "peragro.entity.mesh")
    }

    void ItemEntity::Initialize(const iEvent& ev)
    {
      ::Client::Entity::Entity::Initialize(ev);

      csRef<iMesh> mesh = GetComponent<iMesh>("peragro.entity.mesh");
      mesh->Load();

      ev.Retrieve("itemId", itemId);

      csString buffer;
      buffer.Format("%s:%d:%d", name.c_str(), type, id);
      celEntity->SetName(buffer);
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
