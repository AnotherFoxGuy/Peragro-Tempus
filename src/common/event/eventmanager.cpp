/*
  Copyright (C) 2009 Development Team of Peragro Tempus

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

#include <boost/thread.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "common/event/eventdata.h"

namespace PT
{
  namespace Event
  {
    EventManager::EventManager()
    {
    }

    EventManager::~EventManager()
    {
    }

    void EventManager::SendEvents()
    {
      EventDataPtr event;
      WaitForEvent(event);

      do
      {
        try
        {
          SendEvent(event);
        }
        catch (boost::thread_interrupted& ex)
        {
          throw;
        }
        catch (...)
        {
          std::cerr << "Failed sending event '" << event->GetId() << std::endl
            << boost::current_exception_diagnostic_information();
        }
      } while (GetQueuedEvent(event));
    }

    void EventManager::Post(const EventDataPtr& event)
    {
      if (!event) return;

      LockType lock(readyEventsMutex);
      readyEvents.push_back(event);
      eventReady.notify_one();
    }

    Connection EventManager::AddHandlerImpl(const Detail::EventHandlerId& id,
      const Slot& slot)
    {
      LockType lock(handlerMutex);
      Detail::EventHandlerMap::const_iterator itr(handlers.find(id));
      if (itr == handlers.end())
      {
        itr = handlers.insert(id).first;
      }
      return itr->signal->connect(slot);
    }

    Connection EventManager::AddHandler(const EventId& eventId,
      const Slot& slot)
    {
      Detail::EventHandlerId id(eventId);
      return AddHandlerImpl(id, slot);
    }

    Connection EventManager::AddHandler(const EventId& eventId,
      const Entity::EntityId& entityId, const Slot& slot)
    {
      Detail::EventHandlerId id(eventId, entityId);
      return AddHandlerImpl(id, slot);
    }

    void EventManager::WaitForEvent(EventDataPtr& event)
    {
      LockType lock(readyEventsMutex);
      while (readyEvents.empty()) eventReady.wait(lock);

      event = readyEvents.front();
      readyEvents.pop_front();
    }

    bool EventManager::GetQueuedEvent(EventDataPtr& event)
    {
      LockType lock(readyEventsMutex);
      if (readyEvents.empty()) return false;

      event = readyEvents.front();
      readyEvents.pop_front();
      return true;
    }

    void EventManager::SendEvent(const EventDataPtr& event)
    {
      LockType lock(handlerMutex);

      Detail::EventHandlerId id(event->GetId(), event->GetEntityId());
      Detail::EventHandlerMap::const_iterator itr(handlers.find(id));
      if (itr == handlers.end()) return;

      const Signal& signal = (*itr->signal);
      lock.unlock();

      signal(*event);
    }

  }
}

