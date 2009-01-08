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

#ifndef INTERACTIONUTILITY
#define INTERACTIONUTILITY

#include "src/server/entity/pcentity.h"

/**
 * Interaction utility
 */
class InteractionUtility
{
private:
public:
  /**
   * Queries the stat for a character based on the stat's name.
   * It would be tempting to use this function directly,however a character can
   * get bonuses, therefore this function should be called from the different
   * getters for stats.
   * @param lockedCharacter The locked version of the character.
   * @param statName The stat's name.
   * @return The stats current value.
   */
  static unsigned int GetStatValue(Character* lockedCharacter,
                                   const char* statName);
  /**
   * Combines the character's items stat values, based on stat name
   * @param lockedCharacter The locked version of the character.
   * @param statName The stat's name.
   * @return The items bonuses given a special stat.
   */
  static unsigned int GetStatValueForAllEquipedItems(Character* lockedCharacter,
                                                     const char* statName);
  /**
   * Used to get an equiped item based on the slot number.
   * @param lockedCharacter The locked version of the character.
   * @param slot Which item, is decided by the slot its equiped on.
   * @return The item in that slot, or NULL if none found.
   */
  static Item* GetItem(Character* lockedCharacter, unsigned char slot);
  /**
   * Queries the stat value from an item based on stat name.
   * @param item The item to return the stat value for.
   * @param statName The stat's name.
   * @return The items stat based on the statName.
   */
  static unsigned int GetStatValueForItem(const Item* item, const char* statName);
};

#endif // INTERACTIONUTILITY
