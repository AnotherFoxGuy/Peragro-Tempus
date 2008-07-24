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
/**
 * @file playercontrols.h
 *
 * @basic Handles the player character controls.
 */

#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <csutil/scf_implementation.h>
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

/**
 * Handles the player character controls.
 */
class ComponentPlayerControls : public scfImplementationExt1<ComponentPlayerControls, ComponentCommon, iPlayerControls>
{
private:
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The pointer library.
  PointerLibrary* pointerlib;

  /// The player entity.
  PT::Entity::Entity* entity;
  ///Player entity's camera.
  csWeakRef<iPcDefaultCamera> camera;
  /// The event handlers.
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  /// Direction of moving. -1 for backward, 0 for standing still, 1 for
  /// forward.
  char walk;
  /// Direction of turning. -1 for left, 0 for straight, 1 for right.
  char turn;
  /// Determines if player is running or not.
  bool run;
  /// Determines if player is jumping or not.
  bool jump;
  /// Specifies whether character is sitting or not.
  /// @todo It's not very logical to have boats sit, is it? (boats will
  /// probably be MountEntities).
  bool sitting;
  /// Whether to reverse turning when walking backwards.
  bool backwardReverse;
  /// Whether to send local movement events instead of waiting for the server.
  bool localMovement;


  /// Helper method for sending new movement information.
  bool PerformMovementAction();

  /// Handler for the walk forward event.
  bool ActionForward(iEvent& ev);
  /// Handler for the walk backward event.
  bool ActionBackward(iEvent& ev);
  /// Handler for the turn left event.
  bool ActionLeft(iEvent& ev);
  /// Handler for the turn right event.
  bool ActionRight(iEvent& ev);
  /// Handler for the toggle walk event. Called when player presses the "keep
  /// walking" key.
  bool ActionToggleWalk(iEvent& ev);
  /// Handler for the toggle run event. Called when switching from walking to
  /// running and back.
  bool ActionToggleRun(iEvent& ev);
  /// Handler for the activate weapon event. Called when attacking with a
  /// weapon (in order to produce animation).
  bool ActionActivateWeapon(iEvent& ev);
  /// Handler for the jump event.
  bool ActionJump(iEvent& ev);
  /// Handler for the move-to event.
  bool ActionMoveTo(iEvent& ev);
  /// Handler for the configuration update event.
  bool UpdateOptions(iEvent& ev);
  /// Reloads values from the configuration manager.
  bool UpdateOptions();


public:
    /// Constructor.
    ComponentPlayerControls (iObjectRegistry*);
    /// Destructor.
    virtual ~ComponentPlayerControls();

    /**
     * Initialize.
     * @param pl The pointer library.
     * @param ent The entity to handle control events for.
     * @return True, indicating success.
     */
    virtual bool Initialize (PointerLibrary* pl, PT::Entity::Entity* ent);
};

#endif
