/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include <iutil/string.h>
#include <iutil/vfs.h>
#include <csutil/ref.h>

#include <iengine/mesh.h>

#include "client/data/effect/dataeffect.h"

struct iObjectRegistry;
struct iCollection;

namespace PT
{
  namespace Effect
  {
    struct iModelFactory : public virtual iBase
    {
      SCF_INTERFACE(iModelFactory, 1,0,0);
    };

    struct Factory : public scfImplementation1<Factory, iModelFactory>
    {
    private:
      csRef<iCollection> collection;
      iObjectRegistry* object_reg;

    public:
      Factory(const std::string& fileName, iObjectRegistry* object_reg);
      ~Factory();
      std::string fileName;
      /// Don't call this function yourself!
      void Load();
      /// Get the collection that is used internally for holding the data.
      iCollection* GetCollection() { return collection; }
      /// GetObjectRegistry
      iObjectRegistry* GetObjectRegistry() { return object_reg; }
    };

    /**
     * @ingroup effects
     * Helper class that holds the CS mesh and remaining time.
     * @see EffectsManager
     */
    class Effect 
    {
    private:
      ///iMeshWrapper - CS mesh of the effect.
      csRef<iMeshWrapper> mesh;
      ///Factory - factory of the effect.
      csRef<Factory> factory;
      ///Remaining time on the effect till destruction.
      float timeleft;
      /// meshName
      std::string meshName;
      /// offset
      PtVector3 offset;

    public:
      /**
       * Base constructor.
       * @param Effect::Factory of the effect.
       * @param Data::Effect.
       */
      Effect (Factory* factory, Data::Effect* eff);
      ~Effect();

      /// Don't call this function yourself!
      void Load();

      /**
       * This function is called every frame by the EffectsManager, 
       * don't call this yourself!
       * @param elapsed_ticks Ticks elapsed since last frame.
       */
      bool Handle (csTicks elapsed_ticks);

      /**
       * @return CS mesh of the effect.
       */
      iMeshWrapper* GetMesh () const { return mesh; }	  
    };

  } // Data namespace 
} // PT namespace 

#endif // EFFECT_H
