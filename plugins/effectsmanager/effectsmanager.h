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

#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H

#include "include/effectsmanager.h"

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>

#include <csutil/ref.h>
#include <iutil/virtclk.h>

#include <string>

#include "effect.h"

struct iObjectRegistry;
struct iSector;
struct iEvent;

namespace PT
{
  namespace Events
  {
    struct EventHandlerCallback;
  }
}

/**
 * @ingroup effects
 * Handles the creation of particle/mesh effects.
 */
class EffectsManager : public scfImplementation3<EffectsManager,iEffectsManager,iEventHandler,iComponent>
{
private:
  bool UpdateOptions();

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);

private:
  /// The object registry.
  iObjectRegistry* object_reg;
  csRef<iEngine> engine;
  csRef<iVirtualClock> vc;

private:
  ///Effect instances. Instances are owned by EffectsManager.
  csRefArray<Effect> effects;

  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

  /**
   * This function is called every frame.
   * It updates all effects and removes them when their times has past.
   * @param elapsed_ticks Ticks elapsed since last frame.
   */
  void HandleEffects(csTicks elapsed_ticks);

  /**
   * Looks up an effect by name in the EffectDataManager.
   * @param effectName Unique name for the wanted effect.
   * @return Pointer to the created iMeshWrapper, 0 if an error occured.
   */
  PT::Data::Effect* GetEffectData (const std::string& effectName);

  bool CreateEffect (iEvent& ev);

private:
  /// The queue of events waiting to be handled.
  csRef<iEventQueue> eventQueue;
  /// The event name registry, used to convert event names to IDs and back.
  csRef<iEventNameRegistry> nameRegistry;

  bool HandleEvent(iEvent& ev);

  CS_EVENTHANDLER_NAMES ("peragro.effectsmanager")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

public:
  /**
   * Base constructor.
   */
  EffectsManager (iBase* iParent);
  ~EffectsManager ();

  /**
   * Gets some CS plugins like the Loader, VFS and Engine.
   * @return True if successful, false if an error occured.
   */
  bool Initialize (iObjectRegistry* obj_reg);

  /**
   * @param effectName Unique name for the wanted effect.
   * @param parent iMeshWrapper to bind this effect to.
   * @return True if successful, false if an error occured.
   */
  bool CreateEffect (const std::string& effectName, iMeshWrapper* parent);

  /**
   * @param effectName Unique name for the wanted effect.
   * @param pos Position to where to create this effect.
   * @return True if successful, false if an error occured.
   */
  bool CreateEffect (const std::string& effectName, const csVector3& pos, iSector* sector = 0);

  /**
   * @param pos Position to where to create this effect.
   * @return True if successful, false if an error occured.
   */
  bool CreateDecal (csVector3 pos);

};

#endif // EFFECTSMANAGER_H
