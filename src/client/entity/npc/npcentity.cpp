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

#include "npcentity.h"

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

#include "client/component/componentmanager.h"
#include "include/client/component/entity/mesh/mesh.h"

namespace PT
{
  namespace Entity
  {

    NpcEntity::NpcEntity() : CharacterEntity(Common::Entity::NPCEntityType)
    {
      pl->CreatePropertyClass(celEntity, "pcmove.actor.standard");
      pl->CreatePropertyClass(celEntity, "pcmove.linear");
    }

    void NpcEntity::Initialize(const iEvent& ev)
    {
      CharacterEntity::Initialize(ev);

      csString buffer;
      buffer.Format("npc_%d", id);
      celEntity->SetName(buffer);

      // Forcing into idle animation.
      PlayAnimation("idle", 1.0f, true, true);
    }

    void NpcEntity::Interact()
    {
      PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      csRef<iEvent> interfaceEvent = evmgr->CreateEvent("interface.interact");
      interfaceEvent->Add("entityId", id);
      std::string actions = "Talk";
      interfaceEvent->Add("actions", actions.c_str());
      evmgr->AddEvent(interfaceEvent);
    }

  } // namespace Entity
} // namespace PT

