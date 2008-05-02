#include "sleep.h"
#include "timerengine.h"
#include "timer.h"

//#define TIMERENGINE_DEBUG true
#define INTERVAL_MS 100

TimerEngine* TimerEngine::self;

TimerEngine::TimerEngine()
{
  self = this;
  last.Initialize();
}

void TimerEngine::Run()
{
  // Time since last run.
  time_t elapsed = last.GetElapsedMS();

  while (elapsed < INTERVAL_MS)
  {
#ifdef TIMERENGINE_DEBUG
    if (elapsed > 0)
      printf("elapsed: %ld ms, sleeping: %ld ms\n", elapsed, INTERVAL_MS - elapsed);
#endif

    pt_sleep(INTERVAL_MS - elapsed);

    elapsed = last.GetElapsedMS();
  }
  // Check if we slept too long.
  time_t offset = INTERVAL_MS - elapsed;  

#ifdef TIMERENGINE_DEBUG
  if (offset != 0)
    printf("overslept: %ld ms\n", -1*offset);
#endif

  // Set last to the current time, offset by the time overslept.
  last.Initialize(offset);

  mutex.lock();
  for (size_t i=0; i<timers.getCount(); i++)
  {
    timers.get(i)->tick();
  }
  mutex.unlock();
}
