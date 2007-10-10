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
#include "client/event/event.h"

namespace PT
{
  namespace Entity
  {
    /**
     * You get an instance of the class by calling
     * Instance(const Events::EntityAddEvent* ev). To initialize the
     * instance, you must pass a valid non-NULL pointer the first time.
     * Every subsequent pass of valid non-NULL pointer will reinitialize
     * the class. This may be used when relogging or doing a login/logout.
     * @brief Singleton class representing the player himself.
     * @author Branko Majic <branko.majic NO SPAM AT gmail.com>
    */
    class PlayerEntity : public PcEntity
    {
    private:
      csWeakRef<iPcDefaultCamera> camera;/**<Player entity's camera.*/

      char walk; /**<Determines direction of moving. -1 for backward, 0 for standing still, 1 for forward.*/
      char turn; /**<Determines direction of turning. -1 for left, 0 for straight, 1 for right.*/
      bool run; /**<Determines if player is running or not.*/
      bool ready; /**<Determines if the client is ready for movement and other actions or not.*/
      float cameraDistance; /**<Distance between camera and player entity, aka zoom.*/

      PlayerEntity(const Events::EntityAddEvent& ev);

      ~PlayerEntity() {};
      void Create();

      bool ActionForward(PT::Events::Eventp ev); ///<Handler for a walk forward event.
      bool ActionBackward(PT::Events::Eventp); ///<Handler for a walk backward event.
      bool ActionLeft(PT::Events::Eventp); ///<Handler for a turn left event.
      bool ActionRight(PT::Events::Eventp); ///<Handler for a turn right event.
      bool ActionToggleWalk(PT::Events::Eventp); ///<Handler for a toggle walk event. Called when player presses the "keep walking" key.
      bool ActionToggleRun(PT::Events::Eventp); ///<Handler for a toggle run event. Called when switching from walking to running and back.
      bool ActionPanUp(PT::Events::Eventp); ///<Handler for a pan up event.
      bool ActionPanDown(PT::Events::Eventp); ///<Handler for a pan down event.
      bool ActionToggleCamera(PT::Events::Eventp); ///<Handler for a camera toggle event. Called when changing camera type.
      bool ActionToggleDistClipping(PT::Events::Eventp); ///<Handler for a distance clipping event. Called when removing entities beyond some distance.
      bool ActionActivateWeapon(PT::Events::Eventp ev); ///<Handler for an activate weapon event. Called when attacking with a weapon (in order to produce animation).
      bool ActionZoomIn(PT::Events::Eventp ev); ///<Handler for a zoom-in event.
      bool ActionZoomOut(PT::Events::Eventp ev); ///<Handler for a zoom-out event.

      bool PerformMovementAction(); ///<Helper method for sending new movement information.

    public:
      void Interact(){}

      /**
       * Returns a pointer to an instance of the class. Initiate the instance
       * by passing a non-null valid Events::EntityAddEvent pointer.
       * @param ev Pointer to an EntityAddEvent object. If NULL, existing instance
       *           is returned, otherwise the object is used for initialization of
       *           the new class instance, while deleting the old instance as needed.
       * @return 0 if an error occured, or if no instance was created yet.
       *         Otherwise returns a pointer to an instance of the class.
       */
      static PlayerEntity* Instance(const Events::EntityAddEvent* ev=NULL);

      /**
       * Set whether the client is ready or not.
       * @param value New value.
       */
      void SetReady(bool value) { ready = value; }

      /**
       * @return Returns the player entity's camera.
       */
      iPcDefaultCamera* GetCamera() { return camera; }
    };
  }
}

#endif
