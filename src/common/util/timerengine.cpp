/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "timerengine.h"
#include "sleep.h"
#include "timer.h"

#include <limits>

#ifdef max
  #undef max
#endif

TimerEngine* TimerEngine::self = 0;

TimerEngine::TimerEngine()
  : intervalMS(100), counter(0),
  counterMax((std::numeric_limits<time_t>::max() / intervalMS) - 1)
{
  self = this;
}

void TimerEngine::registerTimer(Timer* timer)
{
  mutex.lock();
  timers.add(timer);
  mutex.unlock();
}

void TimerEngine::unregisterTimer(Timer* timer)
{
  mutex.lock();
  size_t pos = timers.find(timer);
  if (pos < timers.getCount())
  {
    timers.remove(pos);
  }
  mutex.unlock();
}

void TimerEngine::Run()
{
  // Total time it should be since counter start.
  const time_t offset = counter * intervalMS;
  // Time since when last tick should have been.
  time_t elapsed = startTime.GetElapsedMS() - offset;

  while (elapsed < intervalMS)
  {
    // Try sleep until the time to tick.
    pt_sleep(intervalMS - elapsed);
    // Just in case the sleep was not long enough.
    elapsed = startTime.GetElapsedMS() - offset;
  }

  if (counter < counterMax)
  {
    ++counter;
  }
  else
  {
    // Reset the timer before it can overflow. In practice this is about 20
    // days on a system with a 4 byte time_t, millions of years with 8 bytes.
    counter = 0;
    startTime.Initialize();
  }

  mutex.lock();
  for (size_t i = 0; i < timers.getCount(); ++i)
  {
    timers.get(i)->tick();
  }
  mutex.unlock();
}
