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
/**
 * @file ieventmanager.h
 *
 * @basic Event manager interface.
 */

#ifndef PT_EVENT_IEVENTMANAGER_H
#define PT_EVENT_IEVENTMANAGER_H

#include "common/event/signal.h"
#include "common/event/eventdata_fwd.h"
#include "common/event/eventid.h"
#include "common/entity/entityid.h"

namespace PT
{
  namespace Event
  {
    /**
     * Holds a queue of events and a map of event ids to event handler
     *   functions, manages sending events after they are posted to the queue.
     */
    class iEventManager
    {
    public:
      /// Virtual destructor.
      virtual ~iEventManager() {}

      /**
       * Wait for an event in the queue, then send events until the queue is
       *   empty; suitable to be called by a dedicated thread.
       */
      virtual void SendEvents() = 0;

      /**
       * Post an event to the event queue, for the event thread(s) to send.
       * @param event The event data.
       */
      virtual void Post(const EventDataPtr& event) = 0;

      /**
       * Add handler function for an event.
       * @param eventId The event id to handle; you can pass std::string or
       *   const char[] which will be converted to StringId.
       * @param slot Function or functor to call; you will probably want to use
       *   boost::bind.
       * @return Connection object which can be stored to disconnect the
       *   handler later.
       */
      virtual Connection AddHandler(const EventId& eventId,
        const Slot& slot) = 0;

      /**
       * Add handler function for an event, for a specific entity.
       * @param eventId The event id to handle; you can pass std::string or
       *   const char[] which will be converted to StringId.
       * @param entityId The id of the entity this event should be handled by.
       * @param slot Function or functor to call; you will probably want to use
       *   boost::bind.
       * @return Connection object which can be stored to disconnect the
       *   handler later.
       */
      virtual Connection AddHandler(const EventId& eventId,
        const Entity::EntityId& entityId, const Slot& slot) = 0;

    };

  } // Event namespace
} // PT namespace

#endif
