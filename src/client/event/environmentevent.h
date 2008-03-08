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

#ifndef PTENVIRONMENT_EVENT_H
#define PTENVIRONMENT_EVENT_H

#include <cssysdef.h>
#include <csgeom/vector3.h>
#include <csutil/array.h>

#include <vector>

#include "client/event/event.h"

#include <physicallayer/datatype.h>

namespace PT
{
  namespace Events
  {
    /**
    * EnvironmentEvent event base class.
    */
    class EnvironmentEvent : public Event
    {
    public:
      EnvironmentEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~EnvironmentEvent() {}
    };

    /**
    * EntityEvent helper function.
    */
    template <class T>
    T GetEnvironmentEvent(Eventp ev)
    {
      EnvironmentEvent* environmentEv = static_cast<EnvironmentEvent*> (ev.get());
      if (!environmentEv)
      {
        printf("E: Not an Entity event!\n");
        return 0;
      }
      T tEv = static_cast<T> (environmentEv);
      if (!tEv)
      {
        printf("E: Wasn't listening for this %s event!\n", ev->name.c_str());
        return 0;
      }

      return tEv;
    }

    /**
    * Set Day Time.
    */
    class EnvironmentDayTimeEvent : public EnvironmentEvent
    {
    public:
      unsigned char hour;

    public:
      EnvironmentDayTimeEvent() : EnvironmentEvent("environment.daytime", true) {}
      virtual ~EnvironmentDayTimeEvent() {}
    };
  } // Events namespace
} // PT namespace

#endif // PTENVIRONMENT_EVENT_H
