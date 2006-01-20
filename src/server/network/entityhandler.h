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

#ifndef _ENTITYHANDLER_H_
#define _ENTITYHANDLER_H_

#include "stdio.h"

#include "common/network/messagehandler.h"
#include "common/network/netmessage.h"
#include "common/network/entitymessages.h"

//#include "connectionmanager.h"
#include "common/entity/entitymanager.h"

#include "server/user.h"

class Server;

class EntityHandler : public MessageHandler
{
private:
  Server* server;

public:
  EntityHandler(Server* server) 
  : server(server)
  {
  }

  void handle(GenericMessage* msg)
  {
    // No user handling without User
    if (!msg->getConnection()->getUser()->getEntity())
      return;

    char type = msg->getMsgType();
    if (type != MESSAGES::ENTITY) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == ENTITY::MOVE_REQUEST) handleMoveRequest(msg);
    if (id == ENTITY::DRUPDATE_REQUEST) handleDrUpdateRequest(msg);
    if (id == ENTITY::PICK_REQUEST) handlePickRequest(msg);
    if (id == ENTITY::DROP_REQUEST) handleDropRequest(msg);
  }

  char getType() const
  {
    return MESSAGES::ENTITY;
  }

  void handleMoveRequest(GenericMessage* msg);
  void handleDrUpdateRequest(GenericMessage* msg);
  void handlePickRequest(GenericMessage* msg);
  void handleDropRequest(GenericMessage* msg);
};

#endif // _ENTITYHANDLER_H_
