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

#include "client/event/event.h"
#include "common/network/entitymessages.h"

namespace PT
{
  namespace Events
  {
    class EntityEvent : public Event
    {
    private:
      ENTITY::MESSAGES type;

    public:
      unsigned int entityId;

      // Add
      std::string entityName;
      std::string meshName;
      float* position;
      std::string sectorName;

      // Move
      float walk;
      float turn;

      // DR Update
      float rotation;

      // MoveTo
      float* fromPosition;
      float* toPosition;
      float speed;

      // Add player
      std::vector<int> equipmentItemIDs;

      // Mount
      unsigned int mount_id;
      unsigned char control;

    public:
      EntityEvent() : Event(1, false) {}
      virtual ~EntityEvent() {}

      ENTITY::MESSAGES GetType() { return type; }
      void SetType(ENTITY::MESSAGES type) { this->type = type; }

    };

  } // Events namespace 
} // PT namespace 

#endif // PTENTITY_EVENT_H
