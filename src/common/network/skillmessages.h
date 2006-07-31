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

#ifndef _SKILLMESSAGES_H_
#define _SKILLMESSAGES_H_

#include "netmessage.h"

namespace SKILL
{
  enum SKILL_MESSAGES
  {
    USAGE_START_REQUEST=0,
    USAGE_START_RESPONSE=1,
    USAGE_STOP_REQUEST=2,
    USAGE_COMPLETION=3,
    USAGE_INTERRUPT=4
  };
}

class SkillUsageStartRequestMessage : public NetMessage
{
  int target;
  int caster;
  short skill;

public:
  SkillUsageStartRequestMessage() : NetMessage(MESSAGES::SKILL,SKILL::USAGE_START_REQUEST) 
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(caster);
    serial.setInt32(target);
    serial.setInt16(skill);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    caster = serial.getInt32();
    target = serial.getInt32();
    skill = serial.getInt16();
  }

  unsigned int getTarget() { return target; }
  void setTarget(unsigned int t) { target = t; }

  unsigned int getCaster() { return caster; }
  void setCaster(unsigned int c) { caster = c; }

  unsigned short getSkill() { return skill; }
  void setSkill(unsigned short s) { skill = s; }
};

class SkillUsageStartResponseMessage : public NetMessage
{
  int target;
  int caster;
  short skill;
  short mpCost;
  ptString error;

public:
  SkillUsageStartResponseMessage() : NetMessage(MESSAGES::SKILL,SKILL::USAGE_START_RESPONSE) 
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt16(skill);
    serial.setInt16(mpCost);
    serial.setInt32(caster);
    serial.setInt32(target);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    skill = serial.getInt16();
    mpCost = serial.getInt16();
    caster = serial.getInt32();
    target = serial.getInt32();
    error = serial.getString();
  }

  unsigned int getTarget() { return target; }
  void setTarget(unsigned int t) { target = t; }

  unsigned int getCaster() { return caster; }
  void setCaster(unsigned int c) { caster = c; }

  ptString& getError() { return error; }
  void setError(ptString message) { error = message; }

  unsigned short getSkill() { return skill; }
  void setSkill(unsigned short s) { skill = s; }

  unsigned short getMpCost() { return mpCost; }
  void setMpCost(unsigned short mc) { mpCost = mc; }
};

class SkillUsageStopRequestMessage : public NetMessage
{
  short skill;

public:
  SkillUsageStopRequestMessage() : NetMessage(MESSAGES::SKILL,SKILL::USAGE_STOP_REQUEST) 
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt16(skill);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    skill = serial.getInt16();
  }

  unsigned short getSkill() { return skill; }
  void setSkill(unsigned short s) { skill = s; }
};

class SkillUsageCompletionMessage : public NetMessage
{
  int target;
  int caster;
  short skill;

public:
  SkillUsageCompletionMessage() : NetMessage(MESSAGES::SKILL,SKILL::USAGE_COMPLETION) 
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(caster);
    serial.setInt32(target);
    serial.setInt16(skill);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    caster = serial.getInt32();
    target = serial.getInt32();
    skill = serial.getInt16();
  }

  unsigned short getSkill() { return skill; }
  void setSkill(unsigned short s) { skill = s; }

  unsigned int getTarget() { return target; }
  void setTarget(unsigned int t) { target = t; }

  unsigned int getCaster() { return caster; }
  void setCaster(unsigned int c) { caster = c; }
};

class SkillUsageInterruptMessage : public NetMessage
{
  int target;
  int caster;
  short skill;

public:
  SkillUsageInterruptMessage() : NetMessage(MESSAGES::SKILL,SKILL::USAGE_INTERRUPT) 
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(caster);
    serial.setInt32(target);
    serial.setInt16(skill);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    caster = serial.getInt32();
    target = serial.getInt32();
    skill = serial.getInt16();
  }

  unsigned short getSkill() { return skill; }
  void setSkill(unsigned short s) { skill = s; }

  unsigned int getTarget() { return target; }
  void setTarget(unsigned int t) { target = t; }

  unsigned int getCaster() { return caster; }
  void setCaster(unsigned int c) { caster = c; }
};

#endif // _SKILLMESSAGES_H_
