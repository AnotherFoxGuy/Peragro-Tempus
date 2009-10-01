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

#ifndef PT_EVENT_EVENTMANAGER_H
#define PT_EVENT_EVENTMANAGER_H

#include "common/event/ieventmanager.h"

#include <deque>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

#include "eventhandlermap.h"

namespace PT
{
  namespace Event
  {
    class EventManager : public iEventManager
    {
    public:
      EventManager();
      virtual ~EventManager();

      virtual void SendEvents();

      virtual void Post(const EventDataPtr& event);

      virtual Connection AddHandler(const EventId& eventId, const Slot& slot);
      virtual Connection AddHandler(const EventId& eventId,
        const Entity::EntityId& entityId, const Slot& slot);

    private:
      typedef boost::mutex MutexType;
      typedef boost::unique_lock<MutexType> LockType;

      Connection AddHandlerImpl(const Detail::EventHandlerId& id,
        const Slot& slot);

      void WaitForEvent(EventDataPtr& event);
      bool GetQueuedEvent(EventDataPtr& event);
      void SendEvent(const EventDataPtr& event);

      MutexType handlerMutex;
      Detail::EventHandlerMap handlers;

      MutexType readyEventsMutex;
      boost::condition eventReady;
      std::deque<EventDataPtr> readyEvents;
    };

  }
}

#endif
