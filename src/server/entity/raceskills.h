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

#ifndef RACESKILLS_H_
#define RACESKILLS_H_

#include "skill.h"
#include "common/database/database.h"
#include "server/database/tables.h"
#include "server/database/table-raceskills.h"
#include "server/database/table-characterskills.h"
#include "server/server.h"

class RaceSkill
{
public:
  int skill_id;
};

class Connection;

class RaceSkills
{
private:
  Array<RaceSkill*> entries;

  RaceSkill* findEntry(int skill_id)
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

  RaceSkillsTable* estab;

  int es_id;

public:
  RaceSkills() : estab(0) {}
  ~RaceSkills() { entries.delAll(); }

  void copyToCharacter(int char_id)
  {
    CharacterSkillsTable* cst = Server::getServer()->getTables()->getCharacterSkillsTable();
    for (size_t i=0; i<entries.getCount(); i++)
    {
      RaceSkill* skill = entries.get(i);
      cst->insert(char_id, skill->skill_id);
    }
  }

  void loadFromDatabase(RaceSkillsTable* est, int id)
  {
    es_id = id;
    estab = est;

    //Load all Skills from Database
    est->getAllEntries(entries, id);
  }

  void sendAllSkills(Connection* conn);
};

#endif // RACESKILLS_H_
