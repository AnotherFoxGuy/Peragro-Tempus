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
 * @file interactionmanager.h
 *
 * @basic Server interaction manager.
 */

#ifndef INTERACTIONMANAGER
#define INTERACTIONMANAGER

#include "common/util/thread.h"
#include "interaction.h"

#include "src/server/entity/pcentity.h"
#include "interactionqueue.h"


/**
 * Server interaction manager.
 */
class InteractionManager : public Thread
{
public:
  /// Constructor.
  InteractionManager();
  /// Destructor.
  ~InteractionManager();
  /**
   * Selects new target for a given character.
   * @param sourceEntity entity for the character that request target selection.
   * @param targetID the Entity ID of the target.
   * @return True upon sucess, otherwise false.
   */
  bool SelectTarget(const PcEntity *sourceEntity, unsigned int tagetID);
  /**
   * Selects new target for a given character.
   * @param sourceEntity entity for the character that request target selection.
   * @param interactionID the action ID for the interaction.
   * @return True upon sucess, otherwise false.
   */
  bool QueueInteraction(const PcEntity *sourceEntity,
                        unsigned int interactionID,
                        unsigned int targetID);

  void shutdown();

protected:
  /**
   * Function popping interactions of the head of the interaction queue.
   * @return None.
   */
  void Run();


private:
  /// How far away other characters can be but still receive local updates.
  unsigned int notificationDistance;
  /// The queue containing pending interactions.
  InteractionQueue* interactionQueue;
  /**
   * Perform a normal attack.
   * @param interaction Contains details about the attack.
   * @return True if it was successfull.
   */
  bool NormalAttack(Interaction* interaction);
  /**
   * Deducts the players stamina after an attack.
   * @param lockedCharacter The character to deduct stamina for.
   * @param interaction The interaction that causes stamina deduction.
   */
  bool DeductStamina(Character* lockedCharacter, Interaction* interaction);
  /**
   * Calculates the attacking character's chance of hitting.
   * @param lockedAttacker The locked version of the attacking character.
   * @param lockedTarget The locked version of the target character.
   * @return attack chance.
   */
  unsigned int GetAttackChance(Character* lockedAttacker, Character* lockedTarget);
  /**
   * Calculates the damage made.
   * @param lockedAttacker The locked version of the attacking character.
   * @param lockedTarget The locked version of the target character.
   * @return attack chance.
   */
  int CalculateDamage(Character* lockedAttacker, Character* lockedTarget);
  /**
   * Used to direct an interaction to the correct handler.
   * @param interaction The interaction.
   * @return True if the interaction was handled.
   */
  bool PerformInteraction(Interaction* interaction);
  /**
   * Checks that target is within reach and "legal" target.
   * @param lockedAttacker The locked version of the attacking character.
   * @param lockedTarget The locked version of the target character.
   * @return true if attackable, otherwise false.
   */
  bool TargetAttackable(Character* lockedAttacker, Character* lockedTarget);
  /**
   * Finds the target marked for the given character.
   * @param lockedCharacter The locked version of the attacking character.
   * @return Unlocked version of the target character, or NULL if none.
   */
  const Character* GetTargetCharacter(Character* lockedCharacter);
  /**
   * Returns the combined block for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters block.
   */
  unsigned int GetBlock(Character* lockedCharacter);
  /**
   * Returns the combined dodge for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters dodge.
   */
  unsigned int GetDodge(Character* lockedCharacter);
  /**
   * Returns the combined parry for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters parry.
   */
  unsigned int GetParry(Character* lockedCharacter);
  /**
   * Returns the combined strength for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters strength.
   */
  unsigned int GetStrength(Character* lockedCharacter);
  /**
   * Returns the combined reach for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters reach.
   */
  unsigned int GetReach(Character* lockedCharacter);
  /**
   * Returns the combined agility for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters agility.
   */
  unsigned int GetAgility(Character* lockedCharacter);
  /**
   * Returns the combined skill bonuses.
   * @todo
   * @param lockedCharacter The locked version of the character.
   * @return The characters skill bonuses.
   */
  unsigned int GetSkillBonus(Character* lockedCharacter);
  /**
   * Returns the combined sapience for a character, including bonuses.
   * @param lockedCharacter The locked version of the character.
   * @return The characters sapience.
   */
  unsigned int GetSapience(Character* lockedCharacter);
  /**
   * Function to get the weapon(s) heft value.
   * @param lockedCharacter The owner of the weapon
   * @return The weapon(s) heft value.
   */
  unsigned int GetWeaponHeft(Character* lockedCharacter);
  /**
   * Calculates the damage given by the character's weapon(s).
   * @param lockedCharacter The locked version of the character.
   * @return The weapon damage.
   */
  unsigned int GetWeaponDamage(Character* lockedCharacter);
  /**
   * Combines the characters weapon stat values, based on stat name
   * @param lockedCharacter The locked version of the character.
   * @param statName The stat's name.
   * @return The weapons bonuses given a special stat.
   */
  unsigned int GetStatValueForEquipedWeapons(Character* lockedCharacter,
                                      const char* statName);
  /**
   * Get a random number between 0 and 100.
   * @return A random number between 0 and 100.
   */
  unsigned int RollDice(void);
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
   * Used to report the death to nearby characters.
   * @param lockedCharacter The locked character for which to report death.
   * @return None.
   */
  void ReportDeath(Character* lockedCharacter);
  /**
   * Drops all items from the given character.
   * @param lockedCharacter The locked version of the character.
   * @return None.
   */
  void DropAllItems(Character* lockedCharacter);
  /**
   * Reports the damage to all nearby characters.
   * @param lockedCharacter The locked version of the character.
   * @return None.
   */
  void ReportDamage(Character* lockedCharacter);
  /**
   * Sets the distance for which to report nearby events.
   * @param distance The distance to report events for.
   * @return None.
   */
  void SetNotificationDistance(unsigned int distance);
  /**
   * Gives the distance to report nearby interactions.
   * @return The distance to report neraby interactions.
   */
  unsigned int GetNotificationDistance();

};

namespace InteractionManagerSendTo
{
  enum SendTo
  {
    CHARACTER,
    LOCALCAST,
    BROADCAST,
  };
}

#endif //INTERACTIONMANAGER
