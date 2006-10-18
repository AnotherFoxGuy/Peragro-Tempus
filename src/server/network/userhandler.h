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

#include "common/network/nwtypes.h"

#include "common/network/usermessages.h"

class Server;

class UserHandler : public MessageHandler
{
private:
  Server* server;

public:
  UserHandler(Server* server)
  : server(server)
  {
  }

  char getType() { return MESSAGES::USER; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::USER) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == USER::LOGINREQUEST) handleLoginRequest(msg);
    else if (id == USER::REGISTERREQUEST) handleRegisterRequest(msg);
    else if (id == USER::CHARCREATEREQUEST) handleCharCreateRequest(msg);
    else if (id == USER::CHARSELECTREQUEST) handleCharSelectRequest(msg);
  }

  void handleLoginRequest(GenericMessage* msg);
  void handleRegisterRequest(GenericMessage* msg);
  void handleCharCreateRequest(GenericMessage* msg);
  void handleCharSelectRequest(GenericMessage* msg);
};

#endif // _USERHANDLER_H_
