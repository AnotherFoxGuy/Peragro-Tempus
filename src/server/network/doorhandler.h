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

#ifndef _DOORHANDLER_H_
#define _DOORHANDLER_H_

#include "common/network/nwtypes.h"

#include "common/network/doormessages.h"

class Server;

class DoorHandler : public MessageHandler
{
private:
  Server* server;

public:
  DoorHandler(Server* server)
  : server(server)
  {
  }

  char getType() { return MESSAGES::DOOR; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::DOOR) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == DOOR::OPENDOORREQUEST) handleOpenDoorRequest(msg);
    else if (id == DOOR::CLOSEDOORREQUEST) handleCloseDoorRequest(msg);
    else if (id == DOOR::LOCKDOORREQUEST) handleLockDoorRequest(msg);
    else if (id == DOOR::UNLOCKDOORREQUEST) handleUnlockDoorRequest(msg);
  }

  void handleOpenDoorRequest(GenericMessage* msg);
  void handleCloseDoorRequest(GenericMessage* msg);
  void handleLockDoorRequest(GenericMessage* msg);
  void handleUnlockDoorRequest(GenericMessage* msg);
};

#endif // _DOORHANDLER_H_
