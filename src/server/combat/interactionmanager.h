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
  bool SelectTarget(boost::shared_ptr<PcEntity> sourceEntity, unsigned int tagetID);
  /**
   * Selects new target for a given character.
   * @param sourceEntity entity for the character that request target selection.
   * @param interactionID the action ID for the interaction.
   * @return True upon sucess, otherwise false.
   */
  bool QueueInteraction(boost::shared_ptr<PcEntity> sourceEntity,
                        unsigned int interactionID);

  void shutdown();

protected:
  /**
   * Function popping interactions of the head of the interaction queue.
   * @return None.
   */
  void Run();


private:
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
   * @param character The character to deduct stamina for.
   * @param interaction The interaction that causes stamina deduction.
   */
  bool DeductStamina(boost::shared_ptr<Character> character, Interaction* interaction);
  /**
   * Calculates the attacking character's chance of hitting.
   * @param attacker The locked version of the attacking character.
   * @param target The locked version of the target character.
   * @return attack chance.
   */
  unsigned int GetAttackChance(boost::shared_ptr<Character> attacker, boost::shared_ptr<Character> target);
  /**
   * Calculates the damage made.
   * @param attacker The locked version of the attacking character.
   * @param target The locked version of the target character.
   * @return attack chance.
   */
  int CalculateDamage(boost::shared_ptr<Character> attacker, boost::shared_ptr<Character> target);
  /**
   * Used to direct an interaction to the correct handler.
   * @param interaction The interaction.
   * @return True if the interaction was handled.
   */
  bool PerformInteraction(Interaction* interaction);
  /**
   * Checks that target is within reach and "legal" target.
   * @param attacker The locked version of the attacking character.
   * @param target The locked version of the target character.
   * @return true if attackable, otherwise false.
   */
  bool TargetAttackable(boost::shared_ptr<Character> attacker, boost::shared_ptr<Character> target);
  /**
   * Finds the target marked for the given character.
   * @param character The locked version of the attacking character.
   * @return The target character, or NULL if none.
   */
  boost::shared_ptr<Character> GetTargetCharacter(boost::shared_ptr<Character> character);
  /**
   * Returns the combined block for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters block.
   */
  unsigned int GetBlock(boost::shared_ptr<Character> character);
  /**
   * Returns the combined dodge for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters dodge.
   */
  unsigned int GetDodge(boost::shared_ptr<Character> character);
  /**
   * Returns the combined parry for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters parry.
   */
  unsigned int GetParry(boost::shared_ptr<Character> character);
  /**
   * Returns the combined strength for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters strength.
   */
  unsigned int GetStrength(boost::shared_ptr<Character> character);
  /**
   * Returns the combined reach for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters reach.
   */
  unsigned int GetReach(boost::shared_ptr<Character> character);
  /**
   * Returns the combined agility for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters agility.
   */
  unsigned int GetAgility(boost::shared_ptr<Character> character);
  /**
   * Returns the combined skill bonuses.
   * @todo
   * @param character The locked version of the character.
   * @return The characters skill bonuses.
   */
  unsigned int GetSkillBonus(boost::shared_ptr<Character> character);
  /**
   * Returns the combined sapience for a character, including bonuses.
   * @param character The locked version of the character.
   * @return The characters sapience.
   */
  unsigned int GetSapience(boost::shared_ptr<Character> character);
  /**
   * Function to get the weapon(s) heft value.
   * @param character The owner of the weapon
   * @return The weapon(s) heft value.
   */
  unsigned int GetWeaponHeft(boost::shared_ptr<Character> character);
  /**
   * Calculates the damage given by the character's weapon(s).
   * @param character The locked version of the character.
   * @return The weapon damage.
   */
  unsigned int GetWeaponDamage(boost::shared_ptr<Character> character);
  /**
   * Combines the characters weapon stat values, based on stat name
   * @param character The locked version of the character.
   * @param statName The stat's name.
   * @return The weapons bonuses given a special stat.
   */
  unsigned int GetStatValueForEquipedWeapons(boost::shared_ptr<Character> lockedCharacter,
                                            const std::string& statName);
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
/*
  void SendStatUpdate(const Stat* stat, const CharacterStats* stats,
                      Character* lockedCharacter, const char* name, int target);
*/

  /**
   * Used to report the death to nearby characters.
   * @param character The locked character for which to report death.
   * @return None.
   */
  void ReportDeath(boost::shared_ptr<Character> character);
  /**
   * Drops all items from the given character.
   * @param character The locked version of the character.
   * @return None.
   */
  void DropAllItems(boost::shared_ptr<Character> character);
  /**
   * Reports the damage to all nearby characters.
   * @param character The locked version of the character.
   * @return None.
   */
  void ReportDamage(boost::shared_ptr<Character> character);
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
