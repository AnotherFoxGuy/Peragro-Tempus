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

#include "server/database/database.h"
#include "server/database/table-characters.h"
#include "server/database/table-users.h"
#include "server/entity/charactermanager.h"
#include "server/entity/usermanager.h"
#include "server/useraccountmanager.h"

void UserHandler::handleLoginRequest(GenericMessage* msg)
{
  LoginRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received LoginRequest\n");

  ptString username = request_msg.getUsername();
  const char* password = request_msg.getPassword();

  if (username.isNull() || !password) return;

  User* user = 0;
  ptString retval = server->getUserAccountManager()->login(username, password, user);

  LoginResponseMessage response_msg;
  response_msg.setError(retval);
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  // If login error occured.
  if (!retval.isNull()) // retval != 0
    return;

  const Connection* old_conn = user->getConnection();

  if (old_conn) //User was already logged in
  {
    Connection* c = old_conn->getLock();
    c->setUser(0);
    c->freeLock();
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

  Array<Character*> characters;
  server->getDatabase()->getCharacterTable()->getAllCharacters(characters, user);
  CharListMessage char_msg;
  char_msg.setCharacterCount((char)characters.getCount());
  for (unsigned int i=0; i<characters.getCount(); i++)
  {
    char_msg.setId(i, characters.get(i)->getId());
    char_msg.setName(i, characters.get(i)->getName());
    char_msg.setDecalColour(i, characters.get(i)->getDecalColour());
    char_msg.setHairColour(i, characters.get(i)->getHairColour());
    char_msg.setSkinColour(i, characters.get(i)->getSkinColour());
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

  ptString username = request_msg.getUsername();
  const char* password = request_msg.getPassword();

  ptString retval = server->getUserAccountManager()->signup(username, password);

  RegisterResponseMessage response_msg;
  response_msg.setError(retval);
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);
}

void UserHandler::handleCharCreationRequest(GenericMessage* msg)
{
  CharCreateRequestMessage char_msg;
  char_msg.deserialise(msg->getByteStream());

  User* user = NetworkHelper::getUser(msg);
  //CharacterTable* ct = server->getDatabase()->getCharacterTable();

  ptString char_name = char_msg.getName();

  int char_id = 0;

  unsigned char* haircolour = char_msg.getHairColour();
  unsigned char* skincolour = char_msg.getSkinColour();
  unsigned char* decalcolour = char_msg.getDecalColour();

  // Register the new char
  ptString retval = server->getCharacterManager()->createCharacter(char_name, (int)user->getId(), char_id, haircolour, skincolour, decalcolour);

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

void UserHandler::handleCharSelectionRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  User* user = conn->getUser();

  CharSelectRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  // Assume the client knows nothing.
  user->clearEntityList();

  Character* character = 
    server->getDatabase()->getCharacterTable()->findCharacterById(request_msg.getCharId(), user->getId());

  if (!character)
    return;

  character->setUser(user);

  PcEntity* entity = new PcEntity();
  entity->setCharacter(character);
  entity->setCharId(request_msg.getCharId());
  entity->setName(character->getName());
  entity->setMesh(character->getMesh());
  entity->setPos(character->getPos());
  entity->setSector(character->getSector());
  printf("Adding Character '%s' with entity '%s'\n", *user->getName(), *entity->getName());
  user->setEntity(entity);
  server->addEntity(entity, false);

  CharSelectResponseMessage response_msg;
  response_msg.setError(ptString::Null);
  response_msg.setCharEntityId(entity->getId());
  ByteStream bs;
  response_msg.serialise(&bs);
  msg->getConnection()->send(bs);

  entity->getInventory()->loadFromDatabase(server->getDatabase()->getInventoryTable(), character->getId());
  entity->getStats()->loadFromDatabase(server->getDatabase()->getCharacterStatTable(), character->getId());
  entity->getSkills()->loadFromDatabase(server->getDatabase()->getCharacterSkillsTable(), character->getId());

  entity->getInventory()->sendAllItems(msg->getConnection());
  entity->getStats()->sendAllStats(msg->getConnection());
  entity->getSkills()->sendAllSkills(msg->getConnection());

  /// \todo Implement message packing (all entities in one package).
  for (size_t i=0; i<server->getEntityManager()->getEntityCount(); i++)
  {
    user->sendAddEntity(server->getEntityManager()->getEntity(i));
  }
}
