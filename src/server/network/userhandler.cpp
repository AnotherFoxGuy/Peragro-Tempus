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

#include <string>

#include "common/entity/character.h"

#include "server/network/network.h"
#include "server/database/database.h"
#include "server/database/table-characters.h"
#include "server/database/table-users.h"
#include "server/charactermanager.h"
#include "server/usermanager.h"
#include "server/useraccountmanager.h"

void UserHandler::handleLoginRequest(GenericMessage* msg)
{
  LoginRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received LoginRequest\n");

  std::string username = request_msg.getName();
  std::string password = request_msg.getPwHash();

  if (username == "" || password == "") return;

  User* user = 0;
  const char* retval = server->getUserAccountManager()->login(username, password, user);

  LoginResponseMessage response_msg;
  response_msg.setError(retval ? retval : "");
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  // If login error occured.
  if (retval)
    return;

  server->getUserManager()->addUser(user);

  // Maybe check if the user was already loged in and then give it the character
  // that was previously selected instead and skip the character selection?
  Connection* old_conn = user->getConnection();
  Connection* conn = msg->getConnection();
  conn->setUser(user);
  user->setConnection(conn);

  std::vector<Character*> characters;
  server->getDatabase()->getCharacterTable()->getAllCharacters(characters, user);
  CharacterListMessage char_msg;
  char_msg.setCharacterCount(characters.size());
  for (size_t i = 0; i < characters.size(); i++)
  {
    char_msg.setCharacterId(i, characters[i]->getId());
    char_msg.setCharacterName(i, characters[i]->getName());
  }

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

  std::string username = request_msg.getName();
  std::string password = request_msg.getPwHash();

  const char* retval = server->getUserAccountManager()->signup(username, password);

  RegisterResponseMessage response_msg;
  response_msg.setError(retval ? retval : "");
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);
}

void UserHandler::handleCharCreationRequest(GenericMessage* msg)
{
  CharacterCreationRequestMessage char_msg;
  char_msg.deserialise(msg->getByteStream());

  User* user = msg->getConnection()->getUser();
  CharacterTable* ct = server->getDatabase()->getCharacterTable();

  std::string char_name = char_msg.getName();

  const char* retval;
  int char_id = 0;

  // Register the new char
  retval = server->getCharacterManager()->createCharacter(char_name, user->getId(), char_id);

  // Send response message
  CharacterCreationResponseMessage response_msg;
  response_msg.setError(retval ? retval : "");
  response_msg.setCharacterId(retval ? 0 : char_id);
  response_msg.setCharacterName(retval ? "" : char_name);
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);
}

void UserHandler::handleCharSelectionRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  User* user = conn->getUser();

  CharacterSelectionRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  // Assume the client knows nothing.
  user->clearEntityList();

  Character* character = 
    server->getDatabase()->getCharacterTable()->findCharacterById(request_msg.getCharId());

  if (!character)
    return;

  Entity* entity = new Entity();
  entity->setName(character->getName());
  entity->setMesh(character->getMesh());
  entity->setSector(character->getSector());
  entity->setType(Entity::PlayerEntity);
  entity->getInventory()->loadFromDatabase(server->getDatabase()->getInventoryTable(), character->getId());
  printf("Adding Character '%s' with entity '%s'\n", user->getName().c_str(), entity->getName().c_str());
  user->setEntity(entity);
  server->addEntity(entity);

  CharacterSelectionResponseMessage response_msg;
  response_msg.setError("");
  response_msg.setEntityId(entity->getId());
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  /// \todo Implement message packing (all entities in one package).
  for (size_t i=0; i<server->getEntityManager()->getEntityCount(); i++)
  {
    user->sendAddEntity(server->getEntityManager()->getEntity(i));
  }
}
