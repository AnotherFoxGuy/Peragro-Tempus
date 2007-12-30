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

#ifndef _ADMINHANDLER_H_
#define _ADMINHANDLER_H_

#include "common/network/nwtypes.h"

#include "common/network/adminmessages.h"

class Server;

class AdminHandler : public MessageHandler
{
private:
  Server* server;

public:
  AdminHandler(Server* server)
  : server(server)
  {
  }

  char getType() { return MESSAGES::ADMIN; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::ADMIN) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == ADMIN::CREATESECTOR) handleCreateSector(msg);
    else if (id == ADMIN::CREATEITEM) handleCreateItem(msg);
    else if (id == ADMIN::REMOVEALL) handleRemoveAll(msg);
    else if (id == ADMIN::CREATENPC) handleCreateNpc(msg);
    else if (id == ADMIN::CREATESPAWNPOINT) handleCreateSpawnPoint(msg);
  }

  void handleCreateSector(GenericMessage* msg);
  void handleCreateItem(GenericMessage* msg);
  void handleRemoveAll(GenericMessage* msg);
  void handleCreateNpc(GenericMessage* msg);
  void handleCreateSpawnPoint(GenericMessage* msg);
};

#endif // _ADMINHANDLER_H_
