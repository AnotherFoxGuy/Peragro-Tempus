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
#include "plugins/component/entity/input/playercontrols/playercontrols.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>
#include <iengine/sector.h>
#include <iutil/object.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/actormove.h>
#include <propclass/linmove.h>

#include <propclass/defcam.h>
#include <iengine/camera.h>

#include "client/cursor/cursor.h"
#include "client/effect/effectsmanager.h"

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"
#include "client/event/inputevent.h"

#include "client/entity/entity.h"
#include "client/entity/character/characterentity.h"

//#include "client/entity/statmanager.h"

#include "common/network/entitymessages.h"
#include "client/network/network.h"

#include "client/data/sectordatamanager.h"
#include "client/data/sector.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
IMPLEMENT_COMPONENTFACTORY (PlayerControls, "peragro.entity.input.playercontrols")

ComponentPlayerControls::ComponentPlayerControls(iObjectRegistry* object_reg) :
  scfImplementationType (this, object_reg)
{
  walk = 0;
  turn = 0;
  run = false;
  jump = false;
  backwardReverse = false;
  local_movement = false;
}

ComponentPlayerControls::~ComponentPlayerControls()
{
}

bool ComponentPlayerControls::Initialize (PointerLibrary* pl,
  PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  camera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);

  using namespace PT::Events;
  EventManager* evmgr = pointerlib->getEventManager();

  REGISTER_LISTENER(ComponentPlayerControls, ActionForward,
    "input.ACTION_FORWARD", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionBackward,
    "input.ACTION_BACKWARD", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionLeft,
    "input.ACTION_LEFT", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionRight,
    "input.ACTION_RIGHT", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionJump,
    "input.ACTION_JUMP", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionMoveTo,
    "input.ACTION_MOVETO", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionToggleWalk,
    "input.ACTION_TOGGLEWALK", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionToggleRun,
    "input.ACTION_TOGGLERUN", false)
  REGISTER_LISTENER(ComponentPlayerControls, ActionActivateWeapon,
    "input.ACTION_ACTIVATEWEAPON", false)
  REGISTER_LISTENER(ComponentPlayerControls, UpdateOptions,
    "interface.options", false)

  UpdateOptions();

  return true;
} // end Initialize()

bool ComponentPlayerControls::UpdateOptions(iEvent& ev)
{
  UpdateOptions();
  return false;
} // end UpdateOptions()

bool ComponentPlayerControls::UpdateOptions()
{
  csRef<iConfigManager> app_cfg =
    csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());

  backwardReverse = app_cfg->GetBool("Client.backwardreverse", backwardReverse);
  local_movement = app_cfg->GetBool("Client.local_movement", local_movement);

  return true;
} // end UpdateOptions()

bool ComponentPlayerControls::PerformMovementAction()
{
  using namespace PT::Events;
  using namespace PT::Entity;

  // Local movement option.
  if (local_movement && (entity->GetType() == PT::Entity::PlayerEntityType &&
    !static_cast<CharacterEntity*>(entity)->GetHasMount()))
  {
    PT::Events::EventManager* evmgr = pointerlib->getEventManager();
    csRef<iEvent> entityEvent = evmgr->CreateEvent(
      EntityHelper::MakeEntitySpecific("entity.move", entity->GetId()));
    entityEvent->Add("entityId", entity->GetId());
    ///@TODO
    entityEvent->Add("walkDirection",
     float(/*pointerlib->getStatManager()->GetStat("Speed")*/4*walk*(char(run)+1)));
    entityEvent->Add("turnDirection",
      float(walk == -1 && backwardReverse ? -turn : turn));
    entityEvent->Add("run", run);
    entityEvent->Add("jump", jump);
    entityEvent->Add("local", true);
    evmgr->AddEvent(entityEvent);
  }

  MoveRequestMessage msg;

  msg.setWalk(walk + 1);

  if (walk == -1 && backwardReverse) msg.setTurn(-turn + 1);
  else msg.setTurn(turn + 1);
  msg.setRun(run);
  msg.setJump(jump);
  // Jumping is not a constant action, so we need to turn it off once we've
  // sent the request.
  jump=false;
  pointerlib->getNetwork()->send(&msg);

  // When we move, we turn off sitting.
  // Shouldn't this be done for mouse walk too?
  if (sitting)
  {
    PoseRequestMessage poseMsg;
    poseMsg.setPoseId(0); //TODO: do a posemanager lookup for "idle"!
    pointerlib->getNetwork()->send(&poseMsg);
  }

  return true;
} // end PerformMovementAction()

bool ComponentPlayerControls::ActionForward(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) walk = 1;
  else if (walk > 0) walk = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove =
    CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
  pcactormove->SetMovementSpeed(4);
  pcactormove->Forward(walk != 0);
#endif

  PerformMovementAction();

  return true;
} // end ActionForward()

bool ComponentPlayerControls::ActionBackward(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) walk = -1;
  else if (walk < 0) walk = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove =
    CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
  pcactormove->SetMovementSpeed(4);
  pcactormove->Backward(walk != 0);
#endif

  PerformMovementAction();

  return true;
} // end ActionBackward()

bool ComponentPlayerControls::ActionLeft(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) turn = -1;
  else if (turn < 0) turn = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove =
    CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
  pcactormove->SetRotationSpeed(3);
  pcactormove->RotateLeft(turn != 0);
#endif

  PerformMovementAction();

  return true;
} // end ActionLeft()

bool ComponentPlayerControls::ActionRight(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) turn = 1;
  else if (turn > 0) turn = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove =
    CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
  pcactormove->SetRotationSpeed(3);
  pcactormove->RotateRight(turn != 0);
#endif

  PerformMovementAction();

  return true;
} // end ActionRight()

bool ComponentPlayerControls::ActionToggleWalk(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) (walk == 0) ? walk = 1 : walk = 0;

  PerformMovementAction();

  return true;
} // end ActionToggleWalk()

bool ComponentPlayerControls::ActionToggleRun(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (run == false) run = true;
    else run = false;
  }

  PerformMovementAction();

  return true;
} // end ActionToggleRun()

bool ComponentPlayerControls::ActionActivateWeapon(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    PT::Entity::CharacterEntity* character = static_cast<PT::Entity::CharacterEntity*>(entity);
    if (character) return false;
    ///@TODO
    //character->PlayAnimation("attack_sword_s");
  }

  return true;
} // end ActionActivateWeapon()

bool ComponentPlayerControls::ActionJump(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) jump=true;

  PerformMovementAction();

  return true;
} // end ActionJump()

bool ComponentPlayerControls::ActionMoveTo(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (!camera) return false;
    csRef<iCamera> cam = camera->GetCamera();
    if (!cam) return false;

    csVector3 isect, untransfCoord;
    iSector* sector = 0;
    Cursor* cursor = pointerlib->getCursor();
    csRef<iMeshWrapper> mesh =
      cursor->Get3DPointFrom2D(cam, &isect, &untransfCoord, &sector);

    if (mesh)
    {
      using namespace PT::Events;

      PT::Events::EventManager* evmgr = pointerlib->getEventManager();
      csRef<iEvent> effectEvent = evmgr->CreateEvent("effect.atposition");
      effectEvent->Add("effect", "MoveMarker");
      PT::Events::EntityHelper::SetVector3(
        effectEvent, "position", isect+csVector3(0,0.01f,0));
      if (sector)
        effectEvent->Add("sector", sector->QueryObject()->GetName());
      evmgr->AddEvent(effectEvent);

      csRef<iCelEntity> ownent = entity->GetCelEntity();
      if (!ownent) return false;
      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(ownent, iPcLinearMovement);
      if (!pclinmove) return false;

      MoveToRequestMessage msg;
      msg.setTo(isect.x, isect.y, isect.z);
      msg.setRun(run);
      pointerlib->getNetwork()->send(&msg);

      pointerlib->getReporter()->Report(
        PT::Debug, "OnMouseDown: position: %s", isect.Description().GetData());
    }
    else
    {
      pointerlib->getReporter()->Report(
        PT::Warning, "OnMouseDown: Failed to find mesh!");
    }
  }

  return true;
} // end ActionMoveTo()

