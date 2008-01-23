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

#ifndef PTITEMENTITY_H
#define PTITEMENTITY_H

#include "client/entity/entity.h"

namespace PT
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Implements basic functionality for all items.
     * @todo At this time we only have this class for representing items.
     * Possibly to change in future.
     */
    class ItemEntity : public Entity
    {
    private:
      ///Represents the ID of item type. Not to be confused with entity ID.
      ///@todo deprecate since we don't have to do a lookup and recieve all 
      ///data from the server.
      unsigned int itemId;

      void Create();

    public:
      /**
       * Constructor that sets up the item using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the door properties.
       */
      ItemEntity(const Events::EntityAddEvent& ev);

      ///@return Item type ID.
      int GetItemId () const { return itemId; }
      ///Sets the item's type ID to given value.
      void SetItemId (unsigned int value) { itemId = value; }

      void Interact();
    };
  }
}

#endif // PTITEMENTITY_H
