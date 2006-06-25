#include "workerhome.h"
#include "worker.h"
#include "event.h"
#include "engine.h"

EventWorker* EventWorkerHome::addWorker()
{
  EventWorker* wkr = new EventWorker(engine, workers.getCount()+1);
  workers.add(wkr);
  wkr->replicateHandlers(engine->getHandlerHome());
  return wkr;
}
