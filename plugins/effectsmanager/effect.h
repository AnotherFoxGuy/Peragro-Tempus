/*
    Copyright (C) 2008 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef EFFECT_H
#define EFFECT_H

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <csutil/ref.h>
#include <csutil/weakref.h>
#include <iengine/mesh.h>

#include "include/cacheuser.h"

#include "client/data/effect/dataeffect.h"

struct iObjectRegistry;
struct iSector;

/**
 * @ingroup effects
 * Helper class that holds the CS mesh and remaining time.
 * @see EffectsManager
 */
class Effect : public iCacheUser, public csRefCount
{
private:
  enum EffectType
  {
    ParentEffectType = 0,
    PositionEffectType
  };
  EffectType effectType;

  //ParentEffectType
  csWeakRef<iMeshWrapper> parentMesh;

  //PositionEffectType
  csVector3 pos;
  iSector* sec;

private:
  /// The object registry.
  iObjectRegistry* object_reg;
  ///iMeshWrapper - CS mesh of the effect.
  csRef<iMeshWrapper> mesh;
  /// meshName
  std::string effectName;
  ///Remaining time on the effect till destruction.
  float timeleft;
  /// meshName
  std::string meshName;
  /// offset
  csVector3 offset;

  /**
   * @return CS mesh of the effect.
   */
  iMeshWrapper* GetMesh () const { return mesh; }

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);

private:
  void Loaded(iCacheEntry* cacheEntry);

  void DoneLoading(bool success) {}

  /**
   * This function is called every frame by the EffectsManager.
   * @param elapsed_ticks Ticks elapsed since last frame.
   */
  bool Handle (csTicks elapsed_ticks);
  friend class EffectsManager;

public:
  /**
   * Base constructor.
   * @param Data::Effect.
   */
  Effect(iObjectRegistry* obj_reg, PT::Data::Effect* eff, csVector3 position, iSector* sector);
  /**
   * Base constructor.
   * @param Data::Effect.
   */
  Effect(iObjectRegistry* obj_reg, PT::Data::Effect* eff, iMeshWrapper* parent);
  ~Effect();
};

#endif // EFFECT_H
