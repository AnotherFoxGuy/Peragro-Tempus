/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include <cssysdef.h>
#include <csutil/ref.h>

#include <string>

#include "effect.h"

struct iEngine;
struct iVFS;
struct iObjectRegistry;
struct iLoader;
struct iCollection;
struct iSector;

namespace PT
{
  namespace Effect
  {
    class FactoryManager;

    /**
     * @ingroup effects
     * Handles the creation of particle/mesh effects.
     */
    class EffectsManager
    {
    private:
      ///Effect instances. Instances are owned by EffectsManager.
      csArray<Effect> effects;

      csRef<iObjectRegistry> obj_reg;
      csRef<iEngine> engine;

      FactoryManager* factoryManager;

      /**
       * Looks up an effect by name in the EffectDataManager
       * and creates a CS mesh with it's information.
       * @param effectName Unique name for the wanted effect.
       * @return Pointer to the created iMeshWrapper, 0 if an error occured.
       */
      iMeshWrapper* CreateEffectMesh (const std::string& effectName);

    public:
      /**
       * Base constructor.
       */
      EffectsManager ();
      ~EffectsManager ();

      /**
       * Gets some CS plugins like the Loader, VFS and Engine.
       * @return True if successful, false if an error occured.
       */
      bool Initialize ();

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
      bool CreateEffect (const std::string& effectName, csVector3 pos, iSector* sector = 0);

      /**
       * @param pos Position to where to create this effect.
       * @return True if successful, false if an error occured.
       */
      bool CreateDecal (csVector3 pos);

      /**
       * This function is called every frame, don't call this yourself!
       * It updates all effects and removes them when their times has past.
       * @param elapsed_ticks Ticks elapsed since last frame.
       */
      void HandleEffects(csTicks elapsed_ticks);

    };

  } // Effect namespace
} // PT namespace

#endif // EFFECTSMANAGER_H
