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

#ifndef _TABLE_CHARACTERSKILLS_H_
#define _TABLE_CHARACTERSKILLS_H_

#include "common/util/array.h"
#include "table.h"

class Database;
class Skill;
class CharSkill;

class CharacterSkillsTable : public Table
{
public:
  CharacterSkillsTable(Database* db);
  void createTable();
  void insert(int characterskill, int skill);
  void set(int characterskill, CharSkill* skill);
  int get(int characterskill, CharSkill* skill);
  void dropTable();
  void getAllEntries(Array<CharSkill*>& entries, int id);
};

#endif //_TABLE_CHARACTERSKILLS_H_
