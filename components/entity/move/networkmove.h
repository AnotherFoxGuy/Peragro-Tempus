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
 * @file networkmove.h
 *
 * @basic Component to handle the movement of an entity.
 */

#ifndef NETWORKMOVE_H
#define NETWORKMOVE_H

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csgeom/vector3.h>
#include <csutil/csstring.h>
#include <csutil/refarr.h>
#include "include/client/component/entity/move/networkmove.h"
#include "include/client/component/componenttmpl.h"

#include "client/component/stdcomponentimp.h"

struct iObjectRegistry;
struct iEvent;

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

DECLARE_COMPONENTFACTORY (NetworkMove)

/**
 * Component to handle the movement of an entity.
 */
class ComponentNetworkMove : public scfImplementationExt1<ComponentNetworkMove, ComponentCommon, iNetworkMove>
{
private:
  /**
   * Contains the data for a "move to" event.
   */
  struct MoveToData
    {
      /// The destination coordinates.
      csVector3 destination;
      /// Whether the entity is walking or running.
      bool walking;
      /// The angle to the destination.
      float dest_angle;
      /// The movement speed.
      float walk_speed;
      /// The rotation speed.
      float turn_speed;
      /// The elapsed time since the move to started.
      float elapsed_time;
      /// The time the move to is expected to take.
      float walk_duration;
    };

private:
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The pointer library.
  PointerLibrary* pointerlib;

  /// The entity this component moves.
  PT::Entity::Entity* entity;
  /// The event handlers.
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  /// Whether to use local movement events or wait for the server.
  bool localMovement;
  /// Read the movement options from the configuration.
  bool UpdateOptions(iEvent& ev);

  /**
   * Change entity's 'linear' movement information (changing turning and
   * speed of an entity, or jumping status).
   * @param ev Event containing the movement data for the entity.
   * @return False, so the event will also be handled elsewhere.
   */
  bool Move(iEvent& ev);

  /**
   * Move entity from point A to point B, in linear manner.
   * @param ev Event containing the movement data for the entity.
   * @return True if the movement has been done, false otherwise.
   */
  bool MoveTo(iEvent& ev);

  /// The current move to data.
  MoveToData* moveTo;
  /// The callback for the move to event.
  csRef<PT::Events::EventHandlerCallback> cbMoveToUpdate;
  /// Update the current move to.
  bool MoveToUpdate(iEvent& ev);
  /// Remove the current move to callback.
  bool RemoveMoveToUpdate();

  /**
   * Teleport the entity to a position.
   * @param ev Event containing the teleport data.
   * @return True, indicating the event was handled.
   */
  bool Teleport(iEvent& ev);

  /**
   * Move entity using 'dead reckoning' method.
   * @see http://en.wikipedia.org/wiki/Dead_reckoning
   * @param ev Event containing dead reckoning movement data.
   * @return False, so the event will also be handled elsewhere.
   */
  bool DrUpdate(iEvent& ev);

  /**
   * Sets the gravity to 0 while the world loads.
   * @param ev The world loading event.
   * @return False, so the event will also be handled elsewhere.
   */
  bool DisableGravity(iEvent& ev);

  /**
   * Resets the gravity to normal after the world has loaded.
   * @param ev The world loaded event.
   * @return False, so the event will also be handled elsewhere.
   */
  bool ResetGravity(iEvent& ev);

public:
    /// Constructor.
    ComponentNetworkMove (iObjectRegistry* object_reg);
    /// Destructor.
    virtual ~ComponentNetworkMove();

    /**
     * Initialize this component.
     * @param pl The pointer library.
     * @param ent The entity to handle movement of.
     */
    virtual bool Initialize (PointerLibrary* pl, PT::Entity::Entity* ent);
};

#endif
