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

#include "table-characterskills.h"

CharacterSkillsTable::CharacterSkillsTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from characterskill;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void CharacterSkillsTable::createTable()
{
  printf("Creating Table characterskill...\n");
  db->update("create table characterskill ("
    "id INTEGER, "
    "skill INTEGER, "
    "PRIMARY KEY (id, skill) );");
}

void CharacterSkillsTable::insert(int characterskill, int skill)
{
  db->update("insert or replace into characterskill (id, skill) values ('%d','%d');",
    characterskill, skill);
}

void CharacterSkillsTable::set(int characterskill, unsigned int skill_id)
{
  insert(characterskill, skill_id);
}

int CharacterSkillsTable::get(int characterskill, unsigned int skill_id)
{
  ResultSet* rs = db->query("select * from characterskill where id = '%d' and skill = '%d';",
    characterskill, skill_id);

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
    printf("DB inconsistency: multiple equal skills in the characterskill!");
  }

  delete rs;

  return level;
}

void CharacterSkillsTable::dropTable()
{
  db->update("drop table characterskill;");
}

Array<CharSkillVO*> CharacterSkillsTable::getAllEntries(int id)
{
  ResultSet* rs = db->query("select skill from characterskill where id = '%d';", id);
  Array<CharSkillVO*> entries;
  for (size_t i=0; rs && i<rs->GetRowCount(); i++)
  {
    CharSkillVO* entry = new CharSkillVO();
    int skill_id = atoi(rs->GetData(i,0).c_str());
    entry->skill_id = skill_id;
    entries.add(entry);
  }
  delete rs;
  return entries;
}  
