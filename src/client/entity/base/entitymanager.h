/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#ifndef PT_BASE_ENTITYMANAGER_H
#define PT_BASE_ENTITYMANAGER_H

#include "common/entity/entitymanager.h"

struct iCelEntity;

namespace PT
{
  namespace Entity
  {
    class EntityManager : public Common::Entity::EntityManager
    {
    protected:

    public:
      EntityManager() : Common::Entity::EntityManager() {}
      virtual ~EntityManager() {}

      iCelEntity* findCelEntById(unsigned int id);
    };

  } // namespace Entity
} // namespace PT

#endif // PT_BASE_ENTITYMANAGER_H
