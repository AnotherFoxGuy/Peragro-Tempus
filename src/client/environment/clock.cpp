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
#include "client/event/environmentevent.h"
#include "client/reporter/reporter.h"
#include "client/event/eventmanager.h"

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

  Clock::Clock()
  {
    minute = 0lu;
    hour = 0lu;
    // Set up some likely defaults.
    minutesPerHour = 60lu;
    hoursPerDay = 24lu;
    realPerGame = 60000lu;
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
    iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();

    engine = csQueryRegistry<iEngine> (object_reg);
    if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

    cbInitTime.AttachNew(new Events::EventHandler<Clock>(&Clock::InitTime, this));
    PointerLibrary::getInstance()->getEventManager()->AddListener("environment.inittime", cbInitTime);

    cbUpdateTime.AttachNew(new Events::EventHandler<Clock>(&Clock::UpdateTime, this));
    PointerLibrary::getInstance()->getEventManager()->AddListener("environment.updatetime", cbUpdateTime);

    // Update our manager each frame.
    cb.AttachNew(new FrameCallBack(this));
    engine->AddEngineFrameCallback(cb);

    timer.Initialize();

    return true;
  } // end Initialize()

  bool Clock::InitTime(iEvent& ev)
  {
    if (PT::Events::EnvironmentHelper::GetMinutesPerHour(&ev) == 0)
      Report(PT::Error, "Invalid minutes per hour value recieved!");
    else minutesPerHour = PT::Events::EnvironmentHelper::GetMinutesPerHour(&ev);

    if (PT::Events::EnvironmentHelper::GetHoursPerDay(&ev))
      Report(PT::Error, "Invalid hours per day value recieved!");
    else hoursPerDay = PT::Events::EnvironmentHelper::GetHoursPerDay(&ev);

    size_t tenthsOfSeconds = PT::Events::EnvironmentHelper::GetRealPerGame(&ev);
    // Convert from tenths of seconds to milliseconds.
    if (tenthsOfSeconds == 0) 
      Report(PT::Error, "Invalid real time per game minute value!");
    else realPerGame = tenthsOfSeconds * 100;

    UpdateTime(ev);

    return true;
  } // end InitTime()

  bool Clock::UpdateTime(iEvent& ev)
  {
    // Update the time from the event.
    minute = PT::Events::EnvironmentHelper::GetTimeMinute(&ev);
    hour = PT::Events::EnvironmentHelper::GetTimeHour(&ev);

    // Reset the minute timer to the current time.
    timer.Initialize();

    return true;
  } // end UpdateTime()

  void Clock::Tick()
  {
    // Get the elapsed milliseconds since last clock tick.
    size_t elapsed = timer.GetElapsedMS();

    // Return if elapsed time is less than equivalent to a game minute.
    if (elapsed < realPerGame) return;

    // Increment the clock counters.
    while (elapsed >= realPerGame)
    {
      elapsed -= realPerGame;
      minute++;
    }
    while (minute >= minutesPerHour)
    {
      minute -= minutesPerHour;
      hour++;
    }
    while (hour >= hoursPerDay)
    {
      hour -= hoursPerDay;
    }

    // Reset the minute timer offset by the remaining elapsed time.
    timer.Initialize(-1 * static_cast<time_t>(elapsed));
  } // end Tick()

  void Clock::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
  {
    if (!clock)
    {
      engine->RemoveEngineFrameCallback(this);
    }
    else
    {
      clock->Tick();
    }

  } // end StartFrame()

  float Clock::GetTimeDecimal()
  {
    float step =
      // Minute of the day,
      (static_cast<float>((hour * minutesPerHour) + minute) +
      // plus the fraction of the game minute calculated from elapsed real time,
        (static_cast<float>(timer.GetElapsedMS())
         / static_cast<float>(realPerGame))) /
      // over total minutes in a day.
      static_cast<float>(minutesPerHour * hoursPerDay);
    if (step > 1.0f) step -= 1.0f;

    return step;
  } // end GetTimeDecimal()

}
