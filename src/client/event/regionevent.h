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

#ifndef PT_REGION_EVENT_H
#define PT_REGION_EVENT_H

#include <cssysdef.h>

#include "client/event/event.h"

namespace PT
{
  namespace Events
  {
    /**
    * Region event base class.
    */
    class WorldEvent : public Event
    {
    public:
      WorldEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~WorldEvent() {}
    };

    /**
    * RegionEvent helper function.
    */
    template <class T>
    T GetWorldEvent(Eventp ev)
    {
      WorldEvent* worldEv = static_cast<WorldEvent*> (ev.get());
      if (!worldEv)
      {
        printf("E: Not a World event!\n");
        return 0;
      }
      T tEv = static_cast<T> (worldEv);
      if (!tEv)
      {
        printf("E: Wasn't listening for this %s event!\n", ev->name.c_str());
        return 0;
      }

      return tEv;
    }

    /**
    * World loading event.
    */
    class WorldLoadingEvent : public WorldEvent
    {
    public:
      float progress;

    public:
      WorldLoadingEvent() : WorldEvent("world.loading", true), progress (0.0f) {}
      virtual ~WorldLoadingEvent() {}
    };

    /**
    * World loaded event.
    */
    class WorldLoadedEvent : public WorldEvent
    {
    public:
      WorldLoadedEvent() : WorldEvent("world.loaded", true) {}
      virtual ~WorldLoadedEvent() {}
    };

  } // Events namespace 
} // PT namespace 

#endif // PT_REGION_EVENT_H
