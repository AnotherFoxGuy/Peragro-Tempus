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

#include "cssysdef.h"
#include "csutil/ref.h"
#include "iutil/vfs.h"
#include "iengine/engine.h"

#include "client/effects/effect.h"

class Effect;

struct iObjectRegistry;
struct iLoader;

class EffectsManager
{
private:

  csArray<Effect> effects;

  csRef<iEngine> engine;
  csRef<iVFS> vfs;
  csRef<iLoader> loader;

  iMeshWrapper* EffectsManager::CreateEffectMesh (int effect);

public:

  EffectsManager (iObjectRegistry* obj_reg);
  ~EffectsManager ();

  enum EffectType
  {
    Blood=0,
    Levelup=1
  };

  bool Initialize (iObjectRegistry* obj_reg);

 
  bool CreateEffect (iMeshWrapper* parent, int effect);

  void HandleEffects(csTicks elapsed_ticks);

};

#endif // EFFECTSMANAGER_H
