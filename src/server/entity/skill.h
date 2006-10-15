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

#ifndef SKILL_H_
#define SKILL_H_

#include <string.h>
#include "common/util/ptstring.h"

class Entity;
class CharSkill;
class CharacterEntity;
class Stat;

namespace SkillState
{
  enum Type
  {
    READY = 0,
    CASTING = 1,
    RECOVER = 2
  };
};

class Skill
{
private:
  int id;

  ptString name_id;

public:
  enum SkillType
  {
    TYPE_HEAL = 0,
    TYPE_HURT = 1
  };

private:
  SkillType type;

public:
  enum SkillWorking
  {
    WORKS_ALWAYS = 0,
    WORKS_ONCE = 1,
    WORKS_DURATION = 2,
    WORKS_TOGGLE = 3
  };

private:
  SkillWorking op;

public:
  enum SkillAffect
  {
    AFFECT_YOU = 0,  // Skills appicalbe only on yourself.
    AFFECT_ONE = 1,  // Skills appicalbe on one target.
    AFFECT_NEAR = 2, // Skills appicalbe on nearby characters.
    AFFECT_PARTY = 3 // Skills appicalbe on all party members.
  };

private:
  SkillAffect affect;

  float range; // Radius in which the skill will be applied.

  int skillTime; //cast delay
  int hitTime;
  int reuseDelay;
  int buffDuration;

  unsigned int mpCost;

  int targetType;
  int power;

  Stat* mp;
  Stat* hp;

public:
  Skill();
  ~Skill() {}

  void setId(int id) { this->id = id; }
  int getId() { return id; }

  ptString& getName() { return name_id; }
  void setName(ptString id) { name_id = id; }

  SkillType getType() { return type; }
  void setType(SkillType t) { type = t; }

  SkillAffect getAffect() { return affect; }
  void setAffect(SkillAffect a) { affect = a; }

  void setRange(float range) { this->range = range; }
  void setSkillTime(int skillTime) { this->skillTime = skillTime; }
  void setReuseDelay(int reuseDelay) { this->reuseDelay = reuseDelay; }
  void setPower(int power) { this->power = power; }
  void setMpCost(unsigned int mpCost) { this->mpCost = mpCost; }

  //virtual void triggerSkill(CharSkill* skilldata, CharacterEntity* caster);

  void castPrepare(CharacterEntity* caster, unsigned int target_id);
  void castInterrupt(CharSkill* skilldata);
  void castExecute(CharSkill* skilldata);
  void castRecover(CharSkill* skilldata);
};

#endif // SKILL_H_
