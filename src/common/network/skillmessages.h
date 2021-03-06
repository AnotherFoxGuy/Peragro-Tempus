/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#ifndef SKILLMESSAGES_H
#define SKILLMESSAGES_H

#include "netmessage.h"

namespace SKILL
{
  enum MESSAGES
  {
    SKILLLIST = 0,
    SKILLUPDATE = 1
  };
}

class SkillListMessage : public NetMessage
{
  unsigned int entityid;
  class ListStats
  {
  public:
    unsigned short skillid;
    ptString name;
    float xp;
  };

  unsigned int statscount;
  ListStats* stats;


public:
  SkillListMessage() : NetMessage(MESSAGES::SKILL,SKILL::SKILLLIST)
  {
    stats = 0;
  }

  ~SkillListMessage()
  {
    delete [] stats;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getEntityId() const { return entityid; }
  void setEntityId(unsigned int x) { entityid = x; }

  unsigned char getStatsCount() const { return statscount; }
  void setStatsCount(unsigned char ic)
  {
    statscount = ic;
    delete [] stats;
    stats = new ListStats[ic];
  }

  // --- begin ListStats Getter and Setter ---

  unsigned short getSkillId(size_t i) { return stats[i].skillid; }
  void setSkillId(size_t i, unsigned short x) { stats[i].skillid = x; }

  ptString getName(size_t i) { return stats[i].name; }
  void setName(size_t i, ptString x) { stats[i].name = x; }

  float getXp(size_t i) { return stats[i].xp; }
  void setXp(size_t i, float x) { stats[i].xp = x; }

  // --- end ListStats Getter and Setter ---

};

class SkillUpdateMessage : public NetMessage
{
  unsigned int entityid;
  unsigned short skillid;
  float xp;

public:
  SkillUpdateMessage() : NetMessage(MESSAGES::SKILL,SKILL::SKILLUPDATE)
  {
  }

  ~SkillUpdateMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getEntityId() const { return entityid; }
  void setEntityId(unsigned int x) { entityid = x; }

  unsigned short getSkillId() const { return skillid; }
  void setSkillId(unsigned short x) { skillid = x; }

  float getXp() const { return xp; }
  void setXp(float x) { xp = x; }

};

#endif // SKILLMESSAGES_H
