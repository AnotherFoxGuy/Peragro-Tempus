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

#ifndef SKILLHELPER_H
#define SKILLHELPER_H

#include "src/server/entity/pcentity.h"

/**
 * Skill helper
 */
class SkillHelper
{
private:
public:
  /**
   * Queries the skill for a character.
   * It would be tempting to use this function directly,however a character can
   * get bonuses, therefore this function should be called from the different
   * getters for abilities.
   * @param character The locked version of the character.
   * @param skillName The skill's name.
   * @return The skill current value.
   */
  static unsigned int GetSkillLevel(boost::shared_ptr<Character> character,
                                      const std::string& skillName);
  /**
   * Combines the character's items skill values, based on skill name
   * @param character The locked version of the character.
   * @param skillName The skill's name.
   * @return The items bonuses given a special skill.
   */
  static unsigned int GetSkillLevelForAllEquipedItems(boost::shared_ptr<Character> character,
                                                      const std::string& skillName);
  /**
   * Queries the skill value from an item based on skill name.
   * @param item The item to return the skill value for.
   * @param skillName The skill's name.
   * @return The items skill based on the skillName.
   */
  static unsigned int GetSkillLevelForItem(boost::shared_ptr<ItemEntity> item, const std::string& skillName);
};

#endif // SKILLHELPER_H
