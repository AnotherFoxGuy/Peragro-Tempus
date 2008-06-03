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
#include <propclass/linmove.h>

#include <propclass/defcam.h>
#include <iengine/camera.h>

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/entity/entity.h"
#include "client/entity/player/playerentity.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

// These defines should probably go to configuration file.
// PERIOD defines were calculated from the player character animations.
// OFFSET_RANGE defines were made up.
#define WALK_PERIOD 499
#define WALK_OFFSET_RANGE 0.03f
#define RUN_PERIOD 416
#define RUN_OFFSET_RANGE 0.08f
#define HEAD_HEIGHT 1.55f

CS_IMPLEMENT_PLUGIN
IMPLEMENT_COMPONENTFACTORY (ViewBob, "peragro.entity.move.viewbob")

ComponentViewBob::ComponentViewBob(iObjectRegistry* object_reg) :
  scfImplementationType (this, object_reg)
{
  baseHeight = HEAD_HEIGHT;
  currentOffset = 0.0f;
  timePeriod = WALK_PERIOD;
  offsetRange = WALK_OFFSET_RANGE;
}

ComponentViewBob::~ComponentViewBob()
{
}

bool ComponentViewBob::Initialize (PointerLibrary* pl, PT::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  camera = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcDefaultCamera);
  csVector3 offset(0.0f, baseHeight, 0.0f);
  camera->SetFirstPersonOffset(offset);

  vc = csQueryRegistry<iVirtualClock> (pointerlib->getObjectRegistry());
  if (!vc) return pointerlib->getReporter()->
    Report(PT::Error, "Failed to locate Virtual Clock!");
  lastTicks = vc->GetCurrentTicks();

  using namespace PT::Events;
  using namespace PT::Entity;

  EventManager* evmgr = pointerlib->getEventManager();

  REGISTER_LISTENER(ComponentViewBob, Frame, "crystalspace.frame", false)

  return true;
} // end Initialize()

bool ComponentViewBob::Frame(iEvent& ev)
{
  if (!camera.IsValid()) return false;

  // If at least 1 tick has elapsed.
  csTicks elapsedTicks = vc->GetCurrentTicks() - lastTicks;
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
        offsetChanged = Move(elapsedTicks);
        if (!offsetChanged)
        {
          // The distance moved is probably too small to measure for this time
          // period, return without updating offset or lastTicks.
          return false;
        }
      }
      else
      {
        // In first person mode, not moving or in the air.
        offsetChanged = Reset(false, elapsedTicks);
      }
    }
    else
    {
      // Not in first person mode, reset the height now.
      offsetChanged = Reset(true);
    }

    // If offset was changed, shift the camera.
    if (offsetChanged)
    {
      csVector3 offset(0.0f, baseHeight + currentOffset, 0.0f);
      camera->SetFirstPersonOffset(offset);
    }
    // This is done here so that the time accumulates when the distance moved
    // is too small, to keep the calculations correct.
    lastTicks = vc->GetCurrentTicks();
  }

  return false;
} // end Frame()

bool ComponentViewBob::Move(float elapsedTicks)
{
  // 4 parts of the cycle, increases and decreases through + and -.
  float offsetChange = elapsedTicks / (timePeriod/4) * offsetRange;
  if (offsetChange < 0.00001f && offsetChange > -0.00001f) return false;

  if (upwards)
  {
    // If the offset is above the range, change the direction.
    if (currentOffset + offsetChange >= offsetRange)
    {
      currentOffset = offsetRange;
      upwards = false;
    }
    else
    {
      currentOffset += offsetChange;
    }
  }
  else
  {
    // If the offset is below the range, change the direction.
    if (currentOffset - offsetChange <= -1.0f * offsetRange)
    {
      currentOffset = -1.0f * offsetRange;
      upwards = true;
    }
    else
    {
      currentOffset -= offsetChange;
    }
  }
  return true;
} // end Move()

bool ComponentViewBob::Reset(bool hard, float elapsedTicks)
{
  if (currentOffset < 0.00001f && currentOffset > -0.00001f) return false;
  if (hard)
  {
    currentOffset = 0.0f;
    return true;
  }

  // 4 parts of the cycle, increases and decreases through + and -.
  float offsetChange = elapsedTicks / (timePeriod/4) * offsetRange;
  // Above the base height, change down toward it.
  if (currentOffset > 0.0f)
  {
    if (currentOffset - offsetChange <= 0.0f)
    {
      currentOffset = 0.0f;
      upwards = true;
    }
    else
    {
      currentOffset -= offsetChange;
    }
  }
  // Below the base height, change up toward it.
  else if (currentOffset < 0.0f)
  {
    if (currentOffset + offsetChange >= 0.0f)
    {
      currentOffset = 0.0f;
      upwards = true;
    }
    else
    {
      currentOffset += offsetChange;
    }
  }
  return true;
} // end Reset()

