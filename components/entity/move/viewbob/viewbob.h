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
 * @file viewbob.h
 *
 * @basic The view bobbing effect.
 */

#ifndef CLIPBOARD_DEFAULT_HEADER
#define CLIPBOARD_DEFAULT_HEADER

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csgeom/vector3.h>
#include <csutil/csstring.h>
#include <csutil/refarr.h>
#include <iutil/virtclk.h>
#include <csutil/weakref.h>

#include "include/client/component/entity/move/viewbob.h"
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

PT_DECLARE_COMPONENTFACTORY (ViewBob)

/**
 * The view bobbing effect applied to the view as the player character moves.
 */
class ComponentViewBob : public scfImplementationExt1<ComponentViewBob, ComponentCommon, iViewBob>
{
private:
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The pointer library.
  PointerLibrary* pointerlib;

  /// The virtual clock.
  csRef<iVirtualClock> vc;
  /// The ticks at the last valid offset change.
  csTicks lastTicks;

  /// The player entity.
  PT::Entity::Entity* entity;
  /// The event handlers.
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  /// Player entity's camera.
  csWeakRef<iPcDefaultCamera> camera;

  /**
   * Advance to the next frame of the effect.
   * @param ev The event.
   * @return False, so the crystalspace.frame event is still handled elsewhere.
   */
  bool Frame(iEvent& ev);

  /**
   * Change the view height when moving.
   * @param elapsedTicks The ticks elapsed since last frame.
   * @return Whether the offset was changed.
   */
  bool Move(float elapsedTicks);

  /**
   * Change the view height to the standard.
   * @param hard Whether to reset it now or change gradually.
   * @param elapsedTicks The ticks elapsed since last frame.
   * @return Whether the offset was changed.
   */
  bool Reset(bool hard, float elapsedTicks = 0.0f);

  /// Base height of the view.
  float baseHeight;
  /// Offset to the height for the bobbing effect.
  float currentOffset;
  /// Time period of one cycle of view movement change.
  float timePeriod;
  /// Camera offset range.
  float offsetRange;
  /// The direction of camera movement, true is up, false is down.
  bool upwards;

public:
  /// Constructor.
  ComponentViewBob (iObjectRegistry*);
  /// Destructor.
  virtual ~ComponentViewBob();

  /**
   * Initialize the effect
   * @param pl The pointer library.
   * @param ent The entity to apply the effect to.
   */
  virtual bool Initialize (PointerLibrary* pl, PT::Entity::Entity* ent);

  // TODO: add functions or events to change the range and time period,
  // currently the effect is not changed when you run. Might be a good idea
  // to tie it in with the pose manager somehow, and have the values stored in
  // an xml or the database along with the animations.
};

#endif
