/*
    Copyright (C) 2007 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PT_ENTITYTELEPORT_H
#define PT_ENTITYTELEPORT_H

#include "client/entity/entity.h"

namespace PT
{
  namespace Entity
  {
    /**
     @author Branko Majic <branko.majic@gmail.com>
    */
    class TeleportEntity : public Entity
    {
    private:
      ///Unique ID of the teleport.
      unsigned int teleportId;
      ///Destination coordinates for entity using the teleport.
      csVector3 destinationPosition;
      ///Destination sector for entity using the teleport.
      std::string destinationSector;

      void Create();

    public:
      /**
       * Constructor that sets up the teleport using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the teleport properties.
       */
      TeleportEntity(const Events::EntityAddEvent& ev);

      void Interact();

      ///Set the teleport's unique ID.
      void SetTeleportId(unsigned int value) { teleportId = value; }
      ///@return Teleport unique ID.
      unsigned int GetTeleportId() const { return teleportId; }

      ///Set the destination position.
      void SetDestinationPosition(const csVector3& value)
        { destinationPosition = value; }
      ///@return Destination position.
      const csVector3& GetDestinationPosition() const
        { return destinationPosition; }

      ///Set the destination sector name.
      void SetDestinationSector(const std::string& value)
        { destinationSector = value; }
      ///@return Destination sector name.
      const std::string& GetDestinationSector() const
        { return destinationSector; }
    };
  }
}

#endif
