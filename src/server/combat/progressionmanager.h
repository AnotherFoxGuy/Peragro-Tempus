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
             const char* skillKnowledge,
             const char* ability);
  unsigned int GetPenalty(Character* lockedCharacter, const char* attackType);
  unsigned int GetSkillLevel(Character* lockedCharacter, const char* skillType);
  unsigned int GetSkillOrAbilityXP(Character* lockedCharacter,
                                   const char* skillType);
  void IncreaseAbilityXP(Character* lockedCharacter,
                          const char* abilityName,
                          int increase);
  void IncreaseSkillXP(Character* lockedCharacter,
                          const char* skillName,
                          int increase);
  void IncreaseSpecialityXP(Character* lockedCharacter,
                          const char* specialityName,
                          int increase);
  void CheckAbilityProgress(Character* lockedCharacter,
                            const char* abilityName);
    
};

#endif //PROGRESSIONMANAGER
