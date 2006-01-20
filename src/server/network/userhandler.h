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

#ifndef _USERHANDLER_H_
#define _USERHANDLER_H_

#include "stdio.h"

#include "common/network/messagehandler.h"
#include "common/network/usermessages.h"

#include "server/network/entityhandler.h"
#include "server/network/chathandler.h"

class UserHandler : public MessageHandler
{
private:
  Server* server;
  EntityHandler ent_handler;
  ChatHandler chat_handler;


public:
  UserHandler(Server* server) : server(server), ent_handler(server), chat_handler(server)
  {
  }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type == MESSAGES::USER)
    {
      char id = msg->getMsgId();

      if (id == USER::LOGIN_REQUEST) handleLoginRequest(msg);
      else if (id == USER::REGISTER_REQUEST) handleRegisterRequest(msg);
      else if (id == USER::CHARACTER_CREATION_REQUEST) handleCharCreationRequest(msg);
      else if (id == USER::CHARACTER_SELECTION_REQUEST) handleCharSelectionRequest(msg);
    }
    else if (type == MESSAGES::CHAT)
    {
      // No entity handling without Entity
      if (!msg->getConnection()->getUser())
        return;

      chat_handler.handle(msg);
    }
    else //Do not check type if the handler could have subhandlers
    {
      // No entity handling without Entity
      if (!msg->getConnection()->getUser())
        return;

      ent_handler.handle(msg);
    }
  }

  char getType()
  {
    return MESSAGES::USER;
  }

  void handleLoginRequest(GenericMessage* msg);
  void handleRegisterRequest(GenericMessage* msg);
  void handleCharCreationRequest(GenericMessage* msg);
  void handleCharSelectionRequest(GenericMessage* msg);
};

#endif // _USERHANDLER_H_
