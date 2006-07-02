#include <time.h>

#include "sleep.h"
#include "timerengine.h"
#include "pttime.h"
#include "timer.h"

TimerEngine* TimerEngine::self;

void TimerEngine::Run()
{
  size_t now = pt_time_ms();

  if (now - last < 100)
    pt_sleep(100 - now + last);

  last = pt_time_ms();

  mutex.lock();
  for (size_t i=0; i<timers.getCount(); i++)
  {
    timers.get(i)->tick();
  }
  mutex.unlock();
}
