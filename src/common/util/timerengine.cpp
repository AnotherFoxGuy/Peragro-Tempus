#include "sleep.h"
#include "timerengine.h"
#include "timer.h"

#define TIME_T_MAX static_cast<size_t>((((time_t)1 << ((sizeof(time_t) * 8) - 2)) * 2) - 1)
//#define TIMERENGINE_DEBUG true
#define INTERVAL_MS 100

TimerEngine* TimerEngine::self;

TimerEngine::TimerEngine()
{
  self = this;
  start.Initialize();
  counter = 0;

  // The maximum number of timer ticks that can be done before a time_t storing
  // the milliseconds elapsed would overflow, one minute later.
  counter_max = (TIME_T_MAX / INTERVAL_MS) - (60000 / INTERVAL_MS) - 2;
}

void TimerEngine::Run()
{
  // Total time it should be since counter start.
  time_t offset = counter * INTERVAL_MS;
  // Time since when last tick should have been.
  time_t elapsed = start.GetElapsedMS() - offset;

  while (elapsed < INTERVAL_MS)
  {
#ifdef TIMERENGINE_DEBUG
    printf("TimerEngine: elapsed: %ld ms, sleeping: %ld ms, offset: %ld ms\n",
      elapsed, INTERVAL_MS - elapsed, offset);
#endif
    // Try sleep until the time to tick.
    pt_sleep(INTERVAL_MS - elapsed);
    // Just in case the sleep was not long enough.
    elapsed = start.GetElapsedMS() - offset;
  }

  if (counter < counter_max)
  {
    ++counter;
  }
  else
  {
    // Reset the timer before it can overflow. In practice this is about 20
    // days on a system with a 4 byte time_t, millions of years with 8 bytes.
    counter = 0;
    start.Initialize();
  }

  mutex.lock();
  for (size_t i=0; i<timers.getCount(); i++)
  {
    timers.get(i)->tick();
  }
  mutex.unlock();
}
