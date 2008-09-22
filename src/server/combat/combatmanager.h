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
/**
 * @file combatmanager.h
 *
 * @basic Server combat manager.
 */

#ifndef SERVER_COMBAT_MGR
#define SERVER_COMBAT_MGR

#include "src/server/entity/pcentity.h"

/**
 * Server combat manager.
 */
class CombatManager
{
private:
  /// @todo unused pointer.
  CombatManager *combatMgr;
public:
  /// Constructor.
  CombatManager();
  /// Destuctor.
  ~CombatManager();

  /**
   * Handles a request from a player to attack a target.
   * @param attackerEntity The PcEntity for the attacking player.
   * @param targetID the Entity ID of the target.
   * @return False upon failure.
   */
  bool AttackRequest(const PcEntity* attackerEntity, unsigned int targetID);

  /**
   * Handles a request from a player to attack a target
   * @todo This function is just way to big, divide it.
   * @param lockedAttackerCharacter The attacking character.
   * @param lockedTargetCharacter The attacked character.
   * @return False upon failure
   */
  bool AttackRequest(Character* lockedAttackerCharacter,
                    Character* lockedTargetCharacter);

  /**
   * Checks if the player is ready to attack.
   * @todo needs to be implemented.
   * @param lockedCharacter the locked version of a character.
   * @return true if ready to attack, otherwise false.
   */
  bool CheckIfReadyToAttack(Character* attacker);

  /**
   * Checks if the target is attackable, in reach etc.
   * @todo
   * @param attacker The attacking character.
   * @param target The target character.
   * @return True if target is attackable, otherwise false.
   */
  bool CheckIfTargetIsAttackable(Character* attacker,
                                 Character* target);

  /**
   * @todo Dummy function.
   */
  void DecreaseItemDurability();

  /**
   * This effectively gets endurance, as max life is 1 * endurance.
   * @param lockedStats The characters stat container.
   * @return The max life.
   */
  unsigned int GetMaxLife(CharacterStats* lockedStats);

  /**
   * Calculates the attacking character's chance of hitting.
   * @param lockedCharacter The locked version of the attacking character.
   * @return attack chance.
   */
  float GetAttackChance(Character* lockedAttacker,
                        Character* lockedTarget);

  /**
   * @todo Dummy function.
   */
  float GetStaminaDeduction(Character* attacker);

  /**
   * @todo Dummy function.
   */
  float GetDamage(Character* attacker);

  /**
   * @todo Dummy function.
   */
  void SetLastAttackTime(Character* attacker);

  /**
   * @todo Is this really needed.
   */
  int PrepareAttack();

  /**
   * @todo Dummy function.
   */
  int CalculateAttack();

  /**
   * Returns the combined agility for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters agility.
   */
  float GetAgility(Character* lockedCharacter);

  /**
   * Returns the combined skill bonuses.
   * @todo
   * @param lockedCharacter The locked version of the character.
   * @return The characters skill bonuses.
   */
  float GetSkillBonus(Character* lockedCharacter);

  /**
   * Returns the combined sapience for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters sapience.
   */
  float GetSapience(Character* lockedCharacter);

  /**
   * Returns the combined block for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters block.
   */
  float GetBlock(Character* lockedCharacter);

  /**
   * Returns the combined dodge for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters dodge.
   */
  float GetDodge(Character* lockedCharacter);

  /**
   * Returns the combined parry for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters parry.
   */
  float GetParry(Character* lockedCharacter);

  /**
   * Returns the combined strength for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters strength.
   */
  float GetStrength(Character* lockedCharacter);

  /**
   * Calculates the damage given by the character's weapon(s).
   * @param lockedCharacter The locked version of the character.
   * @return The weapon damage.
   */
  float GetWeaponDamage(Character* lockedCharacter);

  /**
   * Queries the stat for a character based on the stat's name.
   * It would be tempting to use this function directly,however a character can
   * get bonuses, therefore this function should be called from the different
   * getters for stats.
   * @param lockedCharacter The locked version of the character.
   * @param statName The stat's name.
   * @return The stats current value.
   */
  float GetStatValue(Character* lockedCharacter, const char* statName);

  /**
   * Deducts the players stamina after an attack.
   * @param lockedCharacter The character to deduct stamina for.
   */
  void DeductStamina(Character* lockedCharacter);

  /**
   * Wrapper function to send stat update.
   * @param stat The type of stat to update.
   * @param stats The characters stat container.
   * @param lockedCharacter The locked version of the character.
   * @param name The name of the stat.
   * @param target Whom to send to.
   */
  void SendStatUpdate(const Stat* stat, const CharacterStats* stats,
                      Character* lockedCharacter, const char* name, int target);

  /**
   * Function to get the weapon(s) heft value.
   * @param lockedCharacter The owner of the weapon
   * @return The weapon(s) heft value.
   */
  float GetWeaponHeft(Character* lockedCharacter);

  /**
   * Get a random number between 0 and 100.
   */
  int RollDice();

  /**
   * Queries the stat value from an item based on stat name.
   * @param item The item to return the stat value for.
   * @param statName The stat's name.
   * @return The items stat based on the statName.
   */
  float GetStatValueForItem(const Item* item, const char* statName);

  /**
   * Combines the character's items stat values, based on stat name
   * @param lockedCharacter The locked version of the character.
   * @param statName The stat's name.
   * @return The items bonuses given a special stat.
   */
  float GetStatValueForAllEquipedItems(Character* lockedCharacter,
                                       const char* statName);

  /**
   * Combines the characters weapon stat values, based on stat name
   * @param lockedCharacter The locked version of the character.
   * @param statName The stat's name.
   * @return The weapons bonuses given a special stat.
   */
  float GetStatValueForEquipedWeapons(Character* lockedCharacter,
                                      const char* statName);

  /**
   * Used to get an equiped item based on the slot number.
   * @param lockedCharacter The locked version of the character.
   * @param slot Which item, is decided by the slot its equiped on.
   * @return The item in that slot, or NULL if none found.
   */
  Item* GetItem(Character* lockedCharacter, unsigned char slot);

};

namespace CombatManagerSendTo
{
  enum SendTo
  {
    CHARACTER,
    LOCALCAST,
    BROADCAST,
  };
} // CombatManagerSendTo namespace
#endif // SERVER_COMBAT_MGR
