#include "workerhome.h"
#include "worker.h"
#include "event.h"
#include "engine.h"

EventWorker* EventWorkerHome::addWorker()
{
  EventWorker* wkr = new EventWorker(engine, workers.size()+1);
  workers.push_back(wkr);
  wkr->replicateHandlers(engine->getHandlerHome());
  return wkr;
}
