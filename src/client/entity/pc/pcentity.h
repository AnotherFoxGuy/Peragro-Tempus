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

#ifndef PTPCENTITY_H
#define PTPCENTITY_H

#include "client/entity/character/characterentity.h"

namespace PT
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Implements player character entity.
     */
    class PcEntity : public CharacterEntity
    {
    protected:
      /**
       * Conveniance constructor used for PlayerEntity class.
       */
      PcEntity() {}
      virtual void Create();

    public:
      /**
       * Constructor that sets up the PC using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the PC properties.
       */
      PcEntity(const Events::EntityAddEvent& ev);

      void Interact();
    };
  }
}

#endif // PTPCENTITY_H
