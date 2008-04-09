/*
    Copyright (C) 2001 by Jorrit Tyberghein

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <cssysdef.h>
#include <iutil/objreg.h>
#include "networkmove.h"

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

#include "client/entity/entity.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY (NetworkMove)

NetworkMove::NetworkMove(iBase* parent) :
	scfImplementationType (this, parent),
	object_reg(0)
{
}

NetworkMove::~NetworkMove()
{
}

bool NetworkMove::Initialize (iObjectRegistry* r)
{
  object_reg = r;
  return true;
}

bool NetworkMove::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  // Register listener for ActionForward.
  csRef<EventHandlerCallback> cbMove;
  cbMove.AttachNew(new EventHandler<NetworkMove>(&NetworkMove::Move, this));
  evmgr->AddListener(EntityHelper::MakeEntitySpecific("entity.move", entity->GetId()), cbMove);
  eventHandlers.Push(cbMove);

  pointerlib->getReporter()->Report(PT::Error, "WOOT Initialize() %d!", entity->GetId());

  return true;
}

bool NetworkMove::Move(iEvent& ev)
{
  using namespace PT::Events;

  pointerlib->getReporter()->Report(PT::Error, "WOOT Move() %d!", entity->GetId());

  return false;
}
