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
#include <csutil/array.h>

#include <vector>

#include "client/event/event.h"

#include <physicallayer/datatype.h>

namespace PT
{
  namespace Events
  {
    /**
    * Entity event base class.
    */
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

    public:
      EntityEvent(EventID name, bool broadCast) : Event(name, broadCast) {}
      virtual ~EntityEvent() {}
    };

    /**
    * EntityEvent helper function.
    */
    template <class T>
    T GetEntityEvent(Eventp ev)
    {
      EntityEvent* entityEv = static_cast<EntityEvent*> (ev.get());
      if (!entityEv)
      {
        printf("E: Not an Entity event!\n");
        return 0;
      }
      T tEv = static_cast<T> (entityEv);
      if (!tEv)
      {
        printf("E: Wasn't listening for this %s event!\n", ev->name.c_str());
        return 0;
      }

      return tEv;
    }

    /**
    * Entity Add event.
    */
    class EntityAddEvent : public EntityEvent
    {
    public:
      unsigned int entityType;
      std::string entityName;
      std::string meshName;
      unsigned int meshId;
      csVector3 position;
      std::string sectorName;

      struct SlotAndItem
      {
        unsigned int slotId;
        unsigned int itemId;
      };

      //std::vector<SlotAndItem> equipment;
      csArray<SlotAndItem> equipment;

      bool locked;
      bool open;

    public:
      EntityAddEvent() : EntityEvent("EntityAddEvent", true) {}
      virtual ~EntityAddEvent() {}
    };

    /**
    * Entity Remove event.
    */
    class EntityRemoveEvent : public EntityEvent
    {
    public:
      EntityRemoveEvent() : EntityEvent("EntityRemoveEvent", true) {}
      virtual ~EntityRemoveEvent() {}
    };

    /**
    * Entity Equip event.
    */
    class EntityEquipEvent : public EntityEvent
    {
    public:
      unsigned char slotId;
      unsigned int itemId;

    public:
      EntityEquipEvent() : EntityEvent("EntityEquipEvent", true) {}
      virtual ~EntityEquipEvent() {}
    };

    /**
    * Entity Mount event.
    */
    class EntityMountEvent : public EntityEvent
    {
    public:
      unsigned int mountId;
      bool control;
      bool mount;

    public:
      EntityMountEvent() : EntityEvent("EntityMountEvent", true) {}
      virtual ~EntityMountEvent() {}
    };

    /*---------------*
    *     Movement
    *----------------*/

    /**
    * Entity Move event.
    */
    class EntityMoveEvent : public EntityEvent
    {
    public:
      float walkDirection;
      float turnDirection;

    public:
      EntityMoveEvent() : EntityEvent("EntityMoveEvent", true) {}
      virtual ~EntityMoveEvent() {}
    };

    /**
    * Entity MoveTo event.
    */
    class EntityMoveToEvent : public EntityEvent
    {
    public:
      float speed;
      csVector3 origin;
      csVector3 destination;

    public:
      EntityMoveToEvent() : EntityEvent("EntityMoveToEvent", true) {}
      virtual ~EntityMoveToEvent() {}
    };

    /**
    * Entity Teleport event.
    */
    class EntityTeleportEvent : public EntityEvent
    {
    public:
      csVector3 position;
      std::string sectorName;

    public:
      EntityTeleportEvent() : EntityEvent("EntityTeleportEvent", true) {}
      virtual ~EntityTeleportEvent() {}
    };

    /**
    * Entity DrUpdate event.
    */
    class EntityDrUpdateEvent : public EntityEvent
    {
    public:
      float rotation;
      csVector3 position;
      std::string sectorName;

    public:
      EntityDrUpdateEvent() : EntityEvent("EntityDrUpdateEvent", true) {}
      virtual ~EntityDrUpdateEvent() {}
    };

    /**
    * Entity PcPropUpdate event.
    */
    class EntityPcPropUpdateEvent : public EntityEvent
    {
    public:
      std::string pcprop;
      celData celdata;

    public:
      EntityPcPropUpdateEvent() : EntityEvent("EntityPcPropUpdateEvent", true) {}
      virtual ~EntityPcPropUpdateEvent() {}
    };

  } // Events namespace 
} // PT namespace 

#endif // PTENTITY_EVENT_H
