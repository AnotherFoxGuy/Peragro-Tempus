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

#include "common/entity/itemmanager.h"
#include "server/network/network.h"
#include "server/usermanager.h"

void EntityHandler::handleMoveRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  std::string name = conn->getUser()->getEntity()->getName();
  MoveEntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("Received MoveRequest from: '%s' w(%d) t(%d)\n", name, request_msg.getWalk(), request_msg.getRot());

  MoveEntityMessage response_msg;
  response_msg.setWalk(request_msg.getWalk()*3);
  response_msg.setRot(request_msg.getRot());
  response_msg.setName(name);
  ByteStream bs;
  response_msg.serialise(&bs);
  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
      user->getConnection()->send(bs);
  }
}

void EntityHandler::handleDrUpdateRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;

  Entity* user_ent = conn->getUser()->getEntity();
  std::string name = user_ent->getName();

  UpdateDREntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("SetDR1: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", request_msg.getSpeed(), request_msg.getPos()[0], request_msg.getPos()[1], request_msg.getPos()[2], request_msg.getRot());
  //printf("Received UpdateDRRequest from: '%s'\n", name);

  user_ent->setPos(request_msg.getPos());
  user_ent->setSector(request_msg.getSector());

  UpdateDREntityMessage response_msg;
  response_msg.setSpeed(request_msg.getSpeed());
  response_msg.setRot(request_msg.getRot());
  response_msg.setAVel(request_msg.getAVel());
  response_msg.setVel(request_msg.getVel());
  response_msg.setWVel(request_msg.getWVel());
  response_msg.setPos(request_msg.getPos());
  response_msg.setOnGround(request_msg.getOnGround());
  response_msg.setSector(request_msg.getSector());
  response_msg.setName(name);
  ByteStream bs;
  response_msg.serialise(&bs);
  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
      user->getConnection()->send(bs);
  }
}

void EntityHandler::handlePickRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  Entity* user_ent = conn->getUser()->getEntity();
  std::string name = user_ent->getName();
  PickEntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received PickRequest from: '%s' -> '%d' \n", name.c_str(), request_msg.getTargetId());

  Entity* e = server->getEntityManager()->findById(request_msg.getTargetId());

  PickEntityResponseMessage response_msg;
  response_msg.setName(name);
  if (!e)
  {
    response_msg.setTarget(0);
    response_msg.setError("Entity doesn't exist");
  }
  else if (e->getType() == Entity::ItemEntity)
  {
    response_msg.setTarget(e->getName());
    response_msg.setError("");
  }
  else if (e->getType() == Entity::PlayerEntity)
  {
    response_msg.setTarget(e->getName());
    response_msg.setError("Don't pick on others!");
  }
  else
  {
    response_msg.setTarget(e->getName());
    response_msg.setError("Unpickable");
  }
  ByteStream bs;
  response_msg.serialise(&bs);
  for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
  {
    User* user = server->getUserManager()->getUser(i);
    if (user && user->getConnection())
      user->getConnection()->send(bs);
  }

  if (response_msg.getError().length() == 0)
  {
    Item* item = server->getItemManager()->findById(e->getItem());

    if (!item) return; //send Error message?

    user_ent->getInventory()->addItem(item,1);

    server->delEntity(e);
  }
}

void EntityHandler::handleDropRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  Entity* user_ent = conn->getUser()->getEntity();
  std::string name = user_ent->getName();
  DropEntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received DropRequest from: '%s' -> '%d' \n", name.c_str(), request_msg.getTargetId());

  Item* item = server->getItemManager()->findById(request_msg.getTargetId());

  if (!item)
  {
    //send Error message?
    return;
  }

  // Check if in Inventory
  bool canTake = user_ent->getInventory()->takeItem(item,1);

  if (!canTake)
  {
    //send Error message?
    return;
  }

  // Create new entity from item.
  Entity* e = new Entity();
  e->setPos(user_ent->getPos());
  e->setSector(user_ent->getSector());
  e->setType(Entity::ItemEntity);
  e->setMesh(item->getMesh());
  e->setName(item->getName());
  e->setItem(item->getId());

  server->addEntity(e);
}
