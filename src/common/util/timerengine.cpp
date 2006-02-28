#include <time.h>

#include "sleep.h"
#include "timerengine.h"
#include "timer.h"

void TimerEngine::Run()
{
  pt_sleep(1);

  // !!! This is INACCURATE !!! in several way.
  // Not only does it take longer with more timers,
  // sleep isn't accurate in the first place!
  // Should work for now though...

  mutex.lock();
  for (size_t i=0; i<timers.getCount(); i++)
  {
    timers.get(i)->tick();
  }
  mutex.unlock();
}
