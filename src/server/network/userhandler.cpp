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

#include "server/entity/character/character.h"

#include "common/utility/diagnosticinformation.h"

#include "server/network/userhandler.h"
#include "networkhelper.h"

#include "server/database/tablemanager.h"
#include "server/database/table-characters.h"
#include "server/database/table-pcentities.h"
#include "server/database/table-users.h"
#include "server/database/table-avatartemplates.h"
#include "server/database/table-meshes.h"

#include "server/entity/entitymanager.h"
#include "server/entity/usermanager.h"
#include "server/environment/environmentmanager.h"
#include "server/environment/clock.h"

#include "server/entity/entitymanager.h"
#include "server/entity/pcentity.h"

#include "server/species/speciesmanager.h"
#include "common/database/database.h"

void UserHandler::handleLoginRequest(GenericMessage* msg)
{
  LoginRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  ptString username = request_msg.getUsername();
  const char* password = request_msg.getPassword();

  Server* server = Server::getServer();

  boost::shared_ptr<User> user;
  ptString retval = server->getUserManager()->Login(*username, password, user);

  LoginResponseMessage response_msg;
  response_msg.setError(retval);

  if (retval.isNull())
    response_msg.setIsAdmin(user->getPermissionList().getLevel(Permission::Admin) > 0);

  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  // If login error occured.
  if (!retval.isNull()) return;

  const Connection* old_conn = user->GetConnection();

  if (old_conn) //User was already logged in
  {
    ptScopedMonitorable<Connection> c (old_conn);
    c->SetUser(boost::shared_ptr<User>());
  }

  // Maybe check if the user was already logged in and then give it the character
  // that was previously selected instead and skip the character selection?
  Connection* conn = msg->getConnection();
  conn->SetUser(user);
  user->SetConnection(conn);

  CharactersTable* ctable = server->GetTableManager()->Get<CharactersTable>();
  PcEntitiesTable* pctable = server->GetTableManager()->Get<PcEntitiesTable>();
  PcEntitiesTableVOArray pcs = pctable->GetAll(user->GetName());
  CharListMessage char_msg;
  char_msg.setCharacterCount((char)pcs.size());
  for (size_t i=0; i < pcs.size(); i++)
  {
    CharactersTableVOp vo = ctable->GetSingle(pcs[i]->entity_id);
    char_msg.setCharId(i, vo->entity_id);
    char_msg.setName(i, vo->name);
    //char_msg.setMesh(i, vo->meshes_id);
    char_msg.setHairColour(i, vo->hairColor);
    char_msg.setSkinColour(i, vo->skinColor);
    char_msg.setDecalColour(i, vo->decalColor);
  }

  ByteStream char_bs;
  char_msg.serialise(&char_bs);
  if (user->GetConnection())
    user->GetConnection()->send(char_bs);
}

void UserHandler::handleRegisterRequest(GenericMessage* msg)
{
  RegisterRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  ptString username = request_msg.getUsername();
  const char* password = request_msg.getPassword();

  ptString retval = server->getUserManager()->Signup(*username, password);

  RegisterResponseMessage response_msg;
  response_msg.setError(retval);
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);
}

void UserHandler::handleCharCreateRequest(GenericMessage* msg)
{
  CharCreateRequestMessage char_msg;
  char_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  boost::shared_ptr<User> user = NetworkHelper::getUser(msg);

  ptString char_name = char_msg.getName();
  unsigned int avatarTemplateID = char_msg.getAvatarTemplateID();
  PT::Colour24 haircolour(char_msg.getHairColour());
  PT::Colour24 skincolour(char_msg.getSkinColour());
  PT::Colour24 decalcolour(char_msg.getDecalColour());
  ///@TODO : Create a CharacterHandler and move this stuff to it.
  // Get AvatarInfo
  AvatarTemplatesTable* atable = server->GetTableManager()->Get<AvatarTemplatesTable>();
  AvatarTemplatesTableVOp avatarTemplate = atable->GetSingle(avatarTemplateID );
  if (!avatarTemplate) return;
  // Get Mesh Name
  MeshesTable* mtable = server->GetTableManager()->Get<MeshesTable>();
  MeshesTableVOp mesh = mtable->GetSingle(avatarTemplate->mesh_id );
  if (!mesh) return;

  ptString error(ptString::Null);
  Common::Entity::Entity::IdType pcId = Common::Entity::Entity::NoEntity;
  try
  {
    boost::shared_ptr<PcEntity> pc =
      server->GetSpeciesManager()->CreatePCFromSpecies(avatarTemplate->species_id);
    pc->SetName(*char_name);
    pc->SetUser(user.get());
    pc->SetMeshName(mesh->factoryName);
    pc->SetFileName(mesh->fileName);
    pc->SaveToDB();

    pcId = pc->GetId();

    pc->SetHairColour(haircolour);
    pc->SetSkinColour(skincolour);
    pc->SetDecalColour(decalcolour);
  }
  catch (std::exception& ex)
  {
    std::cout << "Failed to create character" << std::endl <<
      PT::DiagnosticInformation(ex);

    error = ex.what();
  }

  // Send response message
  CharCreateResponseMessage response_msg;
  response_msg.setError(error);
  response_msg.setCharId(pcId);
  response_msg.setName(char_name);
  response_msg.setHairColour(haircolour);
  response_msg.setSkinColour(skincolour);
  response_msg.setDecalColour(decalcolour);
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);
}

void UserHandler::handleCharSelectRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  boost::shared_ptr<User> user = conn->GetUser();

  CharSelectRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  // Assume the client knows nothing.
  user->ClearKnownEntitites();

  Entityp entity;
  if (user->GetEntity())
  {
    // User has already an entity loaded
    entity = user->GetEntity();
    Server::getServer()->getEntityManager()->Remove(entity);

    if (entity->GetId() != request_msg.getCharId())
    {
      // User tries to login with a new character, remove the old.
      printf("I: handleCharSelectRequest(): User '%s' switching to char %d!\n", user->GetName().c_str(), request_msg.getCharId());
      entity.reset();
    }
    else
    {
      printf("I: handleCharSelectRequest(): User '%s' continuing with char %d!\n", user->GetName().c_str(), request_msg.getCharId());
      server->getEntityManager()->Add(entity);
      return;
    }
  }

  boost::shared_ptr<PcEntity> pc;
  if (!entity)
  {
    // Check if the send Id is valid and the entity belongs to that user.
    PcEntitiesTable* pctable = server->GetTableManager()->Get<PcEntitiesTable>();
    PcEntitiesTableVOArray pcs = pctable->GetAll(user->GetName());
    bool validId = false;
    PcEntitiesTableVOArray::const_iterator it = pcs.begin();
    for ( ; it != pcs.end(); it++)
    {
      if ((*it)->entity_id == request_msg.getCharId())
      {
        validId = true;
        break;
      }
    }
    if (!validId)
    {
      printf("E: handleCharSelectRequest(): entity '%d' does not belong to user '%s'!\n", request_msg.getCharId(), user->GetName().c_str());
      return;
    }

    entity = server->getEntityManager()->CreateNew(Common::Entity::PCEntityType, request_msg.getCharId());
    pc = boost::shared_polymorphic_downcast<PcEntity>(entity);

    entity->LoadFromDB();

    user->SetEntity(pc);
  }

  server->getEntityManager()->Add(entity);

  CharSelectResponseMessage response_msg;
  response_msg.setError(ptString::Null);
  response_msg.setEntityId(entity->GetId());
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  if (!pc) pc = boost::shared_polymorphic_downcast<PcEntity>(entity);
  pc->GetResources()->SendAll(msg->getConnection());
/*
  pc->getInventory()->sendAllItems(msg->getConnection());

  pc->getStats()->sendAllStats(msg->getConnection());
  pc->getSkills()->sendAllSkills(msg->getConnection());

  server->getEnvironmentManager()->GetClock()->InitTime(entity.get());
*/
}

void UserHandler::handleMeshListRequest(GenericMessage* msg)
{
  /*
  Server* server = Server::getServer();

  std::vector<const Mesh*> meshes;
  server->getMeshManager()->findChangedMeshSince(0, meshes);

  MeshListResponseMessage response;

  if (meshes.size() > 255) return; // TODO: Fix me

  response.setMeshesCount((unsigned char) meshes.size());
  for (size_t i=0; i < meshes.size(); i++)
  {
    const Mesh* mesh = meshes[i];
    response.setMeshId(i, mesh->GetId());
    response.setMeshName(i, mesh->getName());
    response.setFileName(i, mesh->getFile());
  }

  ByteStream bs;
  response.serialise(&bs);
  msg->getConnection()->send(bs);
  */
}

void UserHandler::handleAvatarListRequest(GenericMessage* msg)
{

  AvatarTemplatesTableVOArray avatarList;
  AvatarTemplatesTable* avatarTable;

  avatarTable = Server::getServer()->GetTableManager()->Get<AvatarTemplatesTable>();
  avatarList = avatarTable->GetAll();
  if (avatarList.size() > 255) return; // TODO: Fix me

  AvatarListResponseMessage response;
  response.setAvatarCount((char) avatarList.size());
  for (size_t i=0; i < avatarList.size(); i++)
  {
    response.setAvatarId(i, avatarList[i]->id );
    response.setAvatarName(i, avatarList[i]->name);
  }

  ByteStream bs;
  response.serialise(&bs);
  msg->getConnection()->send(bs);
} // end handleAvatarListRequest

void UserHandler::handleAvatarInfoRequest(GenericMessage* msg)
{

  AvatarInfoRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());

  AvatarTemplatesTable* avatarTable;
  avatarTable = Server::getServer()->GetTableManager()->Get<AvatarTemplatesTable>();
  AvatarTemplatesTableVOp avatarTemplate = avatarTable->GetSingle(requestMsg.getAvatarId());
  if (!avatarTemplate)
  {
    printf("Invalid avatarID\n");
    return;
  }

  AvatarInfoResponseMessage response;
  response.setAvatarId(avatarTemplate->id);
  response.setMeshId(avatarTemplate->mesh_id);
  response.setSpeciesId(avatarTemplate->species_id);
  response.setAvatarName(avatarTemplate->name);


  MeshesTable* meshesTable;
  meshesTable = Server::getServer()->GetTableManager()->Get<MeshesTable>();
  MeshesTableVOp mesh = meshesTable->GetSingle(avatarTemplate->mesh_id);
  if (!mesh)
  {
    printf("Invalid mesh_id \n");
    return;
  }
  response.setfactoryName(mesh->factoryName);
  response.setfileName(mesh->fileName);

  Database* db =  Server::getServer()->getDatabase();
  std::string sql = "SELECT sa.Species_id, sa.AbilityTypes_id, at.name, sa.minXP, sa.maxXP " \
                            "FROM SpeciesAbilities sa , AbilityTypes at " \
                            "WHERE  sa.AbilityTypes_id = at.id " \
                            "AND sa.Species_id = ";
  std::stringstream s;
  s << avatarTemplate->species_id;
  sql += s.str();
  sql += ";";
  ResultSet* rs = db->Query(sql.c_str());
  if (!rs) return;
  if ( rs->GetRowCount() > 255 ) return;
  response.setAbilitiesCount((char) rs->GetRowCount());

  std::string name;
  int min;
  int max;
  int xp;
  for (size_t i = 0; rs && i < rs->GetRowCount(); i++)
  {
    DB::Helper::Convert(name, rs->GetData(i, 2));
    DB::Helper::Convert(min, rs->GetData(i, 3));
    DB::Helper::Convert(max, rs->GetData(i, 4));
    DB::Helper::Convert(xp, rs->GetData(i, 4));

    response.setAbilitiesName(i, name);
    response.setAbilitiesMin(i, min);
    response.setAbilitiesMax(i, max);
    response.setAbilitiesXP(i, xp);
  }

  delete rs;

  //@@@ TODO Add other species info to AvatarInfoResponseMessage
  response.setInventoryCount((char) 0);
  response.setEquipmentCount((char) 0);
  response.setReputationsCount((char) 0);
  response.setSkillsCount((char) 0);
  response.setHobbiesCount((char) 0);
  response.setVulberabilitiesCount((char) 0);

  // Send response
  ByteStream bs;
  response.serialise(&bs);
  msg->getConnection()->send(bs);
} // end handleAvatarInfoRequest

