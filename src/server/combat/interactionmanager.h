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

#ifndef INTERACTIONMANGER
#define INTERACTIONMANGER

#include "common/util/thread.h"
#include "interaction.h"

#include "src/server/entity/pcentity.h"

class InteractionManager : public Thread
{
  public:
  InteractionManager();
  ~InteractionManager();
  bool SelectTarget(const PcEntity *sourceEntity, unsigned int tagetID);
  bool QueueAction(const PcEntity *sourceEntity, unsigned int actionID);

  private:
  bool NormalAttack(Interaction* interaction);
  bool DeductStamina(Character* lockedCharacter);
  float GetAttackChance(Character* lockedAttacker, Character* lockedTarget);
  float CalculateDamage(Character* lockedAttacker, Character* lockedTarget);
  bool PerformInteraction(Interaction* interaction);
  bool TargetAttackable(Character* lockedAttacker, Character* lockedTarget);
  const Character* GetTargetCharacter(Character* lockedCharacter);

};
#endif //INTERACTIONMANGER
