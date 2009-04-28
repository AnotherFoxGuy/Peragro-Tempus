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
#include "abilityhelper.h"
#include "skillhelper.h"
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
  skillLevel = SkillHelper::GetSkillLevel(attacker, skillType);

  abilityName = SkillManager::GetAbilityNameForSkill(skillType);

  agilityLevel = AbilityHelper::GetAbilityLevel(attacker, abilityName);

  // Base chance for success should be 50%
  // http://wiki.peragro.org/index.php/Skill
  successChance = skillLevel * agilityLevel - penalty + 50;

  failureChance = 100 - successChance;

  if (randomNumber < successChance)
  {
    if (randomNumber >= (successChance - (failureChance / 10)))
    {
      AddXP(attacker, skillType, abilityName);
    }
  }
  else if (randomNumber > successChance)
  {
    if (randomNumber <= successChance + (failureChance / 10))
    {
      AddXP(attacker, skillType, abilityName);
    }
  }
  else if (randomNumber == successChance)
  {
    AddXP(attacker, skillType, abilityName);
  }
}

/* See: http://wiki.peragro.org/index.php/Experience_Implementation */
void ProgressionManager::AddXP(boost::shared_ptr<Character> character,
                          const std::string& skillName,
                          const std::string& abilityName)
{
  unsigned int skillXP = 0;
  unsigned int abilityXP = 0;

  skillXP = character->GetSkills()->Get(skillName);
  abilityXP = character->GetAbilities()->Get(abilityName);

  // TODO ability skill that isn't right....
  if (skillXP > abilityXP)
  {
    character->GetAbilities()->Add(abilityName, 1);
  }
  else
  {
    character->GetSkills()->Add(skillName, 1);
    //IncreaseSpecialityXP(character, "speciality", 1);
  }
}

unsigned int ProgressionManager::XPIncrease(unsigned int pointsExpended, unsigned int chance)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (pointsExpended > 0)
  {
    randomNumber = RollDice(1,100);
    if (randomNumber <= chance)
      progression++;
    pointsExpended--;
  }
  return progression;
}

unsigned int ProgressionManager::RollDice(unsigned int lower, unsigned int higher)
{
  return (rand() % (higher - lower + 1)) + lower;
}

bool ProgressionManager::RollCheck(unsigned int chance)
{
  return RollDice(1,100) <= chance;
}

unsigned int ProgressionManager::GetPenalty(boost::shared_ptr<Character> character,
                               const std::string& attackType)
{
  return 0;
}