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

#include <csutil/csevent.h>

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

namespace PT
{
  namespace Events
  {
    EventManager::EventManager() : scfImplementationType (this)
    {
    }

    EventManager::~EventManager()
    {
    }

    bool EventManager::Initialize()
    {
      iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      if (!obj_reg) return false;

      eventQueue = csQueryRegistry<iEventQueue> (obj_reg);
      if (!eventQueue) return false;

      nameRegistry = csEventNameRegistry::GetRegistry(obj_reg);
      if (!eventQueue) return false;

      eventQueue->RegisterListener(this);

      return true;
    }

    csRef<iEvent> EventManager::CreateEvent(csEventID eventId, bool fromNetwork)
    {
      if (!fromNetwork)
        return eventQueue->CreateEvent(eventId);
      else
        return csPtr<iEvent>(new csEvent(0, eventId, true));
    }

    void EventManager::AddEvent(iEvent* ev)
    {
      mutex.lock();

      events.Push(ev);

      mutex.unlock();
    } // end AddEvent()

    void EventManager::AddListener(csEventID eventId, EventHandlerCallback* handler)
    {
      Report(PT::Debug, "Adding event listener: %s", Retrieve(eventId));
      eventQueue->Subscribe(this, eventId);
      Listener listen;
      listen.handler = boost::shared_ptr<EventHandlerCallback>(handler);
      listen.eventId = eventId;
      listeners.push_back(listen);
    } // end AddListener()

    void EventManager::AddListener(const std::string& eventId, EventHandlerCallback* handler)
    {
      AddListener(nameRegistry->GetID(eventId.c_str()), handler);
    }

    void EventManager::Handle()
    {
      mutex.lock();
      while (!events.IsEmpty())
      {
        csRef<iEvent> ev = events.Get(0);
        eventQueue->Post(ev);
        events.DeleteIndex(0);
      }
      mutex.unlock();
    } // end Handle()

    bool EventManager::HandleEvent(iEvent& ev)
    {
      csEventID id = ev.GetName();

      std::vector<Listener>::iterator it;
      for(it = listeners.begin(); it != listeners.end(); ++it)
      {
        //Report(PT::Debug, "Handling event: %s for listener %s", Retrieve(id), Retrieve(it->eventId));
        if (id == it->eventId)
        {
          Report(PT::Debug, "Handling event: %s", Retrieve(it->eventId));
          it->handler->HandleEvent(ev);
        } // if
      } // for

      return false;

  } // end HandleEvent()

  } // Events namespace 
} // PT namespace 

