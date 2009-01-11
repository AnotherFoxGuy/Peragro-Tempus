#include <stdio.h>

#include "dummyhandler.h"

#include "common/events/event.h"
#include "common/util/printhelper.h"

void DummyHandler::handle(Event* ev)
{
  size_t j=0;
  for (size_t i = 0; i < 10000; i++)
  {
    j += (j * i) ^ i;
  }
  printf("Handle event %"SIZET" by %"SIZET": %"SIZET"\n", ev->num1, ev->num2, j);
}
