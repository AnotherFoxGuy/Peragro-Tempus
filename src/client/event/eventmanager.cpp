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

    bool EventManager::Initialize()
    {

      return true;
    }

    void EventManager::AddEvent(Event* ev)
    {
      mutex.lock();

      printf("I: Adding event.\n");

      events.push(ev);

      mutex.unlock();
    }

    void EventManager::AddListener(EventID eventId, EventHandlerCallback* handler)
    {
      printf("I: Adding listener: %s\n", eventId.c_str());
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
          printf("I: Handling event: %s\n", it->GetEventId());
          bool handled = it->handler->HandleEvent(ev);
          if (handled && !ev->GetBroadCast())
          {
            printf("I: Event handled: deleting %s\n", it->GetEventId());
            delete ev;
            events.pop();
            return;
          }
        } // if
      } // for

      printf("W: No listeners for event: deleting %s\n", id.c_str());
      events.pop();
      delete ev;
    }

  } // Events namespace 
} // PT namespace 