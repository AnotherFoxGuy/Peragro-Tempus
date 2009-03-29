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

#include "common/network/skillmessages.h"
#include "deserialiser.h"
#include "serialiser.h"

bool SkillListMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setInt24(statscount);
  for ( size_t i = 0; i < statscount ; i++ )
  {
    serial.setInt16(stats[i].skillid);
    serial.setString(stats[i].name);
    serial.setInt16(stats[i].level);
  };

  return serial.isValid();
}

void SkillListMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  statscount = (unsigned int) serial.getInt24();
  setStatsCount(statscount);
  for ( size_t i = 0; i < statscount ; i++ )
  {
    stats[i].skillid = (unsigned short) serial.getInt16();
    stats[i].name = serial.getString();
    stats[i].level = (unsigned short) serial.getInt16();
  };

}

bool SkillUpdateMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setInt16(skillid);
  serial.setInt16(level);
  return serial.isValid();
}

void SkillUpdateMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  skillid = (unsigned short) serial.getInt16();
  level = (unsigned short) serial.getInt16();
}

