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

#include "effect.h"

struct iEngine;
struct iVFS;
struct iObjectRegistry;
struct iLoader;

namespace PT
{
  namespace Data
  {

    class EffectsManager
    {
    private:

      csArray<Effect> effects;

      csRef<iObjectRegistry> obj_reg;
      csRef<iEngine> engine;
      csRef<iVFS> vfs;
      csRef<iLoader> loader;

      iMeshWrapper* CreateEffectMesh (int effect);

    public:

      EffectsManager (iObjectRegistry* obj_reg);
      ~EffectsManager ();

      enum EffectType
      {
        Blood=0,
        Levelup=1,
        Pentagram=2,
        Healeffect=3,
        Deflect=4,
        Die=5,
        Energysphere=6,
        Energyspear=7,
        Healspell=8,
        MoveMarker=9
      };

      bool Initialize ();


      bool CreateEffect (iMeshWrapper* parent, int effect);
      bool CreateEffect (int effect, csVector3 pos);

      bool CreateDecal (csVector3 pos, iCamera* camera);

      void HandleEffects(csTicks elapsed_ticks);

    };

  } // Data namespace 
} // PT namespace 

#endif // EFFECTSMANAGER_H
