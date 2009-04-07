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
 * @file interactionutility.h
 *
 * @basic Utility for interaction system.
 */

#ifndef ABILITYHELPER_H
#define ABILITYHELPER_H

#include "src/server/entity/pcentity.h"

/**
 * Ability helper
 */
class AbilityHelper
{
private:
public:
  /**
   * Queries the ability for a character.
   * It would be tempting to use this function directly,however a character can
   * get bonuses, therefore this function should be called from the different
   * getters for abilities.
   * @param character The locked version of the character.
   * @param abilityName The ability's name.
   * @return The ability current value.
   */
  static unsigned int GetAbilityLevel(boost::shared_ptr<Character> character,
                                      const std::string& abilityName);
  /**
   * Combines the character's items ability values, based on ability name
   * @param character The locked version of the character.
   * @param abilityName The ability's name.
   * @return The items bonuses given a special ability.
   */
  static unsigned int GetAbilityLevelForAllEquipedItems(boost::shared_ptr<Character> character,
                                                      const std::string& abilityName);
  /**
   * Queries the ability value from an item based on ability name.
   * @param item The item to return the ability value for.
   * @param abilityName The ability's name.
   * @return The items ability based on the abilityName.
   */
  static unsigned int GetAbilityLevelForItem(boost::shared_ptr<ItemEntity> item, const std::string& abilityName);
};

#endif // ABILITYHELPER_H
