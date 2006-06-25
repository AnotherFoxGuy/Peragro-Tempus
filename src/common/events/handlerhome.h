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

#ifndef _EVENTS_HANDLERHOME_
#define _EVENTS_HANDLERHOME_

#include "common/util/array.h"
#include "eventtype.h"
#include "handler.h"

/**
 * If you are looking for an EventHandler, check here where they live.
 * This class knows all instanced EventHandlers.
 */
class EventHandlerHome
{
private:
  // array of all EventHandler instances
  Array<EventHandler*> eventhandlers[Events::Count];

public:
  void registerHandler(EventHandler* handler, Events::Type type) { eventhandlers[type].add(handler); }
  size_t getHandlerCount(Events::Type type) { return eventhandlers[type].getCount(); }
  EventHandler* getHandler(Events::Type type, size_t idx) { return eventhandlers[type].get(idx); }

  void replicate(EventHandlerHome* home)
  {
    for (size_t i=0; i<Events::Count; i++)
    {
      for (size_t j=0; j<home->eventhandlers[i].getCount(); j++)
      {
        eventhandlers[i].add(home->eventhandlers[i].get(j));
      }
    }
  }
};

#endif // _EVENTS_HANDLERHOME_


