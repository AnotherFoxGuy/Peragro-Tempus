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

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
//#include <propclass/actormove.h>
#include <propclass/linmove.h>

#include <propclass/defcam.h>
#include <iengine/camera.h>

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

// These defines should probably go to configuration file.
// PERIOD defines were calculated from the player character animations.
// OFFSET_RANGE defines were made up.
#define WALK_PERIOD 499
#define WALK_OFFSET_RANGE 0.03f
#define RUN_PERIOD 416
#define RUN_OFFSET_RANGE 0.08f
#define HEAD_HEIGHT 1.55f

ComponentViewBob::ComponentViewBob(iObjectRegistry* object_reg) :
	scfImplementationType (this, object_reg)
{
  viewBobEffect.base = HEAD_HEIGHT;
  viewBobEffect.offset = 0.0f;
  viewBobEffect.period = WALK_PERIOD;
  viewBobEffect.range = WALK_OFFSET_RANGE;
}

ComponentViewBob::~ComponentViewBob()
{
}

bool ComponentViewBob::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  camera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);

  vc = csQueryRegistry<iVirtualClock> (pointerlib->getObjectRegistry());
  if (!vc) return pointerlib->getReporter()->Report(PT::Error, "Failed to locate Virtual Clock!");

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  REGISTER_LISTENER(ComponentViewBob, Frame, "crystalspace.frame", false)

  return true;
} // end Initialize()

bool ComponentViewBob::Frame(iEvent& ev)
{
  if (!camera.IsValid()) return false;

  csTicks elapsedTicks = vc->GetElapsedTicks();

  if (elapsedTicks > 0)
  {
    bool offsetChanged;
    if (camera->GetMode() == iPcDefaultCamera::firstperson &&
      entity->GetCelEntity())
    {
      // TODO: Make the effect work with mounts.
      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcLinearMovement);
      if (pclinmove && !pclinmove->GetVelocity().IsZero() &&
        pclinmove->IsOnGround())
      {
        // In first person mode, moving and on the ground.
        offsetChanged = viewBobEffect.Move(elapsedTicks);
        if (!offsetChanged) Report(PT::Error,
          "Camera offset change too small for bobbing effect, elapsed ticks: %lu",
          elapsedTicks);
      }
      else
      {
        // In first person mode, not moving or in the air.
        offsetChanged = viewBobEffect.Reset(false, elapsedTicks);
      }
    }
    else
    {
      // Not in first person mode, reset the height now.
      offsetChanged = viewBobEffect.Reset(true);
    }

    // If offset was changed, shift the camera.
    if (offsetChanged)
    {
      csVector3 offset(0.0f, viewBobEffect.base+viewBobEffect.offset, 0.0f);
      camera->SetFirstPersonOffset(offset);
    }
  }

  return false;
}

bool ComponentViewBob::ViewBobEffect::Move(float elapsedTicks)
{
  // 4 parts of the cycle, increases and decreases through + and -.
  float offsetChange = elapsedTicks / (period/4) * range;
  if (offsetChange < 0.00001f && offsetChange > -0.00001f) return false;

  if (upwards)
  {
    // If the offset is above the range, change the direction.
    if (offset + offsetChange >= range)
    {
      offset = range;
      upwards = false;
    }
    else
    {
      offset += offsetChange;
    }
  }
  else
  {
    // If the offset is below the range, change the direction.
    if (offset - offsetChange <= -1.0f * range)
    {
      offset = -1.0f * range;
      upwards = true;
    }
    else
    {
      offset -= offsetChange;
    }
  }
  return true;
} // end ViewBobEffect::Move()

bool ComponentViewBob::ViewBobEffect::Reset(bool hard, float elapsedTicks)
{
  if (offset < 0.00001f && offset > -0.00001f) return false;
  if (hard)
  {
    offset = 0.0f;
    return true;
  }

  // 4 parts of the cycle, increases and decreases through + and -.
  float offsetChange = elapsedTicks / (period/4) * range;
  // Above the base height, change down toward it.
  if (offset > 0.0f)
  {
    if (offset - offsetChange <= 0.0f)
    {
      offset = 0.0f;
      upwards = true;
    }
    else
    {
      offset -= offsetChange;
    }
  }
  // Below the base height, change up toward it.
  else if (offset < 0.0f)
  {
    if (offset + offsetChange >= 0.0f)
    {
      offset = 0.0f;
      upwards = true;
    }
    else
    {
      offset += offsetChange;
    }
  }
  return true;
} // end ViewBobEffect::Reset()

