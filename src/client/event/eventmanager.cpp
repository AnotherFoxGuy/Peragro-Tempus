/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "eventmanager.h"

#include "entityevent.h"

#include "common/util/mutex.h"

namespace PT
{
  namespace Events
  {
    EventManager::EventManager()
    {
    }

    EventManager::~EventManager()
    {
    }

    bool EventManager::TestHandler(Event* ev)
    {
      printf("It worked\n");
      return true;
    }

    bool EventManager::Initialize()
    {
      EventHandler<EventManager>* cb = new EventHandler<EventManager>(&EventManager::TestHandler, this);

      AddListener(1, cb);

      Event* ev = new EntityEvent();

      AddEvent(ev);

      return true;
    }

    void EventManager::AddEvent(Event* ev)
    {
      if (ev) 
        printf("I: Adding event: %d\n", ev->GetEventID());
      else
        printf("I: Adding empty event.\n");

      mutex.lock();
      events.push(ev);
      mutex.unlock();
    }

    void EventManager::AddListener(EventID eventId, EventHandlerCallback* handler)
    {
      printf("I: Adding listener: %d\n", eventId);
      Listener listen;
      listen.handler = handler;
      listen.eventId = eventId;
      listeners.push_back(listen);
    }

    void EventManager::Handle()
    {
      if (events.empty()) return;

      Event* ev = events.front();
      if (!ev) return;
      EventID id = ev->GetEventID();

      std::vector<Listener>::iterator it;
      for(it = listeners.begin(); it != listeners.end(); ++it)
      {
        if (id == it->eventId)
        {
          if (!it->handler) continue;
          printf("I: Handling event: %d\n", it->eventId);
          bool handled = it->handler->HandleEvent(ev);
          if (handled && !ev->GetBroadCast())
          {
            printf("I: Event handled: deleting %d\n", it->eventId);
            //delete ev;
            events.pop();
            return;
          }
        } // if
      } // for

      printf("I: No listeners for event: deleting %d\n", id);
      events.pop();
      //delete ev;
    }

  } // Events namespace 
} // PT namespace 