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
#include <stdlib.h>

#include "database.h"

#include "table-characterskills.h"

/**
 * Constructor that will create the character skill database table if it does
 * not already exist in the database and store the database pointer for
 * later usage.
 *
 * @param db The database to use for storing character skills.
 *
 */
CharacterSkillsTable::CharacterSkillsTable(Database* db) : Table(db) {
  ResultSet* rs = db->query("select count(*) from characterskill;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

/**
 * Method that creates the character skills table in the database.
 *
 * @todo Don't we need the level as well?
 */
void CharacterSkillsTable::createTable()
{
  printf("Creating Table characterskill...\n");
  db->update("create table characterskill ("
    "id INTEGER, "
    "skill INTEGER, "
    "PRIMARY KEY (id, skill) );");
}

/**
 * Method that will do the actual database inseration for a character skill.
 *
 * @todo Looks like whats being stored is characterID and skillID, but when
 * reading back "level" is read as well... Something wrong here?
 *
 * @param characterID The id of the character that owns the skill.
 * @param skillID The id of the skill to insert.
 */
void CharacterSkillsTable::insert(int characterID, int skillID)
{
  db->update("insert or replace into characterskill (id, skill) values ('%d','%d');",
    characterID, skillID);
}

/**
 * Method that will set the level of a specific skill for a character.
 *
 * @todo Looks like whats being stored is characterID and skillID, but when
 * reading back "level" is read as well... Something wrong here?
 *
 * @param characterID The id of the character for which to return the level.
 * @param skill_id The id of the skill for which to return the level.
 */
void CharacterSkillsTable::set(int characterID, unsigned int skill_id)
{
  insert(characterID, skill_id);
}

/**
 * Function that will return the level of a specific skill for a character.
 *
 * @param characterID The id of the character for which to return the level.
 * @param skill_id The id of the skill for which to return the level.
 */
int CharacterSkillsTable::get(int characterID, unsigned int skill_id)
{
  ResultSet* rs = db->query("select * from characterskill where id = '%d' and skill = '%d';",
    characterID, skill_id);

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

/**
 * Method that will remove all character skill data from the database.
 */
void CharacterSkillsTable::dropTable()
{
  db->update("drop table characterskill;");
}

/**
 * Function that will return all character skill entries in the database with a
 * certain id.
 *
 * @param characterID This isthe id of the characterfor which to find skills.
 */
Array<CharSkillVO*> CharacterSkillsTable::getAllEntries(int characterID)
{
  ResultSet* rs = db->query("select skill from characterskill where id = '%d';",
                            characterID);
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
