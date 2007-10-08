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
    class ItemEntity : public Entity
    {
    private:
      unsigned int itemid;

      void Create();

    public:
      ItemEntity(const Events::EntityAddEvent& ev);

      int GetItemId () const { return itemid; }
      void SetItemId (unsigned int value) { itemid = value; }

      void Interact();
    };
  }
}

#endif // PTITEMENTITY_H
