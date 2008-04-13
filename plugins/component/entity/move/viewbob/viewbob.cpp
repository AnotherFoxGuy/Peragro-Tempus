/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include <cssysdef.h>
#include "plugins/component/entity/move/viewbob/viewbob.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

//#include "client/entity/entitymanager.h"
#include "client/entity/entity.h"
#include "client/entity/player/playerentity.h"

#include "client/data/sectordatamanager.h"
#include "client/data/sector.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
IMPLEMENT_COMPONENTFACTORY (ViewBob, "peragro.entity.move.viewbob")

ComponentViewBob::ComponentViewBob(iObjectRegistry* object_reg) :
	scfImplementationType (this, object_reg)
{
}

ComponentViewBob::~ComponentViewBob()
{
}

bool ComponentViewBob::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  using namespace PT::Events;
  using namespace PT::Entity;

//EventManager* evmgr = pointerlib->getEventManager();
/*
  // Register listener for move.
  csRef<EventHandlerCallback> cbMove;
  cbMove.AttachNew(new EventHandler<ComponentNetworkMove>(&ComponentNetworkMove::Move, this));
  evmgr->AddListener(EntityHelper::MakeEntitySpecific("entity.move", entity->GetId()), cbMove);
  eventHandlers.Push(cbMove);

  // Register listener for InterfaceOptionsEvent.
  csRef<EventHandlerCallback> cbUpdate;
  cbUpdate.AttachNew(new EventHandler<ComponentNetworkMove>(&ComponentNetworkMove::UpdateOptions, this));
  evmgr->AddListener("interface.options", cbUpdate);
  eventHandlers.Push(cbUpdate);

  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());
  local_movement = app_cfg->GetBool("Client.local_movement", false);
*/
  return true;
} // end Initialize()

