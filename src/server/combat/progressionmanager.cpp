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

#include "server/entity/entitymanager.h"
#include "server/entity/statmanager.h"
#include "interactionutility.h"
#include "progressionmanager.h"

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
  unsigned int skill = 0;
  unsigned int agility = 0;

  penalty = GetPenalty(lockedTarget, attackType);
  skill = GetSkill(lockedAttacker, skillType);
  agility = (unsigned int) InteractionUtility::GetStatValue(lockedAttacker,
                                                            "agility");

  successChance = skill * agility - penalty;

  failureChance = 100 - successChance;

  if (randomNumber < successChance) {
    if (randomNumber >= (successChance - (failureChance * 0.1))) {
      AddXP(lockedAttacker, skill, agility);
    } 
  } else if (randomNumber > successChance) {
    if (randomNumber <= successChance + (failureChance * 0.1)) {
      AddXP(lockedAttacker, skill, agility);
    }
  } else if (randomNumber == successChance) {
    AddXP(lockedAttacker, skill, agility);
  }
}

void
ProgressionManager::AddXP(Character* lockedCharacter,
                          unsigned int skillKnowledge,
                          unsigned int ability)
{
  // TODO ability skill that isn't right....
  if (skillKnowledge > ability) {
    IncreaseExperience(lockedCharacter, "ability", 1);
  } else {
    IncreaseExperience(lockedCharacter, "skill", 1);
    IncreaseExperience(lockedCharacter, "speciality", 1);
  }

}

void
ProgressionManager::IncreaseExperience(Character* lockedCharacter,
                                       const char* stat,
                                       int difference)
{
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
    IncreaseExperience(lockedCharacter, "endurance", progression);
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
    IncreaseExperience(lockedCharacter, "endurance", progression);
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
    IncreaseExperience(lockedCharacter, "resolve", progression);
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
ProgressionManager::GetSkill(Character* lockedCharacter,
                          const char* skillType)
{
  return 0;
}

