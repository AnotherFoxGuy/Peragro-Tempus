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
#include "skillmanager.h"
#include "interactionutility.h"

#ifdef WIN32
  #include <string.h>
  #define strncasecmp _strnicmp
  #define strcasecmp _stricmp
#endif

#define RETURN_AGILITY_IF(skillType, name) \
  if (name == skillType) { \
    return "Agility";       \
  }

SkillManager::SkillManager()
{
}

SkillManager::~SkillManager()
{
}

void SkillManager::CheckDeprecation(boost::shared_ptr<Character> character,
                               const std::string& skill)
{
  float timeUnused = 0.0f;
  timeUnused = GetTimeSkillUnused(character, skill);
}

float SkillManager::GetTimeSkillUnused(boost::shared_ptr<Character> character,
                                 const std::string& skill)
{
  return GetTimeUnusedLastLogin(character, skill) +
         GetTimeUnusedThisLogin(character, skill);
}

void SkillManager::CharacterLogout(boost::shared_ptr<Character> character,
                              const std::string& skill)
{
  UpdateTimeUnused(character, skill);
}

void SkillManager::ResetTimeSkillLastUsed(boost::shared_ptr<Character> character,
                                     const std::string& skill)
{
}

float SkillManager::GetTimeUnusedThisLogin(boost::shared_ptr<Character> character,
                                     const std::string& skill)
{
  return 0.0f;
}

float SkillManager::GetTimeUnusedLastLogin(boost::shared_ptr<Character> character,
                                     const std::string& skill)
{
  const std::string& strSkillLastUsed = GetSkillLastUsedStr(skill);

  return InteractionUtility::GetStatValue(character, strSkillLastUsed) +
         InteractionUtility::GetStatValueForAllEquipedItems(character,
                                                            strSkillLastUsed);
}

std::string SkillManager::GetSkillLastUsedStr(const std::string& skill)
{
  // TODO add here or do in function directly? Need to free memory...
  return "";
}


// Caller must free string.
std::string SkillManager::GetAbilityNameForSkill(const std::string& skillType)
{
  RETURN_AGILITY_IF(skillType, "OneHandedAxe")
  RETURN_AGILITY_IF(skillType, "OneHandedBlunt")
  RETURN_AGILITY_IF(skillType, "OneHandedFlail")
  RETURN_AGILITY_IF(skillType, "OneHandedSword")
  RETURN_AGILITY_IF(skillType, "OneHandedUnarmed")
  RETURN_AGILITY_IF(skillType, "TwoHandedAxe")
  RETURN_AGILITY_IF(skillType, "TwoHandedBlunt")
  RETURN_AGILITY_IF(skillType, "TwoHandedPolearm")
  RETURN_AGILITY_IF(skillType, "TwoHandedSword")
  RETURN_AGILITY_IF(skillType, "RangedBow")
  RETURN_AGILITY_IF(skillType, "RangedCrossBow")
  RETURN_AGILITY_IF(skillType, "RangedThrown")

  printf("BUG: Should not be here!!");
  return "";
}


