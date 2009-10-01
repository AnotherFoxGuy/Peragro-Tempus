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

#ifndef PT_EVENT_EVENTHANDLERMAP_H
#define PT_EVENT_EVENTHANDLERMAP_H

#include <boost/make_shared.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/operators.hpp>

#include "common/event/eventid.h"

namespace PT
{
  namespace Event
  {
    namespace Detail
    {
      struct EventHandlerId : public boost::totally_ordered<EventHandlerId>
      {
      public:
        typedef Entity::EntityId EntityId;

        EventHandlerId(const EventId& eventId, const EntityId& entityId
          = EntityId()) : eventId(eventId), entityId(entityId) {}
        EventId eventId;
        EntityId entityId;
      };

      inline bool operator==(const EventHandlerId& lhs, const EventHandlerId& rhs)
      { return (lhs.eventId == rhs.eventId) && (lhs.entityId == rhs.entityId); }

      inline bool operator<(const EventHandlerId& lhs, const EventHandlerId& rhs)
      {
        return (lhs.eventId < rhs.eventId) ||
          ((lhs.eventId == rhs.eventId) && (lhs.entityId < rhs.entityId));
      }

      struct EventHandlerEntry
      {
      public:
        EventHandlerEntry(const EventHandlerId& id)
          : id(id), signal(boost::make_shared<Signal>()) {}
        EventHandlerId id;
        boost::shared_ptr<Signal> signal;
      };

      namespace bmi = boost::multi_index;

      typedef bmi::multi_index_container<
        EventHandlerEntry,
        bmi::indexed_by<
          bmi::ordered_unique<
            bmi::member<EventHandlerEntry, EventHandlerId, &EventHandlerEntry::id>
      > > > EventHandlerMap;

    }
  }
}

#endif
