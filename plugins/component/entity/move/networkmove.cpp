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
#include "networkmove.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"

//#include "client/entity/entitymanager.h"
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

  // Register listener for InterfaceOptionsEvent.
  csRef<EventHandlerCallback> cbUpdate;
  cbUpdate.AttachNew(new EventHandler<NetworkMove>(&NetworkMove::UpdateOptions, this));
  evmgr->AddListener("interface.options", cbUpdate);
  eventHandlers.Push(cbUpdate);

  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());
  local_movement = app_cfg->GetBool("Client.local_movement", false);

  pointerlib->getReporter()->Report(PT::Error, "WOOT Initialize() %d!", entity->GetId());

  return true;
}

bool NetworkMove::UpdateOptions(iEvent& ev)
{/*
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());
  local_movement = app_cfg->GetBool("Client.local_movement", false);*/
  return true;
}

bool NetworkMove::Move(iEvent& ev)
{
  using namespace PT::Events;

  pointerlib->getReporter()->Report(PT::Error, "WOOT Move() %d!", entity->GetId());

  unsigned int id = entity->GetId();

  float walk = 0;
  float turn = 0;
  bool run = false;
  bool jump = false;
  bool local = false;
  csEventError error = csEventErrNone;
  

  error = ev.Retrieve("walkDirection", walk);
  error = ev.Retrieve("turnDirection", turn);
  error = ev.Retrieve("run", run);
  error = ev.Retrieve("jump", jump);
  error = ev.Retrieve("local", local);

  if (error != csEventErrNone)
  {
    pointerlib->getReporter()->Report(PT::Error, "NetworkMove::Move(): Failed to retrieve an attribute for %d!", id);
    return false;
  }

  //if (!local && local_movement && pointerlib->getEntityManager()->GetPlayerId() == id){ return false; }

  csWeakRef<iCelEntity> celEntity = entity->GetCelEntity();
  if(!celEntity.IsValid()) return false;

  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcActorMove);
  if (pcactormove.IsValid())
  {
    pcactormove->SetAnimationMapping(CEL_ANIM_IDLE, "idle");
    pcactormove->SetMovementSpeed(fabsf(walk));
    pcactormove->SetRunningSpeed(fabsf(walk));
    pcactormove->SetRotationSpeed(PI);

    pcactormove->RotateLeft(turn < 0.0f);
    pcactormove->RotateRight(turn > 0.0f);

    pcactormove->Forward(walk > 0.0f);
    pcactormove->Backward(walk < 0.0f);

    if (jump) pcactormove->Jump();
    else if (abs((int)walk) > 0) pcactormove->Run(run);
    else pcactormove->Run(false);
  }

  return false;
}
