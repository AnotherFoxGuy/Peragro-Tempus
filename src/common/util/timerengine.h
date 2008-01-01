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

#ifndef TIMERENGINE_H
#define TIMERENGINE_H

#include "array.h"
#include "mutex.h"
#include "thread.h"

class Timer;

class TimerEngine : public Thread
{
private:
  Array<Timer*> timers;

  void Run();

  Mutex mutex;

  size_t last;

  static TimerEngine* self;

public:
  TimerEngine() : last(0) { self = this; }
  ~TimerEngine() {}

  void registerTimer(Timer* timer) 
  { 
    mutex.lock(); 
    timers.add(timer); 
    mutex.unlock(); 
  }

  void unregisterTimer(Timer* timer) 
  { 
    mutex.lock(); 
    size_t pos = timers.find(timer);
    if (pos < timers.getCount())
      timers.remove(pos);
    mutex.unlock(); 
  }

  static TimerEngine* getTimerEngine() { return self; }
};

#endif // TIMERENGINE_H
