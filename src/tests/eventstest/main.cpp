#include "common/events/engine.h"
#include "dummyhandler.h"

int main(int argc, char ** argv)
{
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

    printf("Remaining Events: %i\n", eng.getEventCount());
  }

  while(eng.getEventCount())
  {
    printf("Remaining Events: %i\n", eng.getEventCount());
    pt_sleep(1);
  }

  return 0;
}