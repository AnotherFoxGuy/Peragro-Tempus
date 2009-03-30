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

#include "network.h"
#include "networkhelper.h"

#include "server/database/tablemanager.h"
#include "server/database/table-characters.h"
#include "server/database/table-pcentities.h"
#include "server/database/table-users.h"
#include "server/entity/entitymanager.h"
#include "server/entity/usermanager.h"
#include "server/environment/environmentmanager.h"
#include "server/environment/clock.h"

#include "server/entity/entitymanager.h"
#include "server/entity/pcentity.h"

void UserHandler::handleLoginRequest(GenericMessage* msg)
{
  LoginRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received LoginRequest\n");

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
    char_msg.setHairColour(i, vo->hairColor[0], vo->hairColor[1], vo->hairColor[2]);
    char_msg.setSkinColour(i, vo->skinColor[0], vo->skinColor[1], vo->skinColor[2]);
    char_msg.setDecalColour(i, vo->decalColor[0], vo->decalColor[1], vo->decalColor[2]);
  }

  ByteStream char_bs;
  char_msg.serialise(&char_bs);
  if (user->GetConnection())
    user->GetConnection()->send(char_bs);
}

void UserHandler::handleRegisterRequest(GenericMessage* msg)
{
  printf("Received RegisterRequest\n");
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
  unsigned char* haircolour = char_msg.getHairColour();
  unsigned char* skincolour = char_msg.getSkinColour();
  unsigned char* decalcolour = char_msg.getDecalColour();

  Entityp entity = server->getEntityManager()->CreateNew(Common::Entity::PCEntityType);
  PcEntity* pc = dynamic_cast<PcEntity*>(entity.get());
  pc->SetName(*char_name);
  pc->SetUser(user.get());
  //TODO
  pc->SetMeshName("test");
  pc->SetFileName("/peragro/art/3d_art/characters/male01/male01");
  pc->SetPosition(900.765f, 8.26544f, 11.1211f);
  pc->SaveToDB();

  // Send response message
  CharCreateResponseMessage response_msg;
  response_msg.setError(ptString::Null);
  response_msg.setCharId(pc->GetId());
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
      printf("I: handleCharSelectRequest(): User '%s' switching to char %d!\n", user->GetName(), request_msg.getCharId());
      entity.reset();
    }
    else
    {
      printf("I: handleCharSelectRequest(): User '%s' continuing with char %d!\n", user->GetName(), request_msg.getCharId());
      server->getEntityManager()->Add(entity);
      return;
    }    
  }

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
    boost::shared_ptr<PcEntity> pc = boost::shared_dynamic_cast<PcEntity>(entity);
    
    try
    {
      entity->LoadFromDB();
    }
    catch(const char*)
    {
      printf("E: handleCharSelectRequest(): Invalid entity '%d'!\n", request_msg.getCharId());
      return;
    }

    user->SetEntity(pc);
  }

  server->getEntityManager()->Add(entity);

  CharSelectResponseMessage response_msg;
  response_msg.setError(ptString::Null);
  response_msg.setEntityId(entity->GetId());
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);


  PcEntity* pc = dynamic_cast<PcEntity*>(entity.get());
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

