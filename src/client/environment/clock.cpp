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

#include "clock.h"
#include "client/pointer/pointer.h"
#include "common/event/environmentevent.h"
#include "common/reporter/reporter.h"
#include "common/event/eventmanager.h"

#include <sstream>

#include <iengine/engine.h>
#include <iengine/mesh.h>
#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <iengine/sector.h>
#include <iengine/rview.h>
#include <iengine/camera.h>
#include <ivideo/shader/shader.h>

namespace PT
{
  namespace Environment
  {
    Clock::Clock()
      : gamePerReal(6)
    {
    } // end Clock()

    Clock::~Clock()
    {
      // Remove callback.
      if (cb.IsValid())
      {
        engine->RemoveEngineFrameCallback(cb);
      }
    } // end ~Clock()

    bool Clock::Initialize()
    {
      iObjectRegistry* object_reg =
        PointerLibrary::getInstance()->getObjectRegistry();

      engine = csQueryRegistry<iEngine> (object_reg);
      if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

      cbInitTime.AttachNew(new Events::EventHandler<Clock>(
        &Clock::InitTime, this));
      PointerLibrary::getInstance()->getEventManager()->AddListener(
        "environment.inittime", cbInitTime);

      cbUpdateTime.AttachNew(new Events::EventHandler<Clock>(
        &Clock::UpdateTime, this));
      PointerLibrary::getInstance()->getEventManager()->AddListener(
        "environment.updatetime", cbUpdateTime);

      // Update our manager each frame.
      cb.AttachNew(new FrameCallBack(this));
      engine->AddEngineFrameCallback(cb);

      timer.Initialize();

      return true;
    } // end Initialize()

    bool Clock::InitTime(iEvent& ev)
    {
      ChangeCalendar(PT::Events::EnvironmentHelper::GetCalendar(&ev));

      gamePerReal = PT::Events::EnvironmentHelper::GetGamePerReal(&ev);

      UpdateTime(ev);

      return true;
    } // end InitTime()

    bool Clock::UpdateTime(iEvent& ev)
    {
      // Update the time from the event.
      SetDate(PT::Events::EnvironmentHelper::GetSeconds(&ev));

      // Reset the client timer.
      timer.Initialize();

      return true;
    } // end UpdateTime()

    void Clock::ClientTick()
    {
      // Get the elapsed milliseconds since last clock tick.
      size_t elapsed = timer.GetElapsedMS();

      size_t realMSPerGame = (1000 / gamePerReal);

      // Return if elapsed time is less than equivalent to a game minute.
      if (elapsed < realMSPerGame) return;

      // Increment the clock counters.
      Tick(elapsed / realMSPerGame);

      // Reset the minute timer offset by the remaining elapsed time.
      timer.Initialize(-1 * static_cast<time_t>(elapsed % realMSPerGame));
    } // end Tick()

    void Clock::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
    {
      if (!clock)
      {
        engine->RemoveEngineFrameCallback(this);
      }
      else
      {
        clock->ClientTick();
      }

    } // end StartFrame()

  } // Environment namespace
} // PT namespace
