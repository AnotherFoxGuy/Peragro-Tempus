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
};

class LoginRequestMessage : public NetMessage
{
  char* user;
  char* pwhash;

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
    pwhash = serial.getString();
  }

  const char* getName() { return user; }
  void setName(const char* usr) { user = (char*) usr; }

  const char* getPwHash() { return pwhash; }
  void setPwHash(const char* pw) { pwhash = (char*) pw; }
};

class LoginResponseMessage : public NetMessage
{
  char* error;

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

  const char* getError() { return error; }
  void setError(const char* message) { error = (char* )message; }
};

class RegisterRequestMessage : public NetMessage
{
  char* user;
  char* pwhash;

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
    pwhash = serial.getString();
  }

  const char* getName() { return user; }
  void setName(const char* usr) { user = (char*) usr; }

  const char* getPwHash() { return pwhash; }
  void setPwHash(const char* pw) { pwhash = (char*) pw; }
};

class RegisterResponseMessage : public NetMessage
{
  char* error;

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

  const char* getError() { return error; }
  void setError(const char* message) { error = (char* )message; }
};

class CharacterListMessage : public NetMessage
{
  char charCount;

  class nwCharacter
  {
  public:
    int id;
    const char* name;
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

  const char* getCharacterName(int idx) { return chars[idx].name; }
  void setCharacterName(int idx, const char* name) { chars[idx].name = name; }
};

class CharacterCreationRequestMessage : public NetMessage
{
  char* name;

public:
  CharacterCreationRequestMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_CREATION_REQUEST) {}

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(name);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    name = serial.getString();
  }

  const char* getName() { return name; }
  void setName(const char* n) { name = (char*) n; }
};

class CharacterCreationResponseMessage : public NetMessage
{
  const char* error;
  int char_id;
  const char* char_name;

public:
  CharacterCreationResponseMessage() : NetMessage(MESSAGES::USER,USER::CHARACTER_CREATION_RESPONSE) { }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(char_id);
    serial.setString(char_name);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    char_id = serial.getInt32();
    char_name = serial.getString();
    error = serial.getString();
  }

  const char* getError() { return error; }
  void setError(const char* message) { error = (char* )message; }

  int getCharacterId() { return char_id; }
  void setCharacterId(int charid) { char_id = charid; }

  const char* getCharacterName() { return char_name; }
  void setCharacterName(const char* name) { char_name = name; }

};

class CharacterSelectionRequestMessage : public NetMessage
{
  int charId;

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

  int getCharId() { return charId; }
  void setCharId(int char_id) { charId = char_id; }
};

class CharacterSelectionResponseMessage : public NetMessage
{
  char* error;
  int entityId;

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

  const char* getError() { return error; }
  void setError(const char* message) { error = (char* )message; }

  int getEntityId() { return entityId; }
  void setEntityId(int ent_id) { entityId = ent_id; }
};

#endif // _USERMESSAGES_H_
