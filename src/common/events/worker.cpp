/*
    Copyright (C) 2006 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "worker.h"
#include "event.h"
#include "engine.h"
#include "handlerhome.h"
#include "common/util/sleep.h"
#include "common/util/printhelper.h"

EventWorker::EventWorker(EventEngine* engine, size_t id) : engine(engine), id(id)
{
  count = 0;
}

void EventWorker::replicateHandlers(EventHandlerHome* home)
{
  handlers.replicate(home);
}

void EventWorker::Run()
{
  Event* ev = engine->getNextEvent();
  if (ev == 0)
  {
    pt_sleep(1);
  }
  else
  {
    count++;
    // handle event
    for (size_t i=0; i<handlers.getHandlerCount(ev->type); i++)
    {
      printf("Worker %" SIZET " handled event %" SIZET " (%" SIZET
             " events total)\n", id, ev->num1, count);
      handlers.getHandler(ev->type, i)->handle(ev);
    }
  }
}
