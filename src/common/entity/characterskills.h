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

#include "skill.h"
#include "server/database/table-characterskills.h"

class Connection;

class CharSkill
{
public:
  int skill_id;
  Entity* target;
};

class CharacterSkills
{
private:
  Array<CharSkill*> entries;

  CharSkill* findEntry(int skill_id)
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
  ~CharacterSkills() {}

  void addSkill(Skill* skill)
  {
    CharSkill* entry = findEntry(skill->getId());
    if (!entry)
    {
      entry = new CharSkill();
      entry->skill_id = skill->getId();
      entries.add(entry);
    }
    if (cstab) cstab->set(cs_id, entry);
  }

  void loadFromDatabase(CharacterSkillsTable* cst, int id)
  {
    cs_id = id;
    cstab = cst;

    //Load all Skills from Database
    cst->getAllEntries(entries, id);
  }

  void sendAllSkills(Connection* conn);
};

#endif // CHARACTERSKILLS_H_
