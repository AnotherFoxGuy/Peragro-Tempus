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

#include <stdlib.h>
#include "server/entity/entitymanager.h"
#include "interactionutility.h"
#include "progressionmanager.h"
#include "skillmanager.h"

/* See: http://wiki.peragro.org/index.php/Experience_Implementation */
void
ProgressionManager::CalculateExperienceGain(Character* lockedAttacker,
                                            Character* lockedTarget,
                                            const char* skillType,
                                            const char* attackType)
{
  unsigned int successChance = 0;
  unsigned int failureChance = 0;
  unsigned int randomNumber = 0;
  unsigned int penalty = 0;
  unsigned int skillLevel = 0;
  unsigned int agilityLevel = 0;
  const char*  abilityName = "Agility";

  penalty = GetPenalty(lockedTarget, attackType);
  skillLevel = GetSkillLevel(lockedAttacker, skillType);

  abilityName = SkillManager::GetAbilityNameForSkill(skillType);

  agilityLevel = (unsigned int) InteractionUtility::GetStatValue(lockedAttacker,
                                                                 abilityName);

  // Base chance for success should be 50%
  // http://wiki.peragro.org/index.php/Skill
  successChance = skillLevel * agilityLevel - penalty + 50;

  failureChance = 100 - successChance;

  if (randomNumber < successChance) {
    if (randomNumber >= (successChance - (failureChance / 10))) {
      AddXP(lockedAttacker, skillType, abilityName);
    }
  } else if (randomNumber > successChance) {
    if (randomNumber <= successChance + (failureChance / 10)) {
      AddXP(lockedAttacker, skillType, abilityName);
    }
  } else if (randomNumber == successChance) {
    AddXP(lockedAttacker, skillType, abilityName);
  }
  delete [] abilityName;
}

/* See: http://wiki.peragro.org/index.php/Experience_Implementation */
void
ProgressionManager::AddXP(Character* lockedCharacter,
                          const char* skillName,
                          const char* abilityName)
{
  unsigned int skillXP = 0;
  unsigned int abilityXP = 0;

  skillXP = GetSkillOrAbilityXP(lockedCharacter, skillName);
  abilityXP = GetSkillOrAbilityXP(lockedCharacter, abilityName);

  // TODO ability skill that isn't right....
  if (skillXP > abilityXP) {
    IncreaseAbilityXP(lockedCharacter, abilityName, 1);
  } else {
    IncreaseSkillXP(lockedCharacter, skillName, 1);
    IncreaseSpecialityXP(lockedCharacter, "speciality", 1);
  }

}

/* See: http://wiki.peragro.org/index.php/Improvement_Implementation */
unsigned int
GetXPNeededForNextAbilityLevel(unsigned int currentLevel)
{
  unsigned int requiredXP = 0;

  for (unsigned int i = 1; i <= currentLevel + 1; i++) {
    requiredXP += 10 * i;
  }
  return requiredXP;
}

/* See: http://wiki.peragro.org/index.php/Improvement_Implementation */
unsigned int
GetXPNeededForNextSkillLevel(unsigned int currentLevel)
{
  unsigned int requiredXP = 0;

  for (unsigned int i = 1; i <= currentLevel + 1; i++) {
    requiredXP += i;
  }
  return requiredXP;
}

/* See: http://wiki.peragro.org/index.php/Improvement_Implementation */
unsigned int
GetXPNeededForNextSpecialityLevel(unsigned int currentLevel)
{
  unsigned int requiredXP = 0;

  for (unsigned int i = 1; i <= currentLevel + 1; i++) {
    requiredXP += 10 * i;
  }
  return requiredXP;
}
void
ProgressionManager::CheckAbilityProgress(Character* lockedCharacter,
                                         const char* abilityName)
{
  const char* abilityXPStr = InteractionUtility::GetXPString(abilityName);
  const char* abilityLevelStr = InteractionUtility::GetXPString(abilityName);
  unsigned int neededXP = 0;
  unsigned int currentLevel = 0;
  unsigned int currentXP = 0;

  neededXP = GetXPNeededForNextAbilityLevel(currentLevel);
  currentXP = InteractionUtility::GetStatValue(lockedCharacter, abilityXPStr);
  currentLevel = InteractionUtility::GetStatValue(lockedCharacter,
                                                  abilityLevelStr);

  if (currentXP >= neededXP) {
    InteractionUtility::IncreaseStatValue(lockedCharacter, abilityLevelStr,
                                          currentLevel + 1);
  }

  delete [] abilityXPStr;
  delete [] abilityLevelStr;
}


void
ProgressionManager::IncreaseAbilityXP(Character* lockedCharacter,
                                      const char* abilityName,
                                      int increase)
{
  const char* abilityXPStr = InteractionUtility::GetXPString(abilityName);
  InteractionUtility::IncreaseStatValue(lockedCharacter, abilityXPStr, increase);
  delete [] abilityXPStr;
  CheckAbilityProgress(lockedCharacter, abilityName);
}

void
ProgressionManager::IncreaseSkillXP(Character* lockedCharacter,
                                      const char* skillName,
                                      int increase)
{
  const char* skillXPStr = InteractionUtility::GetXPString(skillName);
  InteractionUtility::IncreaseStatValue(lockedCharacter, skillXPStr, increase);
  delete [] skillXPStr;
  CheckAbilityProgress(lockedCharacter, skillName);
}

void
ProgressionManager::IncreaseSpecialityXP(Character* lockedCharacter,
                                      const char* specialityName,
                                      int increase)
{
  const char* specialityXPStr = InteractionUtility::GetXPString(specialityName);
  InteractionUtility::IncreaseStatValue(lockedCharacter, specialityXPStr, increase);
  delete [] specialityXPStr;
  CheckAbilityProgress(lockedCharacter, specialityName);
}

void
ProgressionManager::HPIncreased(Character* lockedCharacter,
                                unsigned int amount)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (amount > 0) {
    randomNumber = RollDice(1,100);
    if (randomNumber <= 10) {
      progression++;
    }
  }
  if (progression) {
    IncreaseAbilityXP(lockedCharacter, "endurance", progression);
  }
}

void
ProgressionManager::StaminaIncreased(Character* lockedCharacter,
                                     unsigned int amount)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (amount > 0) {
    randomNumber = RollDice(1,100);
    if (randomNumber == 1) {
      progression++;
    }
  }
  if (progression) {
    IncreaseAbilityXP(lockedCharacter, "endurance", progression);
  }
}

void
ProgressionManager::WillPowerIncreased(Character* lockedCharacter,
                                       unsigned int amount)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (amount > 0) {
    randomNumber = RollDice(1,100);
    if (randomNumber == 1) {
      progression++;
    }
  }
  if (progression) {
    IncreaseAbilityXP(lockedCharacter, "resolve", progression);
  }
}

unsigned int
ProgressionManager::RollDice(unsigned int lower, unsigned int higher)
{
  return (rand() % (higher - lower + 1)) + lower;
}

unsigned int
ProgressionManager::GetPenalty(Character* lockedCharacter,
                               const char* attackType)
{
  return 0;
}

unsigned int
ProgressionManager::GetSkillLevel(Character* lockedCharacter,
                                  const char* skillType)
{
  return (unsigned int) InteractionUtility::GetStatValue(lockedCharacter,
                                                         skillType);
}

unsigned int
ProgressionManager::GetSkillOrAbilityXP(Character* lockedCharacter,
                                        const char* type)
{
  size_t length = strlen(type) + strlen("XP") + 1;
  char* str = new char[length];
  unsigned int xp = 0;
  strncat(str, type, length);
  strncat(str, "XP", length);
  xp = (unsigned int) InteractionUtility::GetStatValue(lockedCharacter, str);
  delete [] str;
  return xp;
}

