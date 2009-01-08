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

unsigned int
ProgressionManager::CalculateExperienceGain(Character* lockedAttacker,
                                            Character* lockedTarget,
                                            const char* skillType,
                                            const char* attackType)
{
  float successChance = 0;
  float failureChance = 0;
  float randomNumber = 0;
  float penalty = 0;
  float skill = 0;
  float agility = 0;

  penalty = GetPenalty(lockedTarget, attackType);
  skill = GetSkill(lockedAttacker, skillType);
  agility = InteractionUtility::GetStatValue(lockedAttacker, "agility");

  successChance = skill * agility - penalty;

  failureChance = 100 - successChance;

  if (randomNumber < successChance) {
    if (randomNumber >= (successChance - (failureChance * 0.1))) {
      AddXP();
    } 
  } else if (randomNumber > successChance) {
    if (randomNumber <= successChance + (failureChance * 0.1)) {
      AddXP();
    }
  } else if (randomNumber == successChance) {
    AddXP();
  }
}

void
ProgressionManager::AddXP(unsigned int skillKnowledge,
                          unsigned int ability)
{
  // TODO ability skill that isn't right....
  if (skill > ability) {
    IncreaseExperience("ability", 1);
  } else {
    IncreaseExperience("skill", 1);
  }

  IncreaseExperience(ProgressionManager::SPECIALITY, 1);
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
    IncreaseExperience(Progressionmanager::Endurance, progression);
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
    IncreaseExperience(Progressionmanager::Endurance, progression);
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
    IncreaseExperience(ProgressionManager::Resolve, progression);
  }
}

unsigned int 
ProgressManager::RollDice(unsigned int lower, unsigned int higher)
{
  return (rand() % (higher - lower + 1)) + lower;
}

float
ProgressManager::GetPenalty(Character* lockedCharacter,
                            const char* attackType)
{
}

float
ProgressManager::GetSkill(Character* lockedCharacter,
                          const char* skillType)
{
}

