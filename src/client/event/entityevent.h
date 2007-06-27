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

#ifndef PTENTITY_EVENT_H
#define PTENTITY_EVENT_H

#include <cssysdef.h>
#include <csgeom/vector3.h>

#include <vector>

#include "client/event/event.h"

namespace PT
{
  namespace Events
  {
    class EntityEvent : public Event
    {
    public:
      public:
        enum EntityType
        {
          PlayerEntity=0,
          NPCEntity=1,
          DoorEntity=2,
          ItemEntity=3,
          MountEntity=4
        };

      unsigned int entityId;
      unsigned int entityType;

    public:
      EntityEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~EntityEvent() {}
    };

    class EntityAddEvent : public EntityEvent
    {
    public:
      std::string entityName;
      std::string meshName;
      unsigned int meshId;
      csVector3 position;
      std::string sectorName;

      std::vector<std::pair<int, int> > equipment;

      bool locked;
      bool open;

    public:
      EntityAddEvent() : EntityEvent("EntityAddEvent", false) {}
      virtual ~EntityAddEvent() {}
    };

    class EntityRemoveEvent : public EntityEvent
    {
    public:
      EntityRemoveEvent() : EntityEvent("EntityRemoveEvent", false) {}
      virtual ~EntityRemoveEvent() {}
    };

  } // Events namespace 
} // PT namespace 

#endif // PTENTITY_EVENT_H
