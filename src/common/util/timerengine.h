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

#include <boost/thread/mutex.hpp>

#include "array.h"
#include "pttime.h"

class Timer;

/**
 * The timer engine. This keeps track of all timer objects, and calls the tick
 * method of each of them at the specified interval.
 */
class TimerEngine
{
public:
  /// Constructor.
  TimerEngine();
  /// Destructor.
  ~TimerEngine() {}

  /**
   * Register a timer to this engine.
   * @param timer The timer.
   */
  void registerTimer(Timer* timer);

 /**
   * Unregister a timer from this engine.
   * @param timer The timer.
   */
  void unregisterTimer(Timer* timer);

  /// Get self.
  static TimerEngine* getTimerEngine();

  /// The main loop for this thread.
  void Run();

private:
  /// Pointer to self.
  static TimerEngine* self;

  typedef boost::lock_guard<boost::mutex> LockType;
  /// The mutex locked when ticking all the timers.
  boost::mutex mutex;

  /// Stores the start time.
  PTTime startTime;
  /// Interval in milliseconds representing a timer tick.
  const time_t intervalMS;
  /// Counter of total ticks since start.
  size_t counter;
  /// Maximum ticks before variables could overflow.
  const size_t counterMax;

  /// The array of timers.
  Array<Timer*> timers;
};

inline TimerEngine* TimerEngine::getTimerEngine()
{ return self; }

#endif // TIMERENGINE_H
