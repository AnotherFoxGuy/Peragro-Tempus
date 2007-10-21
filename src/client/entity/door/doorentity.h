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

#ifndef PTDOORENTITY_H
#define PTDOORENTITY_H

#include "client/entity/entity.h"

#include "client/pointer/pointer.h"

namespace PT
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Implements doors and door-like objects, such as gates etc.
     */
    class DoorEntity : public Entity
    {
    private:
      ///Unique ID of the door.
      unsigned int doorId;
      ///Specifies whether the door is open or closed.
      bool open;
      ///Specifies whether the door is locked or not.
      bool locked;

      void Create();

    public:
      /**
       * Constructor that sets up the door using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the door properties.
       */
      DoorEntity(const Events::EntityAddEvent& ev);

      ///@return Unique ID of the door.
      unsigned int GetDoorId() const { return doorId; }
      ///Set the door's unique ID.
      ///@todo Should we really be allowed to do this?
      void SetDoorId(unsigned int value) { doorId = value; }

      ///@return State of door - open (true) or closed (false).
      bool GetOpen() const { return open; }
      ///Set the door's state to a given. True for open, false for closed.
      void SetOpen(bool value) { open = value; }

      ///@return Door's lock state - locked (true) or unlocked (false).
      bool GetLocked() const { return locked; }
      ///Set the door's lock status to given value.
      void SetLocked(bool value) { locked = value; }

      void UpdatePcProp(const UpdatePcPropData& update_pcprop);
      void Interact();
    };
  }
}

#endif // PTDOORENTITY_H
