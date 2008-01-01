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

#ifndef EVENTS_WORKER
#define EVENTS_WORKER

#include "common/util/thread.h"
#include "handlerhome.h"

class EventEngine;

/**
 * The worker is an active object (that means it runs its own thread) which
 * gets the events from the EventQueue and processes it. There can be multiple
 * instances of this class, each with its own thread.
 */
class EventWorker : public Thread
{
private:
  EventEngine* engine;
  EventHandlerHome handlers;

  size_t id, count;

  void Run();

public:

  EventWorker(EventEngine* engine, size_t id);

  void replicateHandlers(EventHandlerHome* home);

};

#endif // EVENTS_WORKER
