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
 * @file skillmanager.h
 *
 * @basic Server skill manager.
 */

#ifndef SKILLMANAGER
#define SKILLMANAGER

//#include "src/server/entity/pcentity.h"

/**
 * Server skill manager.
 */
class SkillManager
{
public:
  /// Constructor.
  SkillManager();
  /// Destructor.
  ~SkillManager();

  void CheckDeprecation(Character* lockedCharacter,
                        const char* skill);
  float GetTimeSkillUnused(Character* lockedCharacter,
                           const char* skill);
  void CharacterLogou(Character* lockedCharacter,
                      const char* skill);
  void ResetTimeSkillLastUsed(Character* lockedCharacter, 
                              const char* skill);
  float GetTimeUnusedLastLogin(Character *lockedCharacter,
                               const char* skill);
  float GetTimeUnusedThisLogin(Character *lockedCharacter,
                               const char* skill);
  const char* GetSkillLastUsedStr(const char* skill);
  void CharacterLogout(Character* lockedCharacter,
                       const char* skill);
  void UpdateTimeUnused(Character* lockedCharacter,
                        const char* skill);
  static const char* GetAbilityNameForSkill(const char* skillType);


private:
};

#endif //SKILLMANAGER
