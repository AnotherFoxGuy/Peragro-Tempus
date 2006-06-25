#include "common/util/sleep.h"
#include "common/events/handler.h"

class DummyHandler : public EventHandler
{
  void handle(Event* ev);
};
