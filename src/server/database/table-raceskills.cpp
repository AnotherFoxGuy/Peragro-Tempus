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

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "database.h"

#include "table-raceskills.h"

#include "table-skills.h"
#include "common/entity/raceskills.h"
#include "common/entity/skill.h"

RaceSkillsTable::RaceSkillsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from raceskill;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void RaceSkillsTable::createTable()
{
  db->update("create table raceskill ("
    "id INTEGER, "
    "skill INTEGER, "
    "PRIMARY KEY (id, skill) );");

  Skill* skill = 0;

  insert(1, 0); //Heal
  delete skill;
}

void RaceSkillsTable::insert(int raceskill, int skill)
{
  db->update("insert or replace into raceskill (id, skill) values ('%d','%d');",
    raceskill, skill);
}

void RaceSkillsTable::set(int raceskill, Skill* skill)
{
  insert(raceskill, skill->getId());
}

int RaceSkillsTable::get(int raceskill, Skill* skill)
{
  char query[1024];
  sprintf(query, "select * from raceskill where id = '%d' and skill = '%d';",
    raceskill, skill->getId());
  ResultSet* rs = db->query(query);

  int level = 0;

  if (rs->GetRowCount() == 1)
  {
    level = atoi(rs->GetData(0,0).c_str());
  }
  else if (rs->GetRowCount() == 0)
  {
    level = 0;
  }
  else
  {
    printf("DB inconsistency: multiple equal skills in the raceskill!");
  }

  delete rs;

  return level;
}

void RaceSkillsTable::dropTable()
{
  db->update("drop table raceskill;");
}

void RaceSkillsTable::getAllEntries(Array<RaceSkill*>& entries, int id)
{
  char query[1024];
  sprintf(query, "select skill from raceskill where id = '%d';", id);
  ResultSet* rs = db->query(query);
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    RaceSkill* entry = new RaceSkill();
    int skill_id = atoi(rs->GetData(i,0).c_str());
    entry->skill_id = skill_id;
    entries.add(entry);
  }
  delete rs;
}  
