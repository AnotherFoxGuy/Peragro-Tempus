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

#include "common/event/eventdata.h"

#include <iostream>
#include <boost/io/ios_state.hpp>

namespace PT
{
  namespace Event
  {
    EventData::EventData(const EventId& id)
      : id(id), entityId()
    {
    }

    EventData::EventData(const EventId& id, const Entity::EntityId& entityId)
      : id(id), entityId(entityId)
    {
    }

    EventData::EventData(const EventData& other)
      : id(other.id), entityId(other.entityId), attributes(other.attributes)
    {
    }

    EventData& EventData::operator=(const EventData& other)
    {
      id = other.id;
      entityId = other.entityId;
      attributes = other.attributes;
      return *this;
    }

    const EventData::Attribute* EventData::Find(const EventAttrId& name) const
    {
      ConstIterator attr(attributes.find(name));
      if (attr == attributes.end())
      {
        return 0;
      }
      return &(attr->second);
    }

    namespace
    {
      struct EventDataPrinter : public boost::static_visitor<>
      {
        EventDataPrinter(std::ostream& os) : os(os) {}
        void operator()(const EventAttrId& name) { os << " " << name; }
        template<typename T> void operator()(const T& t) { os << ":" << t; }
        std::ostream& os;
      };
    }

    std::ostream& operator<<(std::ostream& os, const EventData& event)
    {
      boost::io::ios_flags_saver saver(os);
      os.setf(std::ios_base::boolalpha);
      os << event.GetId();
      if (event.IsEntitySpecific()) os << " EntityId:" << event.GetEntityId();

      event.Apply(EventDataPrinter(os));

      return os;
    }

  }
}

