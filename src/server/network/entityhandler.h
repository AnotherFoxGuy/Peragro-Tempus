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

#include "common/network/nwtypes.h"

#include "common/network/entitymessages.h"

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

  char getType() { return MESSAGES::ENTITY; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::ENTITY) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == ENTITY::MOVEREQUEST) handleMoveRequest(msg);
    else if (id == ENTITY::MOVETOREQUEST) handleMoveToRequest(msg);
    else if (id == ENTITY::PICKREQUEST) handlePickRequest(msg);
    else if (id == ENTITY::DROPREQUEST) handleDropRequest(msg);
    else if (id == ENTITY::RELOCATE) handleRelocate(msg);
    else if (id == ENTITY::TOGGLEFLASHSTEP) handleToggleFlashStep(msg);
    else if (id == ENTITY::DRUPDATEREQUEST) handleDrUpdateRequest(msg);
    else if (id == ENTITY::INVENTORYMOVEITEMREQUEST) handleInventoryMoveItemRequest(msg);
    else if (id == ENTITY::MOUNTREQUEST) handleMountRequest(msg);
    else if (id == ENTITY::UNMOUNTREQUEST) handleUnmountRequest(msg);
    else if (id == ENTITY::POSEREQUEST) handlePoseRequest(msg);
    else if (id == ENTITY::SPAWNITEM) handleSpawnItem(msg);
    else if (id == ENTITY::SPAWNMOUNT) handleSpawnMount(msg);
    else if (id == ENTITY::REMOVE) handleRemove(msg);
  }

  void handleMoveRequest(GenericMessage* msg);
  void handleMoveToRequest(GenericMessage* msg);
  void handlePickRequest(GenericMessage* msg);
  void handleDropRequest(GenericMessage* msg);
  void handleRelocate(GenericMessage* msg);
  void handleToggleFlashStep(GenericMessage* msg);
  void handleDrUpdateRequest(GenericMessage* msg);
  void handleInventoryMoveItemRequest(GenericMessage* msg);
  void handleMountRequest(GenericMessage* msg);
  void handleUnmountRequest(GenericMessage* msg);
  void handlePoseRequest(GenericMessage* msg);
  void handleSpawnItem(GenericMessage* msg);
  void handleSpawnMount(GenericMessage* msg);
  void handleRemove(GenericMessage* msg);
};

#endif // _ENTITYHANDLER_H_
