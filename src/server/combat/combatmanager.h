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

#ifndef SERVER_COMBAT_MGR
#define SERVER_COMBAT_MGR

#include "src/server/entity/pcentity.h"

class CombatManager {
  private:
    CombatManager *combatMgr;
  public:
    CombatManager();
    ~CombatManager();
    int AttackRequest(const PcEntity* attackerEntity, unsigned int targetID);
    int AttackRequest(Character* lockedAttackerCharacter,
                      Character* lockedTargetCharacter);
    bool CheckIfReadyToAttack(Character* attacker);
    bool CheckIfTargetIsAttackable(Character* attacker,
                                   Character* target);
    void DecreaseItemDurability();
    unsigned int GetMaxLife(CharacterStats*);
    float GetAttackChance(Character* lockedAttacker,
                          Character* lockedTarget);
    float GetStaminaDeduction(Character* attacker);
    float GetDamage(Character* attacker);
    void SetLastAttackTime(Character* attacker);
    int PrepareAttack();
    int CalculateAttack();
    float GetAgility(Character* lockedCharacter);
    float GetSkillBonus(Character* lockedCharacter);
    float GetSapience(Character* lockedCharacter);
    float GetBlock(Character* lockedCharacter);
    float GetDodge(Character* lockedCharacter);
    float GetParry(Character* lockedCharacter);
    float GetStrength(Character* lockedCharacter);
    float GetWeaponDamage(Character* lockedCharacter);
    float GetStatValue(Character* lockedCharacter,
                       const char* statName);
    int RollDice();


};
#endif //SERVER_COMBAT_MGR
