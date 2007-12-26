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

#ifndef _TIMER_H_
#define _TIMER_H_

#include "timerengine.h"

class Timer
{
private:
  size_t timer;
  size_t duration;

  bool running;

  inline void tick()
  {
    if (!running)
      return;

    timer--; 

    if (timer != 0)
      return;

    timer = duration;
    timeOut();
  }

  friend class TimerEngine;

protected:
  virtual void timeOut() = 0;

public:
  Timer() : timer(0), duration(0), running(false) { TimerEngine::getTimerEngine()->registerTimer(this); }
  virtual ~Timer() { running = false; TimerEngine::getTimerEngine()->unregisterTimer(this); }

  void start() 
  { 
    if (duration != 0)
      running = true; 
  }

  void stop()
  { 
    running = false;
  }

  void setInterval(int interval)
  {
    duration = interval;
    timer = interval;
  }
};

#endif // _TIMER_H_
