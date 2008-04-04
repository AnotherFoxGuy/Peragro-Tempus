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

#include <iutil/cfgmgr.h>
#include <iutil/cfgfile.h>

namespace PT
{
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
    private:
      static PlayerEntity* instance;

      void ReInit(const Events::EntityAddEvent* ev);

      ///Player entity's camera.
      csWeakRef<iPcDefaultCamera> camera;
      ///Direction of moving. -1 for backward, 0 for standing still, 1 for
      ///forward.
      char walk;
      ///Direction of turning. -1 for left, 0 for straight, 1 for right.
      char turn;
      ///Determines if player is running or not.
      bool run;
      ///Determines if player is jumping or not.
      bool jump;
      ///Determines if the client is ready for movement and other actions or not.
      bool ready;
      ///Distance between camera and player entity, aka zoom.
      float cameraDistance;
      ///Current pitch offset for hopping while walking and running.
      float currentPitch;
      ///Pitch per second change for pitch offset while walking and running.
      float pitchPerSecond;
      ///Pitch offset range for hopping while walking and running.
      float pitchRange;
      ///"Direction" in which the pitch offset should be changed (ie adding or
      ///substracting from it) when moving in first person view.
      int pitchDirection;

      ///Whether to reverse turning when walking backwards.
      bool backwardReverse;
      ///Whether to invert the look up and down controls, like an aeroplane.
      bool invertYAxis;
      ///Minimum FPS for the adaptive distance clipping
      float minFPS;
      ///Maximum FPS for the adaptive distance clipping
      float maxFPS;
      ///Minimum view distance for the adaptive distance clipping
      float minDistance;

      csRef<iConfigManager> app_cfg;
      csRef<iVFS> vfs;

      csRef<iCelEntity> other_self;

      /**
       * Constructor that sets up the player using the information provided by
       * EntityAddEvent event.
       * @see Entity::Entity(const Events::EntityAddEvent&)
       * @param ev Event used for initialising the player properties.
       */
      PlayerEntity(const Events::EntityAddEvent& ev);
      virtual ~PlayerEntity();

      void Create();

      ///Handler for a walk forward event.
      bool ActionForward(PT::Events::Eventp ev);
      ///Handler for a walk backward event.
      bool ActionBackward(PT::Events::Eventp);
      ///Handler for a turn left event.
      bool ActionLeft(PT::Events::Eventp);
      ///Handler for a turn right event.
      bool ActionRight(PT::Events::Eventp);
      ///Handler for a toggle walk event. Called when player presses the "keep
      ///walking" key.
      bool ActionToggleWalk(PT::Events::Eventp);
      ///Handler for a toggle run event. Called when switching from walking to
      ///running and back.
      bool ActionToggleRun(PT::Events::Eventp);
      ///Handler for a pan up event.
      bool ActionPanUp(PT::Events::Eventp);
      ///Handler for a pan down event.
      bool ActionPanDown(PT::Events::Eventp);
      ///Handler for a camera toggle event. Called when changing camera type.
      bool ActionToggleCamera(PT::Events::Eventp);
      ///Handler for a distance clipping event. Called when removing entities
      ///beyond some distance.
      bool ActionToggleDistClipping(PT::Events::Eventp);
      ///Handler for an activate weapon event. Called when attacking with a
      ///weapon (in order to produce animation).
      bool ActionActivateWeapon(PT::Events::Eventp ev);
      ///Handler for a zoom-in event.
      bool ActionZoomIn(PT::Events::Eventp ev);
      ///Handler for a zoom-out event.
      bool ActionZoomOut(PT::Events::Eventp ev);
      ///Handler for a jump event.
      bool ActionJump(PT::Events::Eventp ev);
      ///Handler for a move-to event.
      bool ActionMoveTo(PT::Events::Eventp ev);
      ///Handler for a configuration update event
      bool UpdateOptions(PT::Events::Eventp ev);

      ///Helper method for sending new movement information.
      bool PerformMovementAction();

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
      static PlayerEntity* Instance(const Events::EntityAddEvent* ev = 0);

      /**
       * Set whether the client is ready or not.
       * @param value New value.
       */
      void SetReady(bool value) { ready = value; }

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
       * @todo We need a reliable way for determining FPS. The hopping effect
       * doesn't work for PT::Client::ActionMoveTo()
       * @param fpsLimit FPS limit used for calculating hopping while walking or
       * running.
       */
      void CameraDraw(unsigned int fpsLimit=1000);

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
    };
  }
}

#endif
