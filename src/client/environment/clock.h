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

#ifndef CLOCK_H_
#define CLOCK_H_

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iengine/engine.h>

#include "client/event/eventmanager.h"

struct iEngine;
struct iVirtualClock;
struct iEvent;

namespace PT
{
  class Clock
  {
  private:
    csRef<iEngine> engine;
    csRef<iVirtualClock> vc;

    size_t timeScale;
    csTicks ticksPerHour;

    csTicks timer;

    csRef<Events::EventHandlerCallback> cbDayTime;

    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      Clock* clock;
      FrameCallBack (Clock* clk) : scfImplementationType (this) { clock = clk; }
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;

    void Update();

    bool SetDayTime(iEvent& ev);

  public:
    Clock();
    ~Clock();

    csTicks GetTime() { return timer; }

    float GetMilliSecondsADay() { return 86400000.0f; }//(24*60*60*1000)

   /**
    *Initialize the clock
    *@return True, indicating success
    */
    bool Initialize();

  };
}

#endif // CLOCK_H_
