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

#ifndef COMBATMANAGER_H
#define COMBATMANAGER_H

#include "cssysdef.h"
#include "csutil/ref.h"
#include "iutil/vfs.h"
#include "iengine/engine.h"

#include "physicallayer/pl.h"

#include "client/entity/ptcelentity.h"

#include "common/util/mutex.h"

#include "client/client.h"

#include "client/network/network.h"

class Client;

class Effect;
class GUIManager;

struct iObjectRegistry;
struct iLoader;

class CombatMGR
{
private:

  csRef<iEngine> engine;
  csRef<iVFS> vfs;
  csRef<iLoader> loader;
  csRef<iCelPlLayer> pl;

  Client* client;
  EffectsManager* effectsmgr;
  ptEntityManager* entitymgr;
  GUIManager* guimanager;

  iMeshWrapper* getMesh(iCelEntity* entity);

public:

  enum AttackType
  {
  EnergyBind=0,
  EnergySpear=1,
  Melee=2,
  Heal=3
  };

  CombatMGR (Client* client);
  ~CombatMGR ();

  bool Initialize (iObjectRegistry* obj_reg);

  void hit (int targetId, int damage);
  void die (int targetId);
  void levelup (int targetId);
  void experience (int exp);
  void Attack (int attackerId, int targetId, int attackId, int error);
  void RequestAttack (iCelEntity* target, int attackId);


};

#endif // COMBATMANAGER_H
