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

#include "user.h"
#include "server/server.h"
#include "server/usermanager.h"
#include "server/network/network.h"

void User::sendAddEntity(Entity* entity)
{
  if (ent_list.exists(entity))
    return;

  if (!entity || !*(entity->getName()))
    return;

  printf("send addentity '%s' to '%s'\n", *entity->getName(), *this->getName());

  ent_list.addEntity(entity);
  ByteStream bs;
  if (entity->getType() == Entity::DoorEntity)
  {
    AddDoorMessage msg;
    msg.setName(entity->getName());
    msg.setId(entity->getId());
    msg.setMesh(entity->getMesh());
    msg.setOpen(((DoorEntity*)entity)->getOpen());
    msg.setLocked(((DoorEntity*)entity)->getLocked());
    msg.serialise(&bs);
  }
  else
  {
    AddEntityMessage msg;
    msg.setName(entity->getName());
    msg.setId(entity->getId());
    msg.setType((char)entity->getType());
    msg.setMesh(entity->getMesh());
    msg.setPos(entity->getPos());
    msg.setSector(entity->getSector());
    msg.serialise(&bs);
  }

  connection->send(bs);
}

void User::sendRemoveEntity(Entity* entity)
{
  printf("send delentity '%s' to '%s'\n", *entity->getName(), *this->getName());
  if (!ent_list.exists(entity))
    return;

  ent_list.removeEntity(entity);

  RemoveEntityMessage msg;
  msg.setName(entity->getName());
  msg.setId(entity->getId());
  msg.setType((char)entity->getType());
  ByteStream bs;
  msg.serialise(&bs);
  if (connection) connection->send(bs);
  //delete entity;
}

void User::remove()
{
  if (own_entity)
  {
    PcEntity* tmp = own_entity;
    own_entity = 0;
    Server::getServer()->delEntity(tmp);
  }
  if (connection)
    connection = 0;

  Server::getServer()->getUserManager()->delUser(this);
}
