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

#ifndef USERMESSAGES_H
#define USERMESSAGES_H

#include "netmessage.h"

namespace USER
{
  enum MESSAGES
  {
    LOGINREQUEST=0,
    LOGINRESPONSE=1,
    REGISTERREQUEST=2,
    REGISTERRESPONSE=3,
    CHARLIST=4,
    CHARCREATEREQUEST=5,
    CHARCREATERESPONSE=6,
    CHARSELECTREQUEST=7,
    CHARSELECTRESPONSE=8,
    MESHLISTREQUEST=9,
    MESHLISTRESPONSE=10
  };
}

class LoginRequestMessage : public NetMessage
{
  ptString username;
  const char* password;

public:
  LoginRequestMessage() : NetMessage(MESSAGES::USER,USER::LOGINREQUEST)
  {
  }

  ~LoginRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getUsername() const { return username; }
  void setUsername(ptString x) { username = x; }

  const char* getPassword() const { return password; }
  void setPassword(const char* x) { password = x; }

};

class LoginResponseMessage : public NetMessage
{
  ptString error;
  bool isadmin;

public:
  LoginResponseMessage() : NetMessage(MESSAGES::USER,USER::LOGINRESPONSE)
  {
  }

  ~LoginResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() const { return error; }
  void setError(ptString x) { error = x; }

  bool getIsAdmin() const { return isadmin; }
  void setIsAdmin(bool x) { isadmin = x; }

};

class RegisterRequestMessage : public NetMessage
{
  ptString username;
  const char* password;

public:
  RegisterRequestMessage() : NetMessage(MESSAGES::USER,USER::REGISTERREQUEST)
  {
  }

  ~RegisterRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getUsername() const { return username; }
  void setUsername(ptString x) { username = x; }

  const char* getPassword() const { return password; }
  void setPassword(const char* x) { password = x; }

};

class RegisterResponseMessage : public NetMessage
{
  ptString error;

public:
  RegisterResponseMessage() : NetMessage(MESSAGES::USER,USER::REGISTERRESPONSE)
  {
  }

  ~RegisterResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() const { return error; }
  void setError(ptString x) { error = x; }

};

class CharListMessage : public NetMessage
{
  class ListCharacter
  {
  public:
    unsigned int charid;
    ptString name;
    unsigned char haircolour[3];
    unsigned char skincolour[3];
    unsigned char decalcolour[3];
  };

  unsigned int charactercount;
  ListCharacter* character;


public:
  CharListMessage() : NetMessage(MESSAGES::USER,USER::CHARLIST)
  {
    character = 0;
  }

  ~CharListMessage()
  {
    delete [] character;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned char getCharacterCount() const { return charactercount; }
  void setCharacterCount(unsigned char ic)
  {
    charactercount = ic;
    delete [] character;
    character = new ListCharacter[ic];
  }

  // --- begin ListCharacter Getter and Setter ---

  unsigned int getCharId(size_t i) { return character[i].charid; }
  void setCharId(size_t i, unsigned int x) { character[i].charid = x; }

  ptString getName(size_t i) { return character[i].name; }
  void setName(size_t i, ptString x) { character[i].name = x; }

  unsigned char* getHairColour(size_t i) { return character[i].haircolour; }
  void setHairColour(size_t i, unsigned char r, unsigned char g, unsigned char b)
  {
    character[i].haircolour[0] = r;
    character[i].haircolour[1] = g;
    character[i].haircolour[2] = b;
  }
  void setHairColour(size_t i, const unsigned char* x)
  {
    setHairColour(i, x[0], x[1], x[2]);
  }

  unsigned char* getSkinColour(size_t i) { return character[i].skincolour; }
  void setSkinColour(size_t i, unsigned char r, unsigned char g, unsigned char b)
  {
    character[i].skincolour[0] = r;
    character[i].skincolour[1] = g;
    character[i].skincolour[2] = b;
  }
  void setSkinColour(size_t i, const unsigned char* x)
  {
    setSkinColour(i, x[0], x[1], x[2]);
  }

  unsigned char* getDecalColour(size_t i) { return character[i].decalcolour; }
  void setDecalColour(size_t i, unsigned char r, unsigned char g, unsigned char b)
  {
    character[i].decalcolour[0] = r;
    character[i].decalcolour[1] = g;
    character[i].decalcolour[2] = b;
  }
  void setDecalColour(size_t i, const unsigned char* x)
  {
    setDecalColour(i, x[0], x[1], x[2]);
  }

  // --- end ListCharacter Getter and Setter ---

};

class CharCreateRequestMessage : public NetMessage
{
  ptString name;
  ptString racename;
  unsigned char haircolour[3];
  unsigned char skincolour[3];
  unsigned char decalcolour[3];

public:
  CharCreateRequestMessage() : NetMessage(MESSAGES::USER,USER::CHARCREATEREQUEST)
  {
  }

  ~CharCreateRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  ptString getRaceName() const { return racename; }
  void setRaceName(ptString x) { racename = x; }

  unsigned char* getHairColour() { return haircolour; }
  void setHairColour(unsigned char r, unsigned char g, unsigned char b)
  {
    haircolour[0] = r;
    haircolour[1] = g;
    haircolour[2] = b;
  }
  void setHairColour(const unsigned char* x)
  {
    setHairColour(x[0], x[1], x[2]);
  }

  unsigned char* getSkinColour() { return skincolour; }
  void setSkinColour(unsigned char r, unsigned char g, unsigned char b)
  {
    skincolour[0] = r;
    skincolour[1] = g;
    skincolour[2] = b;
  }
  void setSkinColour(const unsigned char* x)
  {
    setSkinColour(x[0], x[1], x[2]);
  }

  unsigned char* getDecalColour() { return decalcolour; }
  void setDecalColour(unsigned char r, unsigned char g, unsigned char b)
  {
    decalcolour[0] = r;
    decalcolour[1] = g;
    decalcolour[2] = b;
  }
  void setDecalColour(const unsigned char* x)
  {
    setDecalColour(x[0], x[1], x[2]);
  }

};

class CharCreateResponseMessage : public NetMessage
{
  unsigned int charid;
  ptString name;
  unsigned char haircolour[3];
  unsigned char skincolour[3];
  unsigned char decalcolour[3];
  ptString error;

public:
  CharCreateResponseMessage() : NetMessage(MESSAGES::USER,USER::CHARCREATERESPONSE)
  {
  }

  ~CharCreateResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getCharId() const { return charid; }
  void setCharId(unsigned int x) { charid = x; }

  ptString getName() const { return name; }
  void setName(ptString x) { name = x; }

  unsigned char* getHairColour() { return haircolour; }
  void setHairColour(unsigned char r, unsigned char g, unsigned char b)
  {
    haircolour[0] = r;
    haircolour[1] = g;
    haircolour[2] = b;
  }
  void setHairColour(const unsigned char* x)
  {
    setHairColour(x[0], x[1], x[2]);
  }

  unsigned char* getSkinColour() { return skincolour; }
  void setSkinColour(unsigned char r, unsigned char g, unsigned char b)
  {
    skincolour[0] = r;
    skincolour[1] = g;
    skincolour[2] = b;
  }
  void setSkinColour(const unsigned char* x)
  {
    setSkinColour(x[0], x[1], x[2]);
  }

  unsigned char* getDecalColour() { return decalcolour; }
  void setDecalColour(unsigned char r, unsigned char g, unsigned char b)
  {
    decalcolour[0] = r;
    decalcolour[1] = g;
    decalcolour[2] = b;
  }
  void setDecalColour(const unsigned char* x)
  {
    setDecalColour(x[0], x[1], x[2]);
  }

  ptString getError() const { return error; }
  void setError(ptString x) { error = x; }

};

class CharSelectRequestMessage : public NetMessage
{
  unsigned int charid;

public:
  CharSelectRequestMessage() : NetMessage(MESSAGES::USER,USER::CHARSELECTREQUEST)
  {
  }

  ~CharSelectRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getCharId() const { return charid; }
  void setCharId(unsigned int x) { charid = x; }

};

class CharSelectResponseMessage : public NetMessage
{
  unsigned int entityid;
  ptString error;

public:
  CharSelectResponseMessage() : NetMessage(MESSAGES::USER,USER::CHARSELECTRESPONSE)
  {
  }

  ~CharSelectResponseMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getEntityId() const { return entityid; }
  void setEntityId(unsigned int x) { entityid = x; }

  ptString getError() const { return error; }
  void setError(ptString x) { error = x; }

};

class MeshListRequestMessage : public NetMessage
{

public:
  MeshListRequestMessage() : NetMessage(MESSAGES::USER,USER::MESHLISTREQUEST)
  {
  }

  ~MeshListRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class MeshListResponseMessage : public NetMessage
{
  class ListMeshes
  {
  public:
    unsigned int meshid;
    ptString meshname;
    ptString filename;
  };

  unsigned int meshescount;
  ListMeshes* meshes;


public:
  MeshListResponseMessage() : NetMessage(MESSAGES::USER,USER::MESHLISTRESPONSE)
  {
    meshes = 0;
  }

  ~MeshListResponseMessage()
  {
    delete [] meshes;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned char getMeshesCount() const { return meshescount; }
  void setMeshesCount(unsigned char ic)
  {
    meshescount = ic;
    delete [] meshes;
    meshes = new ListMeshes[ic];
  }

  // --- begin ListMeshes Getter and Setter ---

  unsigned int getMeshId(size_t i) { return meshes[i].meshid; }
  void setMeshId(size_t i, unsigned int x) { meshes[i].meshid = x; }

  ptString getMeshName(size_t i) { return meshes[i].meshname; }
  void setMeshName(size_t i, ptString x) { meshes[i].meshname = x; }

  ptString getFileName(size_t i) { return meshes[i].filename; }
  void setFileName(size_t i, ptString x) { meshes[i].filename = x; }

  // --- end ListMeshes Getter and Setter ---

};

#endif // USERMESSAGES_H
