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

#ifndef _USERMESSAGES_H_
#define _USERMESSAGES_H_

#include "netmessage.h"

namespace USER
{
  enum MESSAGES
  {
    LOGIN_REQUEST=0,
    LOGIN_RESPONSE=1,
    REGISTER_REQUEST=2,
    REGISTER_RESPONSE=3,
    CHARACTER_LIST=4,
    CHARACTER_CREATION_REQUEST=5,
    CHARACTER_CREATION_RESPONSE=6,
    CHARACTER_SELECTION_REQUEST=7,
    CHARACTER_SELECTION_RESPONSE=8
  };
}

class LoginRequestMessage : public NetMessage
{
  ptString user;
  const char* pwhash;

public:
  LoginRequestMessage() : NetMessage(MESSAGES::USER,USER::LOGIN_REQUEST) {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(user);
    serial.setString(pwhash);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    user = serial.getString();
    serial.getString(pwhash);
  }

  ptString& getName() { return user; }
  void setName(ptString usr) { user = usr; }

  const char* getPwHash() { return pwhash; }
  void setPwHash(const char* pw) { pwhash = (char*) pw; }
};

class LoginResponseMessage : public NetMessage
{
  ptString error;

public:
  LoginResponseMessage() : NetMessage(MESSAGES::USER,USER::LOGIN_RESPONSE) {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    error = serial.getString();
  }

  ptString& getError() { return error; }
  void setError(ptString message) { error = message; }
};

class RegisterRequestMessage : public NetMessage
{
  ptString user;
  const char* pwhash;

public:
  RegisterRequestMessage() : NetMessage(MESSAGES::USER,USER::REGISTER_REQUEST) {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(user);
    serial.setString(pwhash);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    user = serial.getString();
    serial.getString(pwhash);
  }

  ptString& getName() { return user; }
  void setName(ptString usr) { user = usr; }

  const char* getPwHash() { return pwhash; }
  void setPwHash(const char* pw) { pwhash = (char*) pw; }
};

class RegisterResponseMessage : public NetMessage
{
  ptString error;

public:
  RegisterResponseMessage() : NetMessage(MESSAGES::USER,USER::REGISTER_RESPONSE) { }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    error = serial.getString();
  }

  ptString& getError() { return error; }
  void setError(ptString message) { error = message; }
};

class CharacterListMessage : public NetMessage
{
  char charCount;

  class nwCharacter
  {
  public:
    int id;
    ptString name;
    unsigned int haircolour; //24bit color
    unsigned int skincolour; //24bit color
    unsigned int decalcolour; //24bit color
  };

  nwCharacter* chars;

public:
  CharacterListMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_LIST), chars(0) {}
  ~CharacterListMessage() { delete [] chars; }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(charCount);
    for (int i=0; i<charCount; i++)
    {
      serial.setInt32(chars[i].id);
      serial.setString(chars[i].name);
      serial.setInt24(chars[i].skincolour);
      serial.setInt24(chars[i].haircolour);
      serial.setInt24(chars[i].decalcolour);
    }
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    setCharacterCount(serial.getInt8());
    for (int i=0; i<charCount; i++)
    {
      chars[i].id = serial.getInt32();
      chars[i].name = serial.getString();
      chars[i].skincolour = serial.getInt24();
      chars[i].haircolour = serial.getInt24();
      chars[i].decalcolour = serial.getInt24();
    }
  }

  char getCharacterCount() { return charCount; }
  void setCharacterCount(char cc) 
  {
    charCount = cc; 
    delete [] chars; 
    chars = new nwCharacter[cc];
  }

  int getCharacterId(int idx) { return chars[idx].id; }
  void setCharacterId(int idx, int char_id) { chars[idx].id = char_id; }

  ptString& getCharacterName(int idx) { return chars[idx].name; }
  void setCharacterName(int idx, ptString name) { chars[idx].name = name; }

  unsigned int getSkinColour(int idx) { return chars[idx].skincolour; }
  void setSkinColour(int idx, unsigned int colour) { chars[idx].skincolour = colour; }

  unsigned int getHairColour(int idx) { return chars[idx].haircolour; }
  void setHairColour(int idx, unsigned int colour) { chars[idx].haircolour = colour; }

  unsigned int getDecalColour(int idx) { return chars[idx].decalcolour; }
  void setDecalColour(int idx, unsigned int colour) { chars[idx].decalcolour = colour; }
};

class CharacterCreationRequestMessage : public NetMessage
{
  ptString name;
  unsigned int haircolour; //24bit color
  unsigned int skincolour; //24bit color
  unsigned int decalcolour; //24bit color

public:
  CharacterCreationRequestMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_CREATION_REQUEST) {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(name);
    serial.setInt24(skincolour);
    serial.setInt24(haircolour);
    serial.setInt24(decalcolour);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    name = serial.getString();
    skincolour = serial.getInt24();
    haircolour = serial.getInt24();
    decalcolour = serial.getInt24();
  }

  ptString& getName() { return name; }
  void setName(ptString n) { name = n; }

  unsigned int getSkinColour() { return skincolour; }
  void setSkinColour(unsigned int colour) { skincolour = colour; }

  unsigned int getHairColour() { return haircolour; }
  void setHairColour(unsigned int colour) { haircolour = colour; }

  unsigned int getDecalColour() { return decalcolour; }
  void setDecalColour(unsigned int colour) { decalcolour = colour; }
};

class CharacterCreationResponseMessage : public NetMessage
{
  ptString error;
  unsigned int char_id;
  ptString char_name;
  unsigned int haircolour; //24bit color
  unsigned int skincolour; //24bit color
  unsigned int decalcolour; //24bit color

public:
  CharacterCreationResponseMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_CREATION_RESPONSE) { }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(char_id);
    serial.setString(char_name);
    serial.setInt24(skincolour);
    serial.setInt24(haircolour);
    serial.setInt24(decalcolour);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    char_id = serial.getInt32();
    char_name = serial.getString();
    skincolour = serial.getInt24();
    haircolour = serial.getInt24();
    decalcolour = serial.getInt24();
    error = serial.getString();
  }

  ptString& getError() { return error; }
  void setError(ptString message) { error = message; }

  unsigned int getCharacterId() { return char_id; }
  void setCharacterId(unsigned int charid) { char_id = charid; }

  ptString& getCharacterName() { return char_name; }
  void setCharacterName(ptString name) { char_name = name; }

  unsigned int getSkinColour() { return skincolour; }
  void setSkinColour(unsigned int colour) { skincolour = colour; }

  unsigned int getHairColour() { return haircolour; }
  void setHairColour(unsigned int colour) { haircolour = colour; }

  unsigned int getDecalColour() { return decalcolour; }
  void setDecalColour(unsigned int colour) { decalcolour = colour; }
};

class CharacterSelectionRequestMessage : public NetMessage
{
  unsigned int charId;

public:
  CharacterSelectionRequestMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_SELECTION_REQUEST) {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(charId);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    charId = serial.getInt32();
  }

  unsigned int getCharId() { return charId; }
  void setCharId(unsigned int char_id) { charId = char_id; }
};

class CharacterSelectionResponseMessage : public NetMessage
{
  ptString error;
  unsigned int entityId;

public:
  CharacterSelectionResponseMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_SELECTION_RESPONSE) { }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(entityId);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    entityId = serial.getInt32();
    error = serial.getString();
  }

  ptString& getError() { return error; }
  void setError(ptString message) { error = message; }

  unsigned int getEntityId() { return entityId; }
  void setEntityId(unsigned int ent_id) { entityId = ent_id; }
};

#endif // _USERMESSAGES_H_
