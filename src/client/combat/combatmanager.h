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

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>
#include <iengine/engine.h>

#include "physicallayer/pl.h"

#include "client/entity/entity.h"

#include "common/util/mutex.h"

#include "client/network/network.h"

#include "client/pointer/pointer.h"

class Effect;
class GUIManager;
class Network;

struct iObjectRegistry;
struct iLoader;

class CombatMGR
{
private:

  csRef<iEngine> engine;
  csRef<iVFS> vfs;
  csRef<iLoader> loader;
  csRef<iCelPlLayer> pl;

  PT::Effect::EffectsManager* effectsmgr;
  PT::Entity::EntityManager* entitymgr;
  GUIManager* guimanager;
  PT::Data::SkillDataManager* skillmanager;
  Network* network;

  iMeshWrapper* getMesh(PT::Entity::Entity* entity);

public:
  CombatMGR ();
  ~CombatMGR ();

  bool Initialize ();

  void hit (int targetId, int damage);
  void die (int targetId);
  void levelup (int targetId);
  void experience (int exp);
  void SkillUsageStart (unsigned int casterId, unsigned int targetId, int skillId, ptString error);
  void SkillUsageComplete (unsigned int casterId, unsigned int targetId, int skillId);
  void RequestSkillUsageStart (iCelEntity* target, unsigned int skillId);
  void RequestSkillUsageStart (unsigned int targetId, unsigned int skillId);

  /**
   * Handler for ACTION_HIT event.
   * @param ev Event describing the hit.
   * @return False if an error occured, true otherwise.
   */
  bool ActionHit(iEvent& ev);
};

#endif // COMBATMANAGER_H
