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

#ifndef PTMOUNTENTITY_H
#define PTMOUNTENTITY_H

#include "client/entity/character/characterentity.h"

#include "client/entity/pc/pcentity.h"

#include "client/pointer/pointer.h"

namespace PT
{
  namespace Entity
  {
    /**
     * @ingroup entities
     * Implements mounts. This includes horses, boats, carriages, etc.
     */
    class MountEntity : public CharacterEntity
    {
    private:
      ///Specifies whether the mount is mounted or not.
      bool mounted;

      void Create();

    public:
      /**
       * Constructor that sets up the mount using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const iEvent& ev)
       * @param ev Event used for initialising the door properties.
       */
      MountEntity(const iEvent& ev);
      ~MountEntity(){}

      /**
       * Mount an entity on a mount.
       * @todo Should we allow only for PcEntity instances to mount? Currently
       * we're doing casting to PcEntity anyway.
       * @param player Entity to be mounted.
       */
      void Mount(Entity* player);
      /**
       * Unmount an entity from a mount.
       * @todo Should we allow only for PcEntity instances to unmount? Currently
       * we're doing casting to PcEntity anyway.
       * @param player Entity to be unmounted.
       */
      void UnMount(Entity* player);

      void Interact();

      /**
       * @return True if a mount is mounted by someone, false if it isn't.
       */
      bool isMounted() const { return mounted; }
    };
  }
}
#endif // PTMOUNTENTITY_H
