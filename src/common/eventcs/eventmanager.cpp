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

#include "common/pointer/ipointer.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace Events
  {

    EventManager::Listener::Listener(EventManager* evmgr, csEventID eventId, EventHandlerCallback* handler)
      : scfImplementationType (this)
    {
      this->evmgr = evmgr;
      this->eventId = eventId;
      this->handler = handler;

      evmgr->eventQueue->RegisterListener(this, eventId);
    }

    EventManager::Listener::~Listener()
    {
      //evmgr->pointerlib->getReporter()->Report(PT::Debug, "Listener: ~Listener()");
    }

    bool EventManager::Listener::HandleEvent(iEvent& ev)
    {
      if (handler.IsValid())
        return handler->HandleEvent(ev);
      else
      {
        //evmgr->pointerlib->getReporter()->Report(PT::Error, "Listener: handler invalid! (%s)",
        //  evmgr->Retrieve(eventId));
        evmgr->RemoveListener(this);
        return false;
      }
    }

    //--------------------------------------------------------------------

    EventManager::EventManager() : scfImplementationType (this)
    {
    }

    EventManager::~EventManager()
    {
      if (eventQueue)
        eventQueue->RemoveListener(this);
    }

    bool EventManager::Initialize(iPointerLibrary* pl)
    {
      pointerlib = pl;

      iObjectRegistry* object_reg = pointerlib->getObjectRegistry();
      if (!object_reg) return false;

      eventQueue = csQueryRegistry<iEventQueue> (object_reg);
      if (!eventQueue) return false;

      nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
      if (!nameRegistry) return false;

      eventQueue->RegisterListener(this);

      //Register for the Frame event, for Handle().
      eventQueue->RegisterListener (this, Retrieve("crystalspace.frame"));

      //If this is big enough, AddListener called from another thread won't
      //cause the "Memory block has wrong cookie" error
      listeners.SetCapacity(100);

      return true;
    }

    csRef<iEvent> EventManager::CreateEvent(csEventID eventId, bool fromNetwork)
    {
      ///@TODO: Don't make everything broadcast, requires all handlers
      /// to return correct values.
      //if (!fromNetwork)
        return eventQueue->CreateBroadcastEvent(eventId);
      //else
        //return csPtr<iEvent>(new csEvent(0, eventId, true));
    }

    void EventManager::AddEvent(iEvent* ev)
    {
      LockType lock(mutex);
      events.Push(ev);
    } // end AddEvent()

    void EventManager::AddListener(csEventID eventId, EventHandlerCallback* handler)
    {
      //pointerlib->getReporter()->Report(PT::Debug, "Adding event listener: %s", Retrieve(eventId));
      csRef<Listener> listen; listen.AttachNew(new Listener(this, eventId, handler));
      listeners.Push(listen);
    } // end AddListener()

    void EventManager::AddListener(const std::string& eventId, EventHandlerCallback* handler)
    {
      AddListener(Retrieve(eventId.c_str()), handler);
    }

    void EventManager::RemoveListener (Listener* handler)
    {
      eventQueue->RemoveListener(handler);
      listeners.Delete(handler);
    }

    void EventManager::RemoveListener (EventHandlerCallback* handler)
    {
      // Remove our listener.
      for(size_t i = 0; i < listeners.GetSize(); i++)
      {
        if (listeners.Get(i)->handler == handler)
        {
          eventQueue->RemoveListener(listeners.Get(i));
          listeners.DeleteIndex(i);
          // Don't return, remove _all_ occurences of our handler.
        }
      }
    }

    void EventManager::RemoveListener (EventHandlerCallback* handler, csEventID eventId)
    {
      // Remove our listener.
      for(size_t i = 0; i < listeners.GetSize(); i++)
      {
        if (   (listeners.Get(i)->handler == handler)
            && (listeners.Get(i)->eventId == eventId))
        {
          eventQueue->RemoveListener(listeners.Get(i));
          listeners.DeleteIndex(i);
          return;
        }
      }
    }

    void EventManager::Handle()
    {
      LockType lock(mutex);
      while (!events.IsEmpty())
      {
        csRef<iEvent> ev = events.Get(0);
        eventQueue->Post(ev);
        events.DeleteIndex(0);
      }
    } // end Handle()

    bool EventManager::HandleEvent(iEvent& ev)
    {
      //Listen for the Frame event, for Handle().
      if (ev.GetName() == Retrieve("crystalspace.frame"))
      {
        Handle();
      }

      return false;

  } // end HandleEvent()

  } // Events namespace
} // PT namespace

