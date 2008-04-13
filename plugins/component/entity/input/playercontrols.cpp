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
#include "plugins/component/entity/input/playercontrols.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/actormove.h>

#include <propclass/defcam.h>
#include <iengine/camera.h>

#include "client/event/eventmanager.h"
#include "client/event/entityevent.h"
#include "client/event/inputevent.h"

#include "client/entity/entity.h"
#include "client/entity/player/playerentity.h"

//#include "client/entity/statmanager.h"

#include "common/network/entitymessages.h"
#include "client/network/network.h"

#include "client/data/sectordatamanager.h"
#include "client/data/sector.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

CS_IMPLEMENT_PLUGIN
IMPLEMENT_COMPONENTFACTORY (PlayerControls, "peragro.entity.input.playercontrols")

//-------------------------------------------------------------------------------------
#define REGISTER_LISTENER(Class, funct, ev, specific)			              \
using namespace PT::Events;                                                           \
csRef<EventHandlerCallback> cb##funct;                                                \
cb##funct.AttachNew(new EventHandler<##Class>(&##Class::##funct, this));              \
if (specific)                                                                         \
evmgr->AddListener(EntityHelper::MakeEntitySpecific(ev, entity->GetId()), cb##funct); \
else                                                                                  \
  evmgr->AddListener(ev, cb##funct);                                                  \
eventHandlers.Push(cb##funct);                                                        \
//-------------------------------------------------------------------------------------

ComponentPlayerControls::ComponentPlayerControls(iObjectRegistry* object_reg) :
	scfImplementationType (this, object_reg)
{
  walk = 0;
  turn = 0;
  run = false;
  jump = false;
  cameraDistance = 3.0f;
  backwardReverse = false;
  invertYAxis = false;
  minDistance = 50.0f;
  minFPS = 20.0f;
  maxFPS = 60.0f;
}

ComponentPlayerControls::~ComponentPlayerControls()
{
}

bool ComponentPlayerControls::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  camera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

    REGISTER_LISTENER(ComponentPlayerControls, ActionForward, "input.ACTION_FORWARD", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionBackward, "input.ACTION_BACKWARD", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionLeft, "input.ACTION_LEFT", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionRight, "input.ACTION_RIGHT", false)

    REGISTER_LISTENER(ComponentPlayerControls, ActionJump, "input.ACTION_JUMP", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionMoveTo, "input.ACTION_MOVETO", false)

    REGISTER_LISTENER(ComponentPlayerControls, ActionToggleWalk, "input.ACTION_TOGGLEWALK", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionToggleRun, "input.ACTION_TOGGLERUN", false)

    REGISTER_LISTENER(ComponentPlayerControls, ActionActivateWeapon, "input.ACTION_ACTIVATEWEAPON", false)

    REGISTER_LISTENER(ComponentPlayerControls, UpdateOptions, "interface.options", false)

    ///@TODO: Move to a camera component.
    REGISTER_LISTENER(ComponentPlayerControls, ActionPanUp, "input.ACTION_PANUP", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionPanDown, "input.ACTION_PANDOWN", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionToggleCamera, "input.ACTION_TOGGLECAMERA", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionToggleDistClipping, "input.ACTION_TOGGLEDISTCLIP", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionZoomIn, "input.ACTION_ZOOMIN", false)
    REGISTER_LISTENER(ComponentPlayerControls, ActionZoomOut, "input.ACTION_ZOOMOUT", false)

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
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());

  backwardReverse = app_cfg->GetBool("Client.backwardreverse", backwardReverse);
  local_movement = app_cfg->GetBool("Client.local_movement", false);
  invertYAxis = app_cfg->GetBool("Client.invertYAxis", invertYAxis);
  minDistance = app_cfg->GetFloat("Client.minDistance", minDistance);

  return true;
} // end UpdateOptions()

bool ComponentPlayerControls::PerformMovementAction()
{
  using namespace PT::Events;
  using namespace PT::Entity;

  // Local movement option
  if (local_movement && (static_cast<PcEntity*>(entity) && !static_cast<PcEntity*>(entity)->GetHasMount()))
  {
    PT::Events::EventManager* evmgr = pointerlib->getEventManager();
    csRef<iEvent> entityEvent = evmgr->CreateEvent(EntityHelper::MakeEntitySpecific("entity.move", entity->GetId()));
    entityEvent->Add("entityId", entity->GetId());
    entityEvent->Add("walkDirection", float(/*pointerlib->getStatManager()->GetStat("Speed")*/walk*(char(run)+1)));
    entityEvent->Add("turnDirection", float(walk == -1 && backwardReverse ? -turn : turn));
    entityEvent->Add("run", run);
    entityEvent->Add("jump", jump);
    entityEvent->Add("local", true);
    evmgr->AddEvent(entityEvent);
  }
/*
  MoveRequestMessage msg;

  msg.setWalk(walk+1);

  if (walk == -1 && backwardReverse) msg.setTurn(-turn+1);
  else msg.setTurn(turn+1);
  msg.setRun(run);
  msg.setJump(jump);
  jump=false; //Jumping is not a constant action, so we need to turn it off once we've
  //sent the request.
  pointerlib->getNetwork()->send(&msg);

  //When we move, we turn off sitting.
  // Shouldn't this be done for mouse walk too?
  if (sitting)
  {
    PoseRequestMessage poseMsg;
    poseMsg.setPoseId(0); //TODO: do a posemanager lookup for "idle"!
    pointerlib->getNetwork()->send(&poseMsg);
  }
*/
  return true;
} // end PerformMovementAction()

bool ComponentPlayerControls::ActionForward(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev)) walk = 1;
  else walk = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
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
  else walk = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
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
  else turn = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
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
  else turn = 0;

#ifdef _MOVEMENT_DEBUG_CHARACTER_
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(other_self, iPcActorMove);
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
    if (run==false)
    {
      run = true;
      //Setup values needed for bobbing during run
      //viewBobEffect.range = RUN_OFFSET_RANGE;
      //viewBobEffect.period = RUN_PERIOD;
    }
    else
    {
      run = false;
      //Setup values needed for bobbing during walk
      //viewBobEffect.range = WALK_OFFSET_RANGE;
      //viewBobEffect.period = WALK_PERIOD;
    }
  }
  PerformMovementAction();

  return true;
} // end ActionToggleRun()

bool ComponentPlayerControls::ActionPanUp(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (invertYAxis) camera->SetPitchVelocity(-1.0f);
    else camera->SetPitchVelocity(1.0f);
  }
  else
  {
    camera->SetPitchVelocity(0.0f);
  }

  return true;
} // end ActionPanUp()

bool ComponentPlayerControls::ActionPanDown(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (invertYAxis) camera->SetPitchVelocity(1.0f);
    else camera->SetPitchVelocity(-1.0f);
  }
  else
  {
    camera->SetPitchVelocity(0.0f);
  }

  return true;
} // end ActionPanDown()

bool ComponentPlayerControls::ActionToggleCamera(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    csRef<iPcActorMove> pcactormove =
      CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcActorMove);
    if (!pcactormove) return false;
    pcactormove->ToggleCameraMode();
  }

  return true;
} // end ActionToggleCamera()

bool ComponentPlayerControls::ActionToggleDistClipping(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (!entity->GetCelEntity()) return false;

    //pointerlib->getGUIManager()->GetChatWindow()->AddMessage("Toggled Distance Clipping.");

    if (camera->UseDistanceClipping()) camera->DisableDistanceClipping();
    else camera->EnableAdaptiveDistanceClipping(minFPS, maxFPS, minDistance);
  }

  return true;
} // end ActionToggleDistClipping()

bool ComponentPlayerControls::ActionActivateWeapon(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    PT::Entity::CharacterEntity* character = static_cast<PT::Entity::CharacterEntity*>(entity);
    if (character) return false;
    //character->PlayAnimation("attack_sword_s");
  }

  return true;
} // end ActionActivateWeapon()

bool ComponentPlayerControls::ActionZoomIn(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (!entity->GetCelEntity()) return false;
    cameraDistance -= 0.5;
    if (camera.IsValid()) camera->SetDistance(cameraDistance);
  }
  return true;
} // end ActionZoomIn()

bool ComponentPlayerControls::ActionZoomOut(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (!entity->GetCelEntity()) return false;
    cameraDistance += 0.5;
    if (camera.IsValid()) camera->SetDistance(cameraDistance);
  }
  return true;
} // end ActionZoomOut()

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
    /*
    Cursor* cursor = pointerlib->getCursor();
    csRef<iMeshWrapper> mesh = cursor->Get3DPointFrom2D(cam, &isect, &untransfCoord, &sector);

    if (mesh)
    {
      PT::Effect::EffectsManager* effectsmanager = PointerLibrary::getInstance()->getEffectsManager();
      effectsmanager->CreateEffect("MoveMarker", isect+csVector3(0,0.01f,0), sector);
      //effectsmanager->CreateDecal(isect+csVector3(0,0.25,0));

      csRef<iCelEntity> ownent = GetCelEntity();
      if (!ownent) return false;
      csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ownent, iPcLinearMovement);
      if (!pclinmove) return false;

      MoveToRequestMessage msg;
      msg.setTo(isect.x, isect.y, isect.z);
      msg.setRun(run);
      pointerlib->getNetwork()->send(&msg);

      //Report(PT::Debug, "OnMouseDown: position: %s", isect.Description().GetData());
    }
    else
    {
      //Report(PT::Warning, "OnMouseDown: Failed to find mesh!");
    }
  */
  }

  return true;
} // end ActionMoveTo()

