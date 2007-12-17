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

#include "common/network/usermessages.h"
#include "deserialiser.h"
#include "serialiser.h"

void LoginRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(username);
  serial.setString(password);
}

void LoginRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  username = serial.getString();
  serial.getString(password);
}

void LoginResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(error);
  serial.setInt8(isadmin?1:0);
}

void LoginResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  error = serial.getString();
  isadmin = serial.getInt8() != 0;
}

void RegisterRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(username);
  serial.setString(password);
}

void RegisterRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  username = serial.getString();
  serial.getString(password);
}

void RegisterResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(error);
}

void RegisterResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  error = serial.getString();
}

void CharListMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(charactercount);
  for ( size_t i = 0; i < charactercount ; i++ )
  {
    serial.setInt32(character[i].charid);
    serial.setString(character[i].name);
    serial.setInt8(character[i].haircolour[0]);
    serial.setInt8(character[i].haircolour[1]);
    serial.setInt8(character[i].haircolour[2]);
    serial.setInt8(character[i].skincolour[0]);
    serial.setInt8(character[i].skincolour[1]);
    serial.setInt8(character[i].skincolour[2]);
    serial.setInt8(character[i].decalcolour[0]);
    serial.setInt8(character[i].decalcolour[1]);
    serial.setInt8(character[i].decalcolour[2]);
  };

}

void CharListMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  charactercount = (unsigned char) serial.getInt8();
  setCharacterCount(charactercount);
  for ( size_t i = 0; i < charactercount ; i++ )
  {
    character[i].charid = (unsigned int) serial.getInt32();
    character[i].name = serial.getString();
    character[i].haircolour[0] = (unsigned char) serial.getInt8();
    character[i].haircolour[1] = (unsigned char) serial.getInt8();
    character[i].haircolour[2] = (unsigned char) serial.getInt8();
    character[i].skincolour[0] = (unsigned char) serial.getInt8();
    character[i].skincolour[1] = (unsigned char) serial.getInt8();
    character[i].skincolour[2] = (unsigned char) serial.getInt8();
    character[i].decalcolour[0] = (unsigned char) serial.getInt8();
    character[i].decalcolour[1] = (unsigned char) serial.getInt8();
    character[i].decalcolour[2] = (unsigned char) serial.getInt8();
  };

}

void CharCreateRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(name);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[0]);
}

void CharCreateRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  name = serial.getString();
  haircolour[0] = (unsigned char) serial.getInt8();
  haircolour[1] = (unsigned char) serial.getInt8();
  haircolour[2] = (unsigned char) serial.getInt8();
  skincolour[0] = (unsigned char) serial.getInt8();
  skincolour[1] = (unsigned char) serial.getInt8();
  skincolour[2] = (unsigned char) serial.getInt8();
  decalcolour[0] = (unsigned char) serial.getInt8();
  decalcolour[1] = (unsigned char) serial.getInt8();
  decalcolour[2] = (unsigned char) serial.getInt8();
}

void CharCreateResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(charid);
  serial.setString(name);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(haircolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(skincolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setInt8(decalcolour[0]);
  serial.setString(error);
}

void CharCreateResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  charid = (unsigned int) serial.getInt32();
  name = serial.getString();
  haircolour[0] = (unsigned char) serial.getInt8();
  haircolour[1] = (unsigned char) serial.getInt8();
  haircolour[2] = (unsigned char) serial.getInt8();
  skincolour[0] = (unsigned char) serial.getInt8();
  skincolour[1] = (unsigned char) serial.getInt8();
  skincolour[2] = (unsigned char) serial.getInt8();
  decalcolour[0] = (unsigned char) serial.getInt8();
  decalcolour[1] = (unsigned char) serial.getInt8();
  decalcolour[2] = (unsigned char) serial.getInt8();
  error = serial.getString();
}

void CharSelectRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(charid);
}

void CharSelectRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  charid = (unsigned int) serial.getInt32();
}

void CharSelectResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
  serial.setString(error);
}

void CharSelectResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
  error = serial.getString();
}

