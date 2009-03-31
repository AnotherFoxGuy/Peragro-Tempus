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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

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
      virtual ~PlayerEntity();

      void Interact();

      /**
       * Returns a pointer to an instance of the class.
       */
      static boost::shared_ptr<PlayerEntity> Instance();

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
      void Teleport(const WFMath::Point<3>& pos, float rotation, const std::string& sector);

    private:
      static PlayerEntity* instance;

      ///Player entity's camera.
      csWeakRef<iPcDefaultCamera> camera;

      /**
       * Constructor that sets up the player.
       */
      PlayerEntity();

      protected:
        virtual void Initialize(const iEvent& ev);

    };
  } //Entity namespace
} //PT namespace

#endif
