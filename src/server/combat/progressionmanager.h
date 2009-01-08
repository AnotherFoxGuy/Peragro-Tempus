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
 * @file progressionmanager.h
 *
 * @basic Server progression manager.
 */

#ifndef PROGRESSIONMANAGER
#define PROGRESSIONMANAGER

//#include "src/server/entity/pcentity.h"

/**
 * Server progression manager.
 */
class ProgressionManager
{
public:
  /// Constructor.
  ProgressionManager();
  /// Destructor.
  ~ProgressionManager();

  void CalculateExperienceGain(Character* lockedAttacker,
                               Character* lockedTarget,
                               const char* skillType,
                               const char* attackType);
  void HPIncreased(Character* lockedCharacter, unsigned int amount);
  void StaminaIncreased(Character* lockedCharacter, unsigned int amount);
  void WillPowerIncreased(Character* lockedCharacter, unsigned int amount);
  unsigned int RollDice(unsigned int lower, unsigned int higher);

private:
  void AddXP(Character* lockedCharacter,
             unsigned int skillKnowledge,
             unsigned int ability);
  unsigned int GetPenalty(Character* lockedCharacter, const char* attackType);
  unsigned int GetSkill(Character* lockedCharacter, const char* skillType);
  void IncreaseExperience(Character* lockedCharacter,
                          const char* stat,
                          int difference);
    
};

#endif //PROGRESSIONMANAGER
