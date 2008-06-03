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
#include "common/event/event.h"

#include <iutil/cfgmgr.h>
#include <iutil/cfgfile.h>

#include <propclass/defcam.h>

namespace PT
{
  namespace Events
  {
    struct EventHandlerCallback;
  }
  namespace Entity
  {
    /**
     * @ingroup entities
     * Singleton class representing the player himself.. You get an instance of
     * the class by calling Instance(const Events::EntityAddEvent* ev). To
     * initialize the instance, you must pass a valid non-NULL pointer the first
     * time. Every subsequent pass of valid non-NULL pointer will reinitialize
     * the class. This may be used when relogging or doing a login/logout.
     * @author Branko Majic <branko.majic NO SPAM AT gmail.com>
    */
    class PlayerEntity : public PcEntity
    {
    public:
      void Interact();

      /**
       * Returns a pointer to an instance of the class. Initiate the instance
       * by passing a non-null valid Events::EntityAddEvent pointer.
       * @param ev Pointer to an EntityAddEvent object. If NULL, existing
       * instance is returned, otherwise the object is used for initialisation
       * of the new class instance, while deleting the old instance as needed.
       * @return 0 if an error occured, or if no instance was created yet.
       * Otherwise returns a pointer to an instance of the class.
       */
      static PlayerEntity* Instance(const iEvent* ev = 0);

      /**
       * @return Returns the player entity's camera.
       */
      iPcDefaultCamera* GetCamera() { return camera; }

      /**
       * @return Returns the player entity's current sector.
       */
      iSector* GetSector();

      /**
       * Draws player's camera view.
       */
      void CameraDraw();

      /**
       * Changes the entity position and sector immediatelly
       * and activates the region for it to load.
       * @param pos New position of an entity.
       * @param sector New sector where the entity should reside.
       */
      void Teleport(const csVector3& pos, float rotation, const std::string& sector);

      /**
       * Changes the entity position and sector immediatelly.
       * This also updates the camera.
       * @param pos New position of an entity.
       * @param sector New sector where the entity should reside.
       */
      void SetFullPosition(const csVector3& pos,
                           float rotation,
                           const std::string& sector);

    private:
      static PlayerEntity* instance;

      void ReInit(const iEvent& ev);

      ///Player entity's camera.
      csWeakRef<iPcDefaultCamera> camera;

      csRef<iVFS> vfs;

      csRef<iCelEntity> other_self;

      /**
       * Constructor that sets up the player using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the player properties.
       */
      PlayerEntity(const iEvent& ev);
      virtual ~PlayerEntity();

      void Create();

    };
  } //Entity namespace
} //PT namespace

#endif
