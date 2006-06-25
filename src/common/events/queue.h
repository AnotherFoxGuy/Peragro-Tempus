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

#ifndef _EVENTS_QUEUE_
#define _EVENTS_QUEUE_

#include "common/util/queue.h"
#include "event.h"

/**
 * This will be the queue where all new events wait for being processed.
 * On a second step, we can add multiple arrays for different priorities.
 */
class EventQueue
{
private:
  // array of events
  Queue free;
  Queue events;

public:
  // add new event
  void addEvent(Event* e)
  {
    events.push(e);
  }

  // get next event
  Event* getNextEvent()
  {
    return (Event*) events.pop();
  }

  // free event
  void freeEvent(Event* e)
  {
    free.push(e);
  }

  // get next event
  Event* getFreeEvent()
  {
    if (free.getCount() > 0)
      return (Event*) free.pop();
    else
      return new Event();
  }

  size_t getEventCount()
  {
    return events.getCount();
  }
};

#endif // _EVENTS_QUEUE_


