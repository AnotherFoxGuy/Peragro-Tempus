#include <stdio.h>

#include "dummyhandler.h"

#include "common/events/event.h"

void DummyHandler::handle(Event* ev)
{
  size_t j=0;
  for (size_t i = 0; i < 10000; i++)
  {
    j += (j * i) ^ i;
  }
  printf("Handle event %d by %d: %d\n", ev->num1, ev->num2, j);
}
