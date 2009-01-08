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
#include "skillmanager.h"
#include "interactionutility.h"

SkillManager::SkillManager()
{
}

SkillManager::~SkillManager()
{
}

void
SkillManager::CheckDeprecation(Character* lockedCharacter,
                               const char* skill)
{
  float timeUnused = 0.0f;
  timeUnused = GetTimeSkillUnused(lockedCharacter, skill);
}

float
SkillManager::GetTimeSkillUnused(Character* lockedCharacter, 
                                 const char* skill)
{
  return GetTimeUnusedLastLogin(lockedCharacter, skill) + 
         GetTimeUnusedThisLogin(lockedCharacter, skill);
}

void
SkillManager::CharacterLogout(Character* lockedCharacter,
                              const char* skill)
{
  UpdateTimeUnused(lockedCharacter, skill);
}

void
SkillManager::ResetTimeSkillLastUsed(Character* lockedCharacter,
                                     const char* skill)
{
}

float
SkillManager::GetTimeUnusedThisLogin(Character* lockedCharacter,
                                     const char* skill)
{
  return 0.0f;
}

float
SkillManager::GetTimeUnusedLastLogin(Character* lockedCharacter,
                                     const char* skill)
{
  const char* strSkillLastUsed = GetSkillLastUsedStr(skill);

  return InteractionUtility::GetStatValue(lockedCharacter, strSkillLastUsed) +
         InteractionUtility::GetStatValueForAllEquipedItems(lockedCharacter,
                                                            strSkillLastUsed);
}

const char*
SkillManager::GetSkillLastUsedStr(const char* skill)
{
  // TODO add here or do in function directly? Need to free memory...
  return NULL;

}

#define RETURN_AGILITY_IF(skillType, name) \
  if (strncasecmp(name, skillType, strlen(name)) == 0) { \
    return InteractionUtility::GetAgilityString();       \
  }

// Caller must free string.
const char*
SkillManager::GetAbilityNameForSkill(const char* skillType)
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
  return NULL;
}


