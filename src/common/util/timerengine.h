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
 * @file timerengine.h
 *
 * @basic The timer engine.
 */

#ifndef TIMERENGINE_H
#define TIMERENGINE_H

#include "array.h"
#include "mutex.h"
#include "thread.h"
#include "pttime.h"

class Timer;

/**
 * The timer engine. This keeps track of all timer objects, and calls the tick
 * method of each of them at the specified interval.
 */
class TimerEngine : public Thread
{
private:
  /// The array of timers.
  Array<Timer*> timers;

  /// The big loop.
  void Run();

  /// The mutex locked when ticking all the timers.
  Mutex mutex;

  /// Stores the start time.
  PTTime start;
  /// Counter of total ticks since start.
  size_t counter;
  /// Maximum ticks before variables could overflow.
  size_t counter_max;

  /// Pointer to self.
  static TimerEngine* self;

public:
  /// Constructor.
  TimerEngine();
  /// Destructor.
  ~TimerEngine() {}

  /**
   * Register a timer to this engine.
   * @param timer The timer.
   */
  void registerTimer(Timer* timer) 
  { 
    mutex.lock(); 
    timers.add(timer); 
    mutex.unlock(); 
  }

 /**
   * Unregister a timer from this engine.
   * @param timer The timer.
   */
  void unregisterTimer(Timer* timer) 
  { 
    mutex.lock(); 
    size_t pos = timers.find(timer);
    if (pos < timers.getCount())
      timers.remove(pos);
    mutex.unlock(); 
  }

  /// Get self.
  static TimerEngine* getTimerEngine() { return self; }
};

#endif // TIMERENGINE_H
