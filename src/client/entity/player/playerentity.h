/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_ENTITYPLAYERENTITY_H
#define PT_ENTITYPLAYERENTITY_H

#include "client/entity/pc/pcentity.h"

namespace PT
{
  namespace Entity
  {
    /**
     * You get an instance of the class by calling
     * Instance(const Events::EntityAddEvent* ev). To initialize the
     * instance, you must pass a valid non-NULL pointer the first time.
     * @brief Singleton class representing the player himself.
     * @author Branko Majic <branko.majic NO SPAM AT gmail.com>
    */
    class PlayerEntity : public PcEntity
    {
    private:
      PlayerEntity(const Events::EntityAddEvent& ev);

      ~PlayerEntity() {};
      void Create();

    public:
      void Interact(){}

      /**
       * Returns a pointer to an instance of the class. Initiate the instance
       * by passing a non-null valid Events::EntityAddEvent pointer.
       * @param ev Pointer to an EntityAddEvent object. If NULL, existing instance
       *           is returned, otherwise the object is used for initialization of
       *           the class instance, if it wasn't initialized yet.
       * @return NULL if an error occured, or if no instance was created yet.
       *         Otherwise returns a pointer to an instance of the class.
       */
      static PlayerEntity* Instance(const Events::EntityAddEvent* ev=NULL);
    };
  }
}

#endif
