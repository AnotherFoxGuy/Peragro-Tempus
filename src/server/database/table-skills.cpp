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

#include "table-skills.h"
#include "common/entity/skill.h"

SkillTable::SkillTable(Database* db) : Table(db)
{
  ResultSet* rs = db->query("select count(*) from skills;");
  if (rs == 0)
  {
    createTable();
  }
  delete rs;
}

void SkillTable::createTable()
{
  db->update("create table skills ("
    "id INTEGER, "
    "name TEXT, "
    "type INTEGER, "
    "op INTEGER, "
    "affect INTEGER, "
    "range INTEGER, "
    "skillTime INTEGER, "
    "hitTime INTEGER, "
    "reuseDelay INTEGER, "
    "buffDuration INTEGER, "
    "targetType INTEGER, "
    "power INTEGER, "
    "PRIMARY KEY (id) );");

  //Example Data!
  /// \todo specify all fields!
  insert(ptString("Fire Ball", strlen("Fire Ball")));
}

/// \todo insert all fields!
void SkillTable::insert(ptString name)
{
  if (strlen(*name) > 512) assert("Strings too long");
  char query[1024];
  sprintf(query, "insert into skills (name) values ('%s');", *name);
  db->update(query);
}

void SkillTable::dropTable()
{
  db->update("drop table skills;");
}

bool SkillTable::existsSkill(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select id from skills where name = '%s';", *name);
  ResultSet* rs = db->query(query);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

/// \todo load all fields!
Skill* SkillTable::getSkill(ptString name)
{
  if (strlen(*name)> 512) assert("String too long");
  char query[1024];
  sprintf(query, "select * from skills where name = '%s';", *name);
  ResultSet* rs = db->query(query);

  Skill* skill = 0;

  if (rs && rs->GetRowCount() == 1) 
  {
    skill = new Skill();
    skill->setId(atoi(rs->GetData(0,0).c_str()));
    skill->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
  }
  delete rs;
  return skill;
}

/// \todo load all fields!
void SkillTable::getAllSkills(Array<Skill*>& skills)
{
  ResultSet* rs = db->query("select * from skills;");
  if (!rs) return;
  for (size_t i=0; i<rs->GetRowCount(); i++)
  {
    Skill* skill = new Skill();
    skill->setId(atoi(rs->GetData(i,0).c_str()));
    skill->setName(ptString(rs->GetData(i,1).c_str(), rs->GetData(i,1).length()));
    skills.add(skill);
  }
  delete rs;
}  
