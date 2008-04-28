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
/**
 * @file clock.h
 *
 * @brief The game clock.
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
  /**
   * The game clock.
   */
  class Clock
  {
  private:
    /// The engine.
    csRef<iEngine> engine;
    /// The crystal space virtual clock.
    csRef<iVirtualClock> vc;

    /// The current game minute.
    size_t minute;
    /// The current game hour.
    size_t hour;
    /// Game time minutes per hour.
    size_t minutesPerHour;
    /// Game time hours per day.
    size_t hoursPerDay;
    /// Milliseconds real time per in game minute.
    csTicks realPerGame;

    /// Callback for the inittime event.
    csRef<Events::EventHandlerCallback> cbInitTime;
    /// Callback for the updatetime event.
    csRef<Events::EventHandlerCallback> cbUpdateTime;

    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      Clock* clock;
      FrameCallBack (Clock* clk) : scfImplementationType (this) { clock = clk; }
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;

    /// Advance the local clock.
    void Tick();
    /// Initialize the time parameters from an event.
    bool InitTime(iEvent& ev);
    /// Update the time from an event.
    bool UpdateTime(iEvent& ev);

  public:
    /// Constructor.
    Clock();
    /// Destructor.
    ~Clock();

    /// Get a number from 0 to 1 representing the time of day.
    float GetTimeDecimal();
    /// Return the game time as minutes since the start of the day.
    size_t GetMinuteOfDay() { return (hour * minutesPerHour + minute); }
    /// Get the number of minutes in a game day.
    size_t GetMinutesPerDay() { return minutesPerHour * hoursPerDay; }
    /// Get the current hour.
    size_t GetHour() { return hour; }
    /// Get the current minute.
    size_t GetMinute() { return minute; }

    /**
     * Initialize the clock
     * @return True, indicating success
     */
    bool Initialize();

  };
}

#endif // CLOCK_H_
