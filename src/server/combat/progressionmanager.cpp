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
void ProgressionManager::CalculateExperienceGain(boost::shared_ptr<Character> attacker,
                                            boost::shared_ptr<Character> target,
                                            const std::string& skillType,
                                            const std::string& attackType)
{
  unsigned int successChance = 0;
  unsigned int failureChance = 0;
  unsigned int randomNumber = 0;
  unsigned int penalty = 0;
  unsigned int skillLevel = 0;
  unsigned int agilityLevel = 0;
  std::string  abilityName = "Agility";

  penalty = GetPenalty(target, attackType);
  skillLevel = GetSkillLevel(attacker, skillType);

  abilityName = SkillManager::GetAbilityNameForSkill(skillType);

  agilityLevel = (unsigned int) InteractionUtility::GetStatValue(attacker,
                                                                 abilityName);

  // Base chance for success should be 50%
  // http://wiki.peragro.org/index.php/Skill
  successChance = skillLevel * agilityLevel - penalty + 50;

  failureChance = 100 - successChance;

  if (randomNumber < successChance) {
    if (randomNumber >= (successChance - (failureChance / 10))) {
      AddXP(attacker, skillType, abilityName);
    }
  } else if (randomNumber > successChance) {
    if (randomNumber <= successChance + (failureChance / 10)) {
      AddXP(attacker, skillType, abilityName);
    }
  } else if (randomNumber == successChance) {
    AddXP(attacker, skillType, abilityName);
  }
}

/* See: http://wiki.peragro.org/index.php/Experience_Implementation */
void ProgressionManager::AddXP(boost::shared_ptr<Character> lockedCharacter,
                          const std::string& skillName,
                          const std::string& abilityName)
{
  unsigned int skillXP = 0;
  unsigned int abilityXP = 0;
/*
  skillXP = GetSkillOrAbilityXP(lockedCharacter, skillName);
  abilityXP = GetSkillOrAbilityXP(lockedCharacter, abilityName);

  // TODO ability skill that isn't right....
  if (skillXP > abilityXP) {
    IncreaseAbilityXP(lockedCharacter, abilityName, 1);
  } else {
    IncreaseSkillXP(lockedCharacter, skillName, 1);
    IncreaseSpecialityXP(lockedCharacter, "speciality", 1);
  }
*/
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
ProgressionManager::CheckAbilityProgress(boost::shared_ptr<Character> lockedCharacter,
                                         const std::string& abilityName)
{
  const std::string& abilityXPStr = InteractionUtility::GetXPString(abilityName);
  const std::string& abilityLevelStr = InteractionUtility::GetXPString(abilityName);
  unsigned int neededXP = 0;
  unsigned int currentLevel = 0;
  unsigned int currentXP = 0;

  neededXP = GetXPNeededForNextAbilityLevel(currentLevel);
  currentXP = InteractionUtility::GetStatValue(lockedCharacter, abilityXPStr);
  currentLevel = InteractionUtility::GetStatValue(lockedCharacter,
                                                  abilityLevelStr);

  if (currentXP >= neededXP) 
  {
    InteractionUtility::IncreaseStatValue(lockedCharacter, abilityLevelStr,
                                          currentLevel + 1);
  }
}


void ProgressionManager::IncreaseAbilityXP(boost::shared_ptr<Character> lockedCharacter,
                                      const std::string& abilityName,
                                      int increase)
{
  const std::string abilityXPStr = InteractionUtility::GetXPString(abilityName);
  InteractionUtility::IncreaseStatValue(lockedCharacter, abilityXPStr, increase);
  CheckAbilityProgress(lockedCharacter, abilityName);
}

void ProgressionManager::IncreaseSkillXP(boost::shared_ptr<Character> lockedCharacter,
                                      const std::string& skillName,
                                      int increase)
{
  const std::string skillXPStr = InteractionUtility::GetXPString(skillName);
  InteractionUtility::IncreaseStatValue(lockedCharacter, skillXPStr, increase);
  CheckAbilityProgress(lockedCharacter, skillName);
}

void ProgressionManager::IncreaseSpecialityXP(boost::shared_ptr<Character> lockedCharacter,
                                      const std::string& specialityName,
                                      int increase)
{
  const std::string specialityXPStr = InteractionUtility::GetXPString(specialityName);
  InteractionUtility::IncreaseStatValue(lockedCharacter, specialityXPStr, increase);
  CheckAbilityProgress(lockedCharacter, specialityName);
}

void ProgressionManager::HPIncreased(boost::shared_ptr<Character> lockedCharacter,
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
  if (progression) 
  {
    IncreaseAbilityXP(lockedCharacter, "endurance", progression);
  }
}

void ProgressionManager::StaminaIncreased(boost::shared_ptr<Character> lockedCharacter,
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
  if (progression) 
  {
    IncreaseAbilityXP(lockedCharacter, "endurance", progression);
  }
}

void ProgressionManager::WillPowerIncreased(boost::shared_ptr<Character> lockedCharacter,
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
  if (progression) 
  {
    IncreaseAbilityXP(lockedCharacter, "resolve", progression);
  }
}

unsigned int ProgressionManager::RollDice(unsigned int lower, unsigned int higher)
{
  return (rand() % (higher - lower + 1)) + lower;
}

unsigned int ProgressionManager::GetPenalty(boost::shared_ptr<Character> lockedCharacter,
                               const std::string& attackType)
{
  return 0;
}

unsigned int ProgressionManager::GetSkillLevel(boost::shared_ptr<Character> lockedCharacter,
                                  const std::string& skillType)
{
  return (unsigned int) InteractionUtility::GetStatValue(lockedCharacter,
                                                         skillType);
}
