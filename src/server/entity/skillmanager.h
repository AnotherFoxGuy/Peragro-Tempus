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

#ifndef _SKILLMANAGER_H_
#define _SKILLMANAGER_H_

#include "common/util/array.h"
#include "skill.h"
#include "server/database/table-skills.h"

class SkillManager
{
private:
  Array<Skill*> skills;

public:
  SkillManager() {}
  ~SkillManager() { skills.delAll(); }

  size_t getSkillCount()
  {
    return skills.getCount();
  }

  Skill* getSkill(size_t index)
  {
    return skills.get(index);
  }

  void addSkill(Skill* skill)
  {
    skills.add(skill);
  }

  void delSkill(Skill* skill)
  {
    if (!skill) return;
    //ptString name = skill->getName();
    //if (!name) return;
    for (size_t i = 0; i<skills.getCount(); i++)
    {
      Skill* _skill = skills.get(i);
      if (_skill->getId() == skill->getId())
      {
        skills.remove(i);
        return;
      }
    }
  }

  bool exists(Skill* skill)
  {
    if (!skill) return false;
    for (size_t i = 0; i<skills.getCount(); i++)
    {
      Skill* _skill = skills.get(i);

      if (_skill->getId() == skill->getId())
        return true;
    }
    return false;
  }

  Skill* findByName(ptString name)
  {
    if (name == ptString::Null) return 0;
    for (size_t i = 0; i<skills.getCount(); i++)
    {
      Skill* skill = skills.get(i);
      if (skill->getName() == name)
      {
        return skill;
      }
    }
    return 0;
  }

  Skill* findById(int id)
  {
    for (size_t i = 0; i<skills.getCount(); i++)
    {
      Skill* skill = skills.get(i);
      if (skill->getId() == id)
      {
        return skill;
      }
    }
    return 0;
  }

  void loadFromDB(SkillTable* st)
  {
    //Load all Skills from Database
    st->getAllSkills(skills);
  }

  void clear()
  {
    skills.removeAll();
  }
};

#endif // _SKILLMANAGER_H_
