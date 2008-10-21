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
 * @file viewcontrols.h
 *
 * @basic Handles the player view controls.
 */

#ifndef VIEWCONTROLS_H
#define VIEWCONTROLS_H

#include <csutil/scf_implementation.h>
#include <csutil/refarr.h>
#include <csutil/weakref.h>

#include "include/client/component/entity/input/viewcontrols.h"
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

PT_DECLARE_COMPONENTFACTORY (ViewControls)

/**
 * Handles the player view controls.
 */
class ComponentViewControls : public scfImplementationExt1<ComponentViewControls, ComponentCommon, iViewControls>
{
private:
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The pointer library.
  PointerLibrary* pointerlib;

  /// The player entity.
  PT::Entity::Entity* entity;
  /// The event handlers.
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  /// Player entity's camera.
  csWeakRef<iPcDefaultCamera> camera;
  /// Whether to invert the look up and down controls, like an aeroplane.
  bool invertYAxis;
  /// Distance between camera and player entity, aka zoom.
  float cameraDistance;
  /// Whether to do adaptive distance clipping or not.
  bool distClip;
  /// Minimum FPS for the adaptive distance clipping.
  float minFPS;
  /// Maximum FPS for the adaptive distance clipping.
  float maxFPS;
  /// Minimum view distance for the adaptive distance clipping.
  float minDistance;

  /// Handler for the look left event.
  bool ActionLookLeft(iEvent& ev);
  /// Handler for the look right event.
  bool ActionLookRight(iEvent& ev);
  /// Handler for the look up event.
  bool ActionLookUp(iEvent& ev);
  /// Handler for the look down event.
  bool ActionLookDown(iEvent& ev);
  /// Handler for the zoom in event.
  bool ActionZoomIn(iEvent& ev);
  /// Handler for the zoom out event.
  bool ActionZoomOut(iEvent& ev);
  /// Handler for the camera mode change event.
  bool ActionCameraMode(iEvent& ev);
  /// Handler for the distance clipping toggle event, this removes entities
  /// beyond the set distance.
  bool ActionDistanceClipping(iEvent& ev);
  /// Handler for the configuration update event.
  bool UpdateOptions(iEvent& ev);
  /// Reloads values from the configuration manager.
  bool UpdateOptions();
  /// Update the distance clipping.
  bool UpdateDistanceClipping();

public:
    /// Constructor.
    ComponentViewControls (iObjectRegistry*);
    /// Destructor.
    virtual ~ComponentViewControls();

    /**
     * Initialize.
     * @param pl The pointer library.
     * @param ent The entity to handle view control events for.
     * @return True, indicating success.
     */
    virtual bool Initialize (PointerLibrary* pl, PT::Entity::Entity* ent);
};

#endif
