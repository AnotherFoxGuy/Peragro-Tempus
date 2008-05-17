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

DECLARE_COMPONENTFACTORY (ViewControls)

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
  /// Minimum view distance for the adaptive distance clipping.
  float minDistance;
  /// Distance between camera and player entity, aka zoom.
  float cameraDistance;
  /// Minimum FPS for the adaptive distance clipping.
  float minFPS;
  /// Maximum FPS for the adaptive distance clipping.
  float maxFPS;

  /// Handler for the pan left event.
  bool ActionPanLeft(iEvent& ev);
  /// Handler for the pan right event.
  bool ActionPanRight(iEvent& ev);
  /// Handler for the pan up event.
  bool ActionPanUp(iEvent& ev);
  /// Handler for the pan down event.
  bool ActionPanDown(iEvent& ev);
  /// Handler for the zoom-in event.
  bool ActionZoomIn(iEvent& ev);
  /// Handler for the zoom-out event.
  bool ActionZoomOut(iEvent& ev);
  /// Handler for the camera toggle event. Called when changing camera type.
  bool ActionToggleCamera(iEvent& ev);
  /// Handler for the distance clipping event, to removing entities beyond
  /// some distance.
  bool ActionToggleDistClipping(iEvent& ev);
  /// Handler for the configuration update event.
  bool UpdateOptions(iEvent& ev);
  /// Reloads values from the configuration manager.
  bool UpdateOptions();


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