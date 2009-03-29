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

#include "src/server/entity/character/character.h"

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

  void CheckDeprecation(boost::shared_ptr<Character> character,
                        const std::string& skill);
  float GetTimeSkillUnused(boost::shared_ptr<Character> character,
                           const std::string& skill);
  void CharacterLogou(boost::shared_ptr<Character> character,
                      const std::string& skill);
  void ResetTimeSkillLastUsed(boost::shared_ptr<Character> character,
                              const std::string& skill);
  float GetTimeUnusedLastLogin(boost::shared_ptr<Character> character,
                               const std::string& skill);
  float GetTimeUnusedThisLogin(boost::shared_ptr<Character> character,
                               const std::string& skill);
  std::string GetSkillLastUsedStr(const std::string& skill);
  void CharacterLogout(boost::shared_ptr<Character> character,
                       const std::string& skill);
  void UpdateTimeUnused(boost::shared_ptr<Character> character,
                        const std::string& skill);
  static std::string GetAbilityNameForSkill(const std::string& skillType);


private:
};

#endif //SKILLMANAGER
