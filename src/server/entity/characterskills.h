/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef CHARACTERSKILLS_H_
#define CHARACTERSKILLS_H_

#include "characterskill.h"
#include "server/database/table-characterskills.h"

class Connection;

class CharacterSkills
{
private:
  Array<CharSkill*> entries;

  CharSkill* findEntry(size_t skill_id)
  {
    for(unsigned int i=0; i<entries.getCount(); i++)
    {
      if (entries.get(i)->skill_id == skill_id)
      {
        return entries.get(i);
      }
    }
    return 0;
  }

  CharacterSkillsTable* cstab;

  int cs_id;

public:
  CharacterSkills() : cstab(0) {}
  ~CharacterSkills() { entries.delAll(); }

  void addSkill(Skill* skill)
  {
    CharSkill* entry = findEntry(skill->GetId());
    if (!entry)
    {
      entry = new CharSkill();
      entry->skill_id = skill->GetId();
      entries.add(entry);
    }
    if (cstab) cstab->set(cs_id, entry->skill_id);
  }

  size_t getSkillCount()
  {
    return entries.getCount();
  }

  CharSkill* getSkill(size_t idx)
  {
    return entries[idx];
  }

  CharSkill* findSkill(size_t id)
  {
    return findEntry(id);
  }

  /**
   *
   * Function that will load all skills belonging to a character.
   *
   * @param cst The given database table.
   * @param characterID The id of the character for which to load the skills.
   *
   */
  void loadFromDatabase(CharacterSkillsTable* cst, int characterID)
  {
    cs_id = characterID;
    cstab = cst;

    //Load all Skills from Database
    Array<CharSkillVO*> vos = cst->getAllEntries(characterID);
    for (size_t i = 0; i < vos.getCount(); i++)
    {
      CharSkillVO* vo = vos.get(i);

      CharSkill* skill = new CharSkill();
      skill->skill_id = vo->skill_id;
      skill->state = SkillState::READY;

      entries.add(skill);
    }

  }

  void sendAllSkills(Connection* conn);
};

#endif // CHARACTERSKILLS_H_
