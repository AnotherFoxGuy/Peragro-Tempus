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
    ticksPerHour = 0;
    timer = 0;
    timeScale = 700;//24*60;
  }

  Clock::~Clock()
  {
    // Remove callback.
    if (cb.IsValid())
    {
      engine->RemoveEngineFrameCallback(cb);
    }
  }

  bool Clock::SetDayTime(iEvent& ev)
  {
    // Calculate the light color.
    unsigned int hour = PT::Events::EnvironmentHelper::GetTimeHour(&ev);
    unsigned int minutes = PT::Events::EnvironmentHelper::GetTimeMinute(&ev);

    timer = (hour*60*60*1000);
    timer += (minutes*60*1000);
    timer += 1;


    /* TODO: Adjust the rate the clock progresses based
     on the interval the time updates are recieved.
    csTicks elapsed = vc->GetCurrentTicks() - ticksPerHour;
    //timeScale = elapsed/1000;

    ticksPerHour = vc->GetCurrentTicks();
    */

    return true;
  } // end SetDayTime()

  bool Clock::Initialize()
  {
    iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();

    engine = csQueryRegistry<iEngine> (object_reg);
    if (!engine) return Report(PT::Error, "Failed to locate 3D engine!");

    vc = csQueryRegistry<iVirtualClock> (object_reg);
    if (!vc) return Report(PT::Error, "Failed to locate Virtual Clock!");

    cbDayTime.AttachNew(new Events::EventHandler<Clock>(&Clock::SetDayTime, this));
    PointerLibrary::getInstance()->getEventManager()->AddListener("environment.daytime", cbDayTime);

    // Update our manager each frame.
    cb.AttachNew(new FrameCallBack(this));
    engine->AddEngineFrameCallback(cb);

    return true;
  } // end Initialize()

  void Clock::Update()
  {
    csTicks ticks = vc->GetElapsedTicks();
    timer += ticks*timeScale;

    if (timer >= GetMilliSecondsADay())
      timer = 1;

  } // end Update()

  void Clock::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
  {
    if (!clock)
    {
      engine->RemoveEngineFrameCallback(this);
    }
    else
    {
      clock->Update();
    }

  } // end StartFrame()

}
