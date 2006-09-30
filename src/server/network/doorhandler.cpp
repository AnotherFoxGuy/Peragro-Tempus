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

#include "server/network/network.h"
#include "server/usermanager.h"

void DoorHandler::handleOpenDoorRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  const char* error = 0;
  OpenDoorRequestMessage door_msg;
  door_msg.deserialise(msg->getByteStream());
  printf("Got open door request %d: \n---------------------------\n", door_msg.getDoorId());
  DoorEntity* door = (DoorEntity*)server->getEntityManager()->findById(door_msg.getDoorId());
  if (door->getLocked())
  {
    error = "The Door is Locked";
  }
  ptString pt_err = error?ptString(error, strlen(error)):ptString();
  OpenDoorResponseMessage response_msg;
  response_msg.setDoorId(door_msg.getDoorId());
  response_msg.setError(pt_err);
  ByteStream bs;
  response_msg.serialise(&bs);
  if (!error)
  {
    // Tell all about success or just the client if was closed
    if (door->getOpen())
      conn->send(bs);
    else
    {
      door->setOpen(true);
      Server::getServer()->broadCast(bs);
    }
  }
  else
  {
    // Tell only one about error
    conn->send(bs);
  }
}

void DoorHandler::handleCloseDoorRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  const char* error = 0;
  CloseDoorRequestMessage door_msg;
  door_msg.deserialise(msg->getByteStream());
  printf("Got close door request %d: \n---------------------------\n", door_msg.getDoorId());
  DoorEntity* door = (DoorEntity*)server->getEntityManager()->findById(door_msg.getDoorId());
  if (door->getLocked())
  {
    error = "The Door is Locked";
  }
  ptString pt_err = error?ptString(error, strlen(error)):ptString();
  CloseDoorResponseMessage response_msg;
  response_msg.setDoorId(door_msg.getDoorId());
  response_msg.setError(pt_err);

  ByteStream bs;
  response_msg.serialise(&bs);
  if (!error)
  {
    // Tell all about success or just the client if was closed
    if (!door->getOpen())
      conn->send(bs);
    else
    {
      door->setOpen(false);
      Server::getServer()->broadCast(bs);
    }
  }
  else
  {
    // Tell only one about error
    conn->send(bs);
  }
}

void DoorHandler::handleLockDoorRequest(GenericMessage* msg)
{
  // TODO: implement
}

void DoorHandler::handleUnlockDoorRequest(GenericMessage* msg)
{
  // TODO: implement
}
