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
    MESHLISTRESPONSE=10,
    AVATARLISTREQUEST=11,
    AVATARLISTRESPONSE=12,
    AVATARINFOREQUEST=13,
    AVATARINFORESPONSE=14
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
  unsigned int avatartemplateid;
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

  unsigned int getAvatarTemplateID() const { return avatartemplateid; }
  void setAvatarTemplateID(unsigned int x) { avatartemplateid = x; }

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

class AvatarListRequestMessage : public NetMessage
{

public:
  AvatarListRequestMessage() : NetMessage(MESSAGES::USER,USER::AVATARLISTREQUEST)
  {
  }

  ~AvatarListRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class AvatarListResponseMessage : public NetMessage
{
  class ListAvatar
  {
  public:
    unsigned int avatarid;
    ptString avatarname;
  };

  unsigned int avatarcount;
  ListAvatar* avatar;


public:
  AvatarListResponseMessage() : NetMessage(MESSAGES::USER,USER::AVATARLISTRESPONSE)
  {
    avatar = 0;
  }

  ~AvatarListResponseMessage()
  {
    delete [] avatar;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned char getAvatarCount() const { return avatarcount; }
  void setAvatarCount(unsigned char ic)
  {
    avatarcount = ic;
    delete [] avatar;
    avatar = new ListAvatar[ic];
  }

  // --- begin ListAvatar Getter and Setter ---

  unsigned int getAvatarId(size_t i) { return avatar[i].avatarid; }
  void setAvatarId(size_t i, unsigned int x) { avatar[i].avatarid = x; }

  ptString getAvatarName(size_t i) { return avatar[i].avatarname; }
  void setAvatarName(size_t i, ptString x) { avatar[i].avatarname = x; }

  // --- end ListAvatar Getter and Setter ---

};

class AvatarInfoRequestMessage : public NetMessage
{
  unsigned int avatarid;

public:
  AvatarInfoRequestMessage() : NetMessage(MESSAGES::USER,USER::AVATARINFOREQUEST)
  {
  }

  ~AvatarInfoRequestMessage()
  {
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getAvatarId() const { return avatarid; }
  void setAvatarId(unsigned int x) { avatarid = x; }

};

class AvatarInfoResponseMessage : public NetMessage
{
  unsigned int avatarid;
  ptString avatarname;
  unsigned int meshid;
  unsigned int speciesid;
  ptString factoryname;
  ptString filename;
  class ListInventory
  {
  public:
    unsigned int inventoryitemtemplateid;
    unsigned int inventorychance;
  };

  unsigned int inventorycount;
  ListInventory* inventory;

  class ListEquipment
  {
  public:
    unsigned int equipmentitemtemplateid;
    unsigned int equipmentchance;
  };

  unsigned int equipmentcount;
  ListEquipment* equipment;

  class ListReputations
  {
  public:
    ptString reputationsname;
    unsigned int reputationsminlevel;
    unsigned int reputationsmaxlevel;
    unsigned int reputationslevel;
  };

  unsigned int reputationscount;
  ListReputations* reputations;

  class ListSkills
  {
  public:
    ptString skillsname;
    unsigned int skillsmin;
    unsigned int skillsmax;
    unsigned int skillsxp;
  };

  unsigned int skillscount;
  ListSkills* skills;

  class ListHobbies
  {
  public:
    ptString hobbiesname;
    unsigned int hobbiesorder;
    unsigned int hobbiesxp;
  };

  unsigned int hobbiescount;
  ListHobbies* hobbies;

  class ListAbilities
  {
  public:
    ptString abilitiesname;
    unsigned int abilitiesmin;
    unsigned int abilitiesmax;
    unsigned int abilitiesxp;
  };

  unsigned int abilitiescount;
  ListAbilities* abilities;

  class ListVulberabilities
  {
  public:
    ptString vulberabilitiesname;
    unsigned int vulberabilitiesmin;
    unsigned int vulberabilitiesmax;
    unsigned int vulberabilitiesvalue;
  };

  unsigned int vulberabilitiescount;
  ListVulberabilities* vulberabilities;


public:
  AvatarInfoResponseMessage() : NetMessage(MESSAGES::USER,USER::AVATARINFORESPONSE)
  {
    inventory = 0;
    equipment = 0;
    reputations = 0;
    skills = 0;
    hobbies = 0;
    abilities = 0;
    vulberabilities = 0;
  }

  ~AvatarInfoResponseMessage()
  {
    delete [] inventory;
    delete [] equipment;
    delete [] reputations;
    delete [] skills;
    delete [] hobbies;
    delete [] abilities;
    delete [] vulberabilities;
  }

  bool serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getAvatarId() const { return avatarid; }
  void setAvatarId(unsigned int x) { avatarid = x; }

  ptString getAvatarName() const { return avatarname; }
  void setAvatarName(ptString x) { avatarname = x; }

  unsigned int getMeshId() const { return meshid; }
  void setMeshId(unsigned int x) { meshid = x; }

  unsigned int getSpeciesId() const { return speciesid; }
  void setSpeciesId(unsigned int x) { speciesid = x; }

  ptString getfactoryName() const { return factoryname; }
  void setfactoryName(ptString x) { factoryname = x; }

  ptString getfileName() const { return filename; }
  void setfileName(ptString x) { filename = x; }

  unsigned char getInventoryCount() const { return inventorycount; }
  void setInventoryCount(unsigned char ic)
  {
    inventorycount = ic;
    delete [] inventory;
    inventory = new ListInventory[ic];
  }

  // --- begin ListInventory Getter and Setter ---

  unsigned int getInventoryItemTemplateId(size_t i) { return inventory[i].inventoryitemtemplateid; }
  void setInventoryItemTemplateId(size_t i, unsigned int x) { inventory[i].inventoryitemtemplateid = x; }

  unsigned int getInventoryChance(size_t i) { return inventory[i].inventorychance; }
  void setInventoryChance(size_t i, unsigned int x) { inventory[i].inventorychance = x; }

  // --- end ListInventory Getter and Setter ---

  unsigned char getEquipmentCount() const { return equipmentcount; }
  void setEquipmentCount(unsigned char ic)
  {
    equipmentcount = ic;
    delete [] equipment;
    equipment = new ListEquipment[ic];
  }

  // --- begin ListEquipment Getter and Setter ---

  unsigned int getEquipmentItemTemplateId(size_t i) { return equipment[i].equipmentitemtemplateid; }
  void setEquipmentItemTemplateId(size_t i, unsigned int x) { equipment[i].equipmentitemtemplateid = x; }

  unsigned int getEquipmentChance(size_t i) { return equipment[i].equipmentchance; }
  void setEquipmentChance(size_t i, unsigned int x) { equipment[i].equipmentchance = x; }

  // --- end ListEquipment Getter and Setter ---

  unsigned char getReputationsCount() const { return reputationscount; }
  void setReputationsCount(unsigned char ic)
  {
    reputationscount = ic;
    delete [] reputations;
    reputations = new ListReputations[ic];
  }

  // --- begin ListReputations Getter and Setter ---

  ptString getReputationsName(size_t i) { return reputations[i].reputationsname; }
  void setReputationsName(size_t i, ptString x) { reputations[i].reputationsname = x; }

  unsigned int getReputationsMinLevel(size_t i) { return reputations[i].reputationsminlevel; }
  void setReputationsMinLevel(size_t i, unsigned int x) { reputations[i].reputationsminlevel = x; }

  unsigned int getReputationsMaxLevel(size_t i) { return reputations[i].reputationsmaxlevel; }
  void setReputationsMaxLevel(size_t i, unsigned int x) { reputations[i].reputationsmaxlevel = x; }

  unsigned int getReputationsLevel(size_t i) { return reputations[i].reputationslevel; }
  void setReputationsLevel(size_t i, unsigned int x) { reputations[i].reputationslevel = x; }

  // --- end ListReputations Getter and Setter ---

  unsigned char getSkillsCount() const { return skillscount; }
  void setSkillsCount(unsigned char ic)
  {
    skillscount = ic;
    delete [] skills;
    skills = new ListSkills[ic];
  }

  // --- begin ListSkills Getter and Setter ---

  ptString getSkillsName(size_t i) { return skills[i].skillsname; }
  void setSkillsName(size_t i, ptString x) { skills[i].skillsname = x; }

  unsigned int getSkillsMin(size_t i) { return skills[i].skillsmin; }
  void setSkillsMin(size_t i, unsigned int x) { skills[i].skillsmin = x; }

  unsigned int getSkillsMax(size_t i) { return skills[i].skillsmax; }
  void setSkillsMax(size_t i, unsigned int x) { skills[i].skillsmax = x; }

  unsigned int getSkillsXP(size_t i) { return skills[i].skillsxp; }
  void setSkillsXP(size_t i, unsigned int x) { skills[i].skillsxp = x; }

  // --- end ListSkills Getter and Setter ---

  unsigned char getHobbiesCount() const { return hobbiescount; }
  void setHobbiesCount(unsigned char ic)
  {
    hobbiescount = ic;
    delete [] hobbies;
    hobbies = new ListHobbies[ic];
  }

  // --- begin ListHobbies Getter and Setter ---

  ptString getHobbiesName(size_t i) { return hobbies[i].hobbiesname; }
  void setHobbiesName(size_t i, ptString x) { hobbies[i].hobbiesname = x; }

  unsigned int getHobbiesOrder(size_t i) { return hobbies[i].hobbiesorder; }
  void setHobbiesOrder(size_t i, unsigned int x) { hobbies[i].hobbiesorder = x; }

  unsigned int getHobbiesXP(size_t i) { return hobbies[i].hobbiesxp; }
  void setHobbiesXP(size_t i, unsigned int x) { hobbies[i].hobbiesxp = x; }

  // --- end ListHobbies Getter and Setter ---

  unsigned char getAbilitiesCount() const { return abilitiescount; }
  void setAbilitiesCount(unsigned char ic)
  {
    abilitiescount = ic;
    delete [] abilities;
    abilities = new ListAbilities[ic];
  }

  // --- begin ListAbilities Getter and Setter ---

  ptString getAbilitiesName(size_t i) { return abilities[i].abilitiesname; }
  void setAbilitiesName(size_t i, ptString x) { abilities[i].abilitiesname = x; }

  unsigned int getAbilitiesMin(size_t i) { return abilities[i].abilitiesmin; }
  void setAbilitiesMin(size_t i, unsigned int x) { abilities[i].abilitiesmin = x; }

  unsigned int getAbilitiesMax(size_t i) { return abilities[i].abilitiesmax; }
  void setAbilitiesMax(size_t i, unsigned int x) { abilities[i].abilitiesmax = x; }

  unsigned int getAbilitiesXP(size_t i) { return abilities[i].abilitiesxp; }
  void setAbilitiesXP(size_t i, unsigned int x) { abilities[i].abilitiesxp = x; }

  // --- end ListAbilities Getter and Setter ---

  unsigned char getVulberabilitiesCount() const { return vulberabilitiescount; }
  void setVulberabilitiesCount(unsigned char ic)
  {
    vulberabilitiescount = ic;
    delete [] vulberabilities;
    vulberabilities = new ListVulberabilities[ic];
  }

  // --- begin ListVulberabilities Getter and Setter ---

  ptString getVulberabilitiesName(size_t i) { return vulberabilities[i].vulberabilitiesname; }
  void setVulberabilitiesName(size_t i, ptString x) { vulberabilities[i].vulberabilitiesname = x; }

  unsigned int getVulberabilitiesMin(size_t i) { return vulberabilities[i].vulberabilitiesmin; }
  void setVulberabilitiesMin(size_t i, unsigned int x) { vulberabilities[i].vulberabilitiesmin = x; }

  unsigned int getVulberabilitiesMax(size_t i) { return vulberabilities[i].vulberabilitiesmax; }
  void setVulberabilitiesMax(size_t i, unsigned int x) { vulberabilities[i].vulberabilitiesmax = x; }

  unsigned int getVulberabilitiesValue(size_t i) { return vulberabilities[i].vulberabilitiesvalue; }
  void setVulberabilitiesValue(size_t i, unsigned int x) { vulberabilities[i].vulberabilitiesvalue = x; }

  // --- end ListVulberabilities Getter and Setter ---

};

#endif // USERMESSAGES_H
