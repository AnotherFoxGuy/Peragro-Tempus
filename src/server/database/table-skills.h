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

#ifndef _TABLE_SKILLS_H_
#define _TABLE_SKILLS_H_

#include "table.h"

class Database;
class Skill;
class ptString;

/**
 * Provides an interface to the database to handle storage of Skills.
 */
class SkillTable : public Table
{
public:
  /**
   * Constructor for the SkillTable.
   * If no skill table is found in the database, the createTable function
   * will be called in order to populate the database.
   * @param db A pointer to the database
   */
  SkillTable(Database* db);
  /**
   * Creates a table in the database that will store skills.
   */
  void createTable();
   /**
   * Inserts a skill into the database.
   * @param name The name of the skill.
   * @param type The type of skill.
   * @param range The range the skill is usuable.
   * @param skillTime How long the skill lasts.
   * @param reuseDelay How long to wait before the skill can be used again.
   * @param power The power of the skill or the power need to use it? (TODO).
   * @param mpCost How much mana each usage drains.
   */
  void insert(ptString name, int type, float range, int skillTime,
              int reuseDelay, int power, int mpCost);
  /**
   * Removes all skills currently stored in the database.
   */
  void dropTable();
  /**
   * Checks if a skill exists or not.
   * @return True if the skill exists, false otherwise.
   */
  bool existsSkill(ptString name);
  /**
   * Returns a skill based on the name.
   * The caller is responsible for freeing the returned skill.
   * @param name The name of the skill to search for.
   * @return Skill if found, otherwise 0.
   */
  Skill* getSkill(ptString name);
  /**
   * This function will load all skills from the database.
   * The caller is responsible for freeing all skills in the array.
   * @param skills An array that will contain all skills.
   */
  void getAllSkills(Array<Skill*>& skills);
};

#endif //_TABLE_SKILLS_H_
