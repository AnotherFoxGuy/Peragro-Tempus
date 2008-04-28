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
#include <iutil/virtclk.h>

namespace PT
{

  Clock::Clock()
  {
    minute = 0;
    hour = 0;
    // Set up some likely defaults.
    minutesPerHour = 60;
    hoursPerDay = 24;
    realPerGame = 60000;
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

    vc = csQueryRegistry<iVirtualClock> (object_reg);
    if (!vc) return Report(PT::Error, "Failed to locate Virtual Clock!");

    cbInitTime.AttachNew(new Events::EventHandler<Clock>(&Clock::InitTime, this));
    PointerLibrary::getInstance()->getEventManager()->AddListener("environment.inittime", cbInitTime);

    cbUpdateTime.AttachNew(new Events::EventHandler<Clock>(&Clock::UpdateTime, this));
    PointerLibrary::getInstance()->getEventManager()->AddListener("environment.updatetime", cbUpdateTime);

    // Update our manager each frame.
    cb.AttachNew(new FrameCallBack(this));
    engine->AddEngineFrameCallback(cb);

    return true;
  } // end Initialize()

  bool Clock::InitTime(iEvent& ev)
  {
    minutesPerHour = PT::Events::EnvironmentHelper::GetMinutesPerHour(&ev);
    hoursPerDay = PT::Events::EnvironmentHelper::GetHoursPerDay(&ev);

    size_t tenthsOfSeconds = PT::Events::EnvironmentHelper::GetRealPerGame(&ev);
    // Convert from tenths of seconds to milliseconds.
    realPerGame = tenthsOfSeconds * 100;

    UpdateTime(ev);

    return true;
  } // end InitTime()

  bool Clock::UpdateTime(iEvent& ev)
  {
    hour = PT::Events::EnvironmentHelper::GetTimeHour(&ev);
    minute = PT::Events::EnvironmentHelper::GetTimeMinute(&ev);

    return true;
  } // end UpdateTime()

  void Clock::Tick()
  {
    static csTicks counter = 0;
    counter += vc->GetElapsedTicks();
    if (counter < realPerGame) return;

    while (counter >= realPerGame)
    {
      counter -= realPerGame;
      minute++;
    }
    while (minute >= minutesPerHour)
    {
      minute -= minutesPerHour;
      hour++;
      Report(PT::Debug, "%d o'clock.", hour, minute);
    }
    while (hour >= hoursPerDay)
    {
      hour -= hoursPerDay;
      Report(PT::Debug, "A new day dawns...");
    }
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
    float step = static_cast<float>(GetMinuteOfDay()) /
      static_cast<float>(GetMinutesPerDay());
    return step;
  } // end GetTimeDecimal()

}
