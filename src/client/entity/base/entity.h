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

#ifndef PT_BASE_ENTITY_H
#define PT_BASE_ENTITY_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/weakref.h>
#include <physicallayer/entity.h>
#include <physicallayer/pl.h>

#include "common/entity/entity.h"

namespace PT
{
  namespace Entity
  {
    /**
    * Extention of the common Entity class to be aware of CEL entities.
    */
    class Entity : public Common::Entity::Entity
    {
      friend class EntityManager;

    protected:
      ///CEL entity of the entity.  See the CEL documentation for more info.
      csWeakRef<iCelEntity> celEntity;
      ///CEL physical layer, so that the entity can clean up after itself
      csRef<iCelPlLayer> pl;

    public:
      Entity() : Common::Entity::Entity(), celEntity(0), pl(0) {}

      virtual ~Entity() 
      { if (pl && celEntity) pl->RemoveEntity(celEntity); }

      ///@return Entity's CEL entity.
      iCelEntity* GetCelEntity () const { return celEntity; }
      ///Make entity use some other CEL entity instead of its own.
      ///@todo Should we really be allowed to do this?
      void SetCelEntity (iCelEntity* value) { celEntity = value; }

    };
  } // Entity namespace
} // PT namespace

#endif // PT_BASE_ENTITY_H
