/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csgeom/vector3.h>
#include <csutil/csstring.h>
#include <csutil/refarr.h>
#include <csutil/weakref.h>

#include "include/client/component/entity/input/playercontrols.h"
#include "include/client/component/componenttmpl.h"

#include "client/component/stdcomponentimp.h"

struct iObjectRegistry;
struct iEvent;
struct iPcDefaultCamera;

namespace PT
{
  namespace Events
  {
    struct EventHandlerCallback;
  }
  namespace Entity
  {
    class Entity;
  }
} //PT namespace

DECLARE_COMPONENTFACTORY (PlayerControls)

class ComponentPlayerControls : public scfImplementationExt1<ComponentPlayerControls, ComponentCommon, iPlayerControls>
{
private:
  iObjectRegistry* object_reg;
  PointerLibrary* pointerlib;

  PT::Entity::Entity* entity;
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  ///Player entity's camera.
  csWeakRef<iPcDefaultCamera> camera;
  ///Whether to invert the look up and down controls, like an aeroplane.
  bool invertYAxis;
  ///Minimum view distance for the adaptive distance clipping.
  float minDistance;
  ///Distance between camera and player entity, aka zoom.
  float cameraDistance;
  ///Minimum FPS for the adaptive distance clipping.
  float minFPS;
  ///Maximum FPS for the adaptive distance clipping.
  float maxFPS;

  ///Direction of moving. -1 for backward, 0 for standing still, 1 for
  ///forward.
  char walk;
  ///Direction of turning. -1 for left, 0 for straight, 1 for right.
  char turn;
  ///Determines if player is running or not.
  bool run;
  ///Determines if player is jumping or not.
  bool jump;
  ///Specifies whether character is sitting or not.
  ///@todo It's not very logical to have boats sit, is it? (boats will
  ///probably be MountEntities).
  bool sitting;
  ///Whether to reverse turning when walking backwards.
  bool backwardReverse;
  ///Whether to do local movement instead of waiting for the server.
  bool local_movement;


  ///Helper method for sending new movement information.
  bool PerformMovementAction();

  ///Handler for a walk forward event.
  bool ActionForward(iEvent& ev);
  ///Handler for a walk backward event.
  bool ActionBackward(iEvent& ev);
  ///Handler for a turn left event.
  bool ActionLeft(iEvent& ev);
  ///Handler for a turn right event.
  bool ActionRight(iEvent& ev);
  ///Handler for a toggle walk event. Called when player presses the "keep
  ///walking" key.
  bool ActionToggleWalk(iEvent& ev);
  ///Handler for a toggle run event. Called when switching from walking to
  ///running and back.
  bool ActionToggleRun(iEvent& ev);
  ///Handler for a pan up event.
  bool ActionPanUp(iEvent& ev);
  ///Handler for a pan down event.
  bool ActionPanDown(iEvent& ev);
  ///Handler for a camera toggle event. Called when changing camera type.
  bool ActionToggleCamera(iEvent& ev);
  ///Handler for a distance clipping event. Called when removing entities
  ///beyond some distance.
  bool ActionToggleDistClipping(iEvent& ev);
  ///Handler for an activate weapon event. Called when attacking with a
  ///weapon (in order to produce animation).
  bool ActionActivateWeapon(iEvent& ev);
  ///Handler for a zoom-in event.
  bool ActionZoomIn(iEvent& ev);
  ///Handler for a zoom-out event.
  bool ActionZoomOut(iEvent& ev);
  ///Handler for a jump event.
  bool ActionJump(iEvent& ev);
  ///Handler for a move-to event.
  bool ActionMoveTo(iEvent& ev);
  ///Handler for a configuration update event
  bool UpdateOptions(iEvent& ev);
  /**
   * Reloads values from the configuration manager
   */
  bool UpdateOptions();


public:
    ComponentPlayerControls (iObjectRegistry*);
    virtual ~ComponentPlayerControls();

    virtual bool Initialize (PointerLibrary*, PT::Entity::Entity*);
};

#endif
