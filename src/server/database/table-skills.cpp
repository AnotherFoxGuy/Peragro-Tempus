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
    "mpCost INTEGER, "
    "PRIMARY KEY (id) );");

  //Example Data!
  /// \todo specify all fields!
  insert(ptString("Heal", strlen("Heal")), Skill::TYPE_HEAL, 10, 100, 1000, 10, 5);
  insert(ptString("Energy Bind", strlen("Energy Bind")), Skill::TYPE_HURT, 10, 100, 1000, 10, 10);
}

/// \todo insert all fields!
void SkillTable::insert(ptString name, int type, float range, int skillTime, int reuseDelay, int power, int mpCost)
{
  db->update("insert into skills (name, type, range, skillTime, reuseDelay, power, mpCost)"
    " values ('%q', '%i', '%f', '%i', '%i', '%i', '%i');",
    *name, type, range, skillTime, reuseDelay, power, mpCost);
}

void SkillTable::dropTable()
{
  db->update("drop table skills;");
}

bool SkillTable::existsSkill(ptString name)
{
  ResultSet* rs = db->query("select id from skills where name = '%q';", *name);
  bool existence = (rs->GetRowCount() > 0);
  delete rs;
  return existence;
}

/// \todo load all fields!
Skill* SkillTable::getSkill(ptString name)
{
  ResultSet* rs = db->query("select * from skills where name = '%q';", *name);

  Skill* skill = 0;

  if (rs && rs->GetRowCount() == 1) 
  {
    skill = new Skill();
    skill->setId(atoi(rs->GetData(0,0).c_str()));
    skill->setName(ptString(rs->GetData(0,1).c_str(), rs->GetData(0,1).length()));
    skill->setType(Skill::SkillType(atoi(rs->GetData(0,2).c_str())));
    skill->setRange((float)atof(rs->GetData(0,5).c_str()));
    skill->setSkillTime(atoi(rs->GetData(0,6).c_str()));
    skill->setReuseDelay(atoi(rs->GetData(0,8).c_str()));
    skill->setPower(atoi(rs->GetData(0,11).c_str()));
    skill->setMpCost(atoi(rs->GetData(0,12).c_str()));
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
    skill->setType(Skill::SkillType(atoi(rs->GetData(i,2).c_str())));
    skill->setRange((float)atof(rs->GetData(i,5).c_str()));
    skill->setSkillTime(atoi(rs->GetData(i,6).c_str()));
    skill->setReuseDelay(atoi(rs->GetData(i,8).c_str()));
    skill->setPower(atoi(rs->GetData(i,11).c_str()));
    skill->setMpCost(atoi(rs->GetData(i,12).c_str()));
    skills.add(skill);
  }
  delete rs;
}  
