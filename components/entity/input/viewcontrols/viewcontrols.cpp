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
#include "components/entity/input/viewcontrols/viewcontrols.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/actormove.h>
#include <propclass/linmove.h>

#include <propclass/defcam.h>
#include <iengine/camera.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"
#include "common/event/inputevent.h"

#include "client/entity/entity.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#define YAW_SPEED 2.0f
#define PITCH_SPEED 1.0f
#define ZOOM_SPEED cameraDistance * 0.10f

CS_IMPLEMENT_PLUGIN
PT_IMPLEMENT_COMPONENTFACTORY (ViewControls, "peragro.entity.input.viewcontrols")

ComponentViewControls::ComponentViewControls(iObjectRegistry* object_reg)
  : scfImplementationType (this, object_reg), invertYAxis(false),
    cameraDistance(3.0f), distClip(false), minFPS(20.0f), maxFPS(60.0f),
    minDistance(50.0f)
{
}

ComponentViewControls::~ComponentViewControls()
{
}

bool ComponentViewControls::Initialize (PointerLibrary* pl, Client::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  camera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);

  PT_SETUP_HANDLER_PLUGIN(pointerlib)

  PT_REGISTER_LISTENER(ComponentViewControls, ActionLookLeft, "input.LookLeft")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionLookRight, "input.LookRight")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionLookUp, "input.LookUp")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionLookDown, "input.LookDown")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionZoomIn, "input.ZoomIn")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionZoomOut, "input.ZoomOut")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionCameraMode, "input.CameraMode")
  PT_REGISTER_LISTENER(ComponentViewControls, ActionDistanceClipping, "input.DistanceClipping")
  PT_REGISTER_LISTENER(ComponentViewControls, UpdateOptions, "interface.options.view")

  UpdateOptions();

  return true;
} // end Initialize()

bool ComponentViewControls::UpdateOptions(iEvent& ev)
{
  UpdateOptions();
  return false;
} // end UpdateOptions()

bool ComponentViewControls::UpdateOptions()
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());

  bool distClipTemp = distClip;

  invertYAxis = app_cfg->GetBool("Peragro.Movement.InvertYAxis", invertYAxis);
  distClip = app_cfg->GetBool("Peragro.Video.DistanceClipping", distClip);
  minFPS = app_cfg->GetFloat("Peragro.Video.MinFPS", minFPS);
  maxFPS = app_cfg->GetFloat("Peragro.Video.MaxFPS", maxFPS);
  minDistance = app_cfg->GetFloat("Peragro.Video.MinDistance", minDistance);

  if (distClipTemp != distClip) UpdateDistanceClipping();

  return true;
} // end UpdateOptions()

bool ComponentViewControls::ActionLookLeft(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    camera->SetYawVelocity(-YAW_SPEED);
  }
  else
  {
    if (camera->GetYawVelocity() < 0.0f)
      camera->SetYawVelocity(0.0f);
  }

  return true;
} // end ActionLookLeft()

bool ComponentViewControls::ActionLookRight(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    camera->SetYawVelocity(YAW_SPEED);
  }
  else
  {
    if (camera->GetYawVelocity() > 0.0f)
      camera->SetYawVelocity(0.0f);
  }

  return true;
} // end ActionLookRight()

bool ComponentViewControls::ActionLookUp(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (invertYAxis) camera->SetPitchVelocity(-PITCH_SPEED);
    else camera->SetPitchVelocity(PITCH_SPEED);
  }
  else
  {
    if ((invertYAxis && (camera->GetPitchVelocity() < 0.0f)) ||
        (!invertYAxis && (camera->GetPitchVelocity() > 0.0f)))
      camera->SetPitchVelocity(0.0f);
  }

  return true;
} // end ActionLookUp()

bool ComponentViewControls::ActionLookDown(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    if (invertYAxis) camera->SetPitchVelocity(PITCH_SPEED);
    else camera->SetPitchVelocity(-PITCH_SPEED);
  }
  else
  {
    if ((invertYAxis && (camera->GetPitchVelocity() > 0.0f)) ||
        (!invertYAxis && (camera->GetPitchVelocity() < 0.0f)))
      camera->SetPitchVelocity(0.0f);
  }

  return true;
} // end ActionLookDown()

bool ComponentViewControls::ActionZoomIn(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    // TODO figure out some way of changing the mouse wheel events, so this can
    // be changed to use camera->SetDistanceVelocity(), and work with keys.
    cameraDistance -= ZOOM_SPEED;
    camera->SetDistance(cameraDistance);
  }

  return true;
} // end ActionZoomIn()

bool ComponentViewControls::ActionZoomOut(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    // TODO figure out some way of changing the mouse wheel events, so this can
    // be changed to use camera->SetDistanceVelocity(), and work with keys.
    cameraDistance += ZOOM_SPEED;
    camera->SetDistance(cameraDistance);
  }

  return true;
} // end ActionZoomOut()

bool ComponentViewControls::ActionCameraMode(iEvent& ev)
{
  using namespace PT::Events;

  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (InputHelper::GetButtonDown(&ev))
  {
    csRef<iPcActorMove> pcactormove =
      CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcActorMove);
    if (!pcactormove) return false;
    pcactormove->ToggleCameraMode();
  }

  return true;
} // end ActionCameraMode()

bool ComponentViewControls::ActionDistanceClipping(iEvent& ev)
{
  using namespace PT::Events;

  if (InputHelper::GetButtonDown(&ev))
  {
    ///@TODO
    //pointerlib->getGUIManager()->GetChatWindow()->AddMessage("Toggled Distance Clipping.");

    csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (pointerlib->getObjectRegistry());

    distClip = !distClip;
    app_cfg->SetBool("Peragro.Video.DistanceClipping", distClip);

    return UpdateDistanceClipping();
  }

  return true;
} // end ActionDistanceClipping()

bool ComponentViewControls::UpdateDistanceClipping()
{
  if (!entity->GetCelEntity() || !camera.IsValid()) return false;

  if (distClip)
    camera->EnableAdaptiveDistanceClipping(minFPS, maxFPS, minDistance);
  else camera->DisableDistanceClipping();

  pointerlib->getReporter()->Report(PT::Debug, "Toggled distance clipping %s.", distClip?"on":"off");

  return true;
} // end UpdateDistanceClipping()

