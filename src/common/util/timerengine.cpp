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
