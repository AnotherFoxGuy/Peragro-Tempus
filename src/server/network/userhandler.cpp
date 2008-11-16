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

#include "server/entity/character.h"

#include "network.h"
#include "networkhelper.h"

#include "common/database/database.h"
#include "server/database/table-characters.h"
#include "server/database/table-users.h"
#include "server/entity/charactermanager.h"
#include "server/entity/entitymanager.h"
#include "server/entity/meshmanager.h"
#include "server/entity/racemanager.h"
#include "server/entity/usermanager.h"
#include "server/useraccountmanager.h"
#include "server/environment/environmentmanager.h"
#include "server/environment/clock.h"

void UserHandler::handleLoginRequest(GenericMessage* msg)
{
  LoginRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received LoginRequest\n");

  ptString username = request_msg.getUsername();
  const char* password = request_msg.getPassword();

  Server* server = Server::getServer();

  User* user = 0;
  ptString retval = server->getUserAccountManager()->login(username, password, user);

  LoginResponseMessage response_msg;
  response_msg.setError(retval);

  if (retval.isNull())
    response_msg.setIsAdmin(user->getPermissionList().getLevel(Permission::Admin) > 0);

  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  // If login error occured.
  if (!retval.isNull()) // retval != 0
    return;

  const Connection* old_conn = user->getConnection();

  if (old_conn) //User was already logged in
  {
    ptScopedMonitorable<Connection> c (old_conn);
    c->setUser(0);
  }
  else //new session
  {
    server->getUserManager()->addUser(user);
  }

  // Maybe check if the user was already loged in and then give it the character
  // that was previously selected instead and skip the character selection?
  Connection* conn = msg->getConnection();
  conn->setUser(user);
  user->setConnection(conn);

  Array<CharactersTableVO*> characters
    = server->getTables()->getCharacterTable()->getAllCharacters(user);
  CharListMessage char_msg;
  char_msg.setCharacterCount((char)characters.getCount());
  for (unsigned int i=0; i<characters.getCount(); i++)
  {
    CharactersTableVO* vo = characters.get(i);
    char_msg.setCharId(i, vo->id);
    char_msg.setName(i, vo->name);
    char_msg.setHairColour(i, vo->hair_r, vo->hair_g, vo->hair_b);
    char_msg.setSkinColour(i, vo->skin_r, vo->skin_g, vo->skin_b);
    char_msg.setDecalColour(i, vo->decal_r, vo->decal_g, vo->decal_b);
  }

  characters.delAll();

  ByteStream char_bs;
  char_msg.serialise(&char_bs);
  if (user->getConnection())
    user->getConnection()->send(char_bs);
}

void UserHandler::handleRegisterRequest(GenericMessage* msg)
{
  printf("Received RegisterRequest\n");
  RegisterRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  ptString username = request_msg.getUsername();
  const char* password = request_msg.getPassword();

  ptString retval = server->getUserAccountManager()->signup(username, password);

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

  const User* user = NetworkHelper::getUser(msg);
  //CharacterTable* ct = server->getTables()->getCharacterTable();

  ptString char_name = char_msg.getName();

  int char_id = 0;

  unsigned char* haircolour = char_msg.getHairColour();
  unsigned char* skincolour = char_msg.getSkinColour();
  unsigned char* decalcolour = char_msg.getDecalColour();

  Race* race = server->getRaceManager()->findByName(char_msg.getRaceName());

  // Register the new char
  ptString retval = server->getCharacterManager()->createCharacter(char_name, (int)user->getId(), char_id, race, haircolour, skincolour, decalcolour);

  // Send response message
  CharCreateResponseMessage response_msg;
  response_msg.setError(retval);
  response_msg.setCharId(retval.isNull()?char_id:0);
  response_msg.setName(retval.isNull()?char_name:ptString::Null);
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
  User* user = conn->getUser();

  CharSelectRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  // Assume the client knows nothing.
  user->clearEntityList();

  const PcEntity* entity = 0;
  const Character* character = 0;

  if (user->getEntity())
  {
    // User has already an entity loaded
    entity = user->getEntity();
    character = entity->getCharacter();

    if (character->getId() != request_msg.getCharId())
    {
      // User tries to login with a new character, remove the old.
      Server::getServer()->delEntity(entity->getEntity());
      entity = 0;
      character = 0;
    }
  }

  if (character == 0)
  {
    // Load new character and create an entity for it.
    CharacterManager* cmgr = Server::getServer()->getCharacterManager();
    Character* newchar = cmgr->getCharacter(request_msg.getCharId(), user);
    character = newchar;

    if (!character)
      return;

    newchar->setUser(user);

    PcEntity* newEntity = new PcEntity();
    entity = newEntity;

    newEntity->setCharacter(character);

    {
      ptScopedMonitorable<Entity> ent (entity->getEntity());
      ent->setName(character->getName());
      ent->setMesh(character->getMesh());

      printf("Adding Character '%s' with entity '%s'\n", *user->getName(), *entity->getEntity()->getName());
      user->setEntity(newEntity);

      ent->setRotation(character->getRotation());
      ent->setSector(character->getSector());
      ent->setPos(character->getPos());

      newchar->getInventory()->loadFromDatabase(server->getTables()->getInventoryTable(), character->getId());
      newchar->getStats()->loadFromDatabase(server->getTables()->getCharacterStatTable(), character->getId());
      newchar->getSkills()->loadFromDatabase(server->getTables()->getCharacterSkillsTable(), character->getId());
      newchar->getReputation()->loadFromDatabase(server->getTables()->getCharacterReputationsTable(), character->getId());
    } // Release lock on ent.

    server->addEntity(entity->getEntity(), false);
  }

  CharSelectResponseMessage response_msg;
  response_msg.setError(ptString::Null);
  response_msg.setEntityId(entity->getEntity()->getId());
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  ptScopedMonitorable<Character> lockedChar (character);
  lockedChar->getInventory()->sendAllItems(msg->getConnection());
  lockedChar->getStats()->sendAllStats(msg->getConnection());
  lockedChar->getSkills()->sendAllSkills(msg->getConnection());

  server->getEnvironmentManager()->GetClock()->InitTime(entity->getEntity());
}

void UserHandler::handleMeshListRequest(GenericMessage* msg)
{
  Server* server = Server::getServer();

  std::vector<const Mesh*> meshes;
  server->getMeshManager()->findChangedMeshSince(0, meshes);

  MeshListResponseMessage response;

  if (meshes.size() > 255) return; // TODO: Fix me

  response.setMeshesCount((unsigned char) meshes.size());
  for (size_t i=0; i < meshes.size(); i++)
  {
    const Mesh* mesh = meshes[i];
    response.setMeshId(i, mesh->getId());
    response.setMeshName(i, mesh->getName());
    response.setFileName(i, mesh->getFile());
  }

  ByteStream bs;
  response.serialise(&bs);
  msg->getConnection()->send(bs);
}

void UserHandler::handleRaceListRequest(GenericMessage* msg)
{
  Server* server = Server::getServer();

  size_t raceCount = server->getRaceManager()->getRaceCount();

  RaceListResponseMessage response;

  response.setRacesCount(raceCount);
  for (size_t i=0; i< raceCount; i++)
  {
    Race* race = server->getRaceManager()->getRace(i);
    response.setRaceId(i, race->getId());
    response.setRaceName(i, race->getName());
    response.setMeshId(i, race->getMesh()->getId());
  }

  ByteStream bs;
  response.serialise(&bs);
  msg->getConnection()->send(bs);
}
