#include <stdio.h>

#include "common/events/engine.h"
#include "dummyhandler.h"
#include "common/util/wincrashdump.h"
#include "common/util/printhelper.h"

int main(int argc, char ** argv)
{
  setWinCrashDump(argv[0]);

  // Create EventSystem
  EventEngine eng;

  DummyHandler dh;

  // Register Event
  eng.registerHandler(&dh, Events::Dummy);

  for (size_t i = 0; i<10; i++)
  {
    eng.addWorker();
  }
  eng.start();

  for (size_t i = 0; i<100000; i++)
  {
    Event* ev = eng.getFreeEvent();
    ev->type = Events::Dummy;
    ev->num1 = i;
    eng.addEvent(ev);

    printf("Adding new event, total events in queue: %" SIZET "\n", eng.getEventCount());
  }

  while (eng.getEventCount())
  {
    printf("Remaining events in queue: %" SIZET "\n", eng.getEventCount());
    pt_sleep(1);
  }

  eng.stop();

  return 0;
}

