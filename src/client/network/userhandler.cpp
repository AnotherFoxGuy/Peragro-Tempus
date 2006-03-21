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

#include "client/network/network.h"
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

void UserHandler::handleLoginResponse(GenericMessage* msg)
{
  printf("Received LoginResponse\n");
  LoginResponseMessage response;
  response.deserialise(msg->getByteStream());
  ptString error = response.getError();
  if (!error.isNull())
  {
    printf("Login Failed due to: %s\n", *error);
    guimanager = client->GetGuiManager();
    guimanager->GetLoginWindow()->EnableWindow();
    return;
  }
  printf("Login succeeded!\n");
  client->loggedIn();
}

void UserHandler::handleRegisterResponse(GenericMessage* msg)
{
  printf("Received RegisterResponse\n");
  RegisterResponseMessage answer_msg;
  answer_msg.deserialise(msg->getByteStream());
  ptString error = answer_msg.getError();
  if (!error.isNull())
  {
    printf("Registration Failed due to: %s\n", *error);
    return;
  }

  printf("Registration succeeded!\n");
}

void UserHandler::handleCharList(GenericMessage* msg)
{
  CharacterListMessage char_msg;
  char_msg.deserialise(msg->getByteStream());
  //printf("Got %d character:\n---------------------------\n", char_msg.getCharacterCount());
  guimanager = client->GetGuiManager();
  for (int i=0; i<char_msg.getCharacterCount(); i++)
  {
    guimanager->GetSelectCharWindow ()->AddCharacter(char_msg.getCharacterId(i), *char_msg.getCharacterName(i));
    //printf("Char %d: %s\n", char_msg.getCharacterId(i), char_msg.getCharacterName(i));
  }
}

void UserHandler::handleCharCreationResponse(GenericMessage* msg)
{
  CharacterCreationResponseMessage answer_msg;
  answer_msg.deserialise(msg->getByteStream());
  guimanager = client->GetGuiManager();

  if (answer_msg.getError().isNull())
  {
    guimanager->GetSelectCharWindow ()->AddCharacter(answer_msg.getCharacterId(), *answer_msg.getCharacterName());
  }
  else
  {
    printf("Character creation failed due to: %s\n", *answer_msg.getError());
  }
}

void UserHandler::handleCharSelectionResponse(GenericMessage* msg)
{
  client->loadRegion("/peragro/art/world");
  CharacterSelectionResponseMessage answer_msg;
  answer_msg.deserialise(msg->getByteStream());
  client->GetEntityManager()->setCharacter(answer_msg.getEntityId());
}
