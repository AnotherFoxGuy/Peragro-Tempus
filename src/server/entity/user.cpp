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

#include "character.h"
#include "doorentity.h"
#include "entity.h"
#include "pcentity.h"
#include "itementity.h"
#include "user.h"
#include "usermanager.h"

#include "common/network/entitymessages.h"

#include "server/server.h"

void User::setEntity(PcEntity* entity)
{
  own_entity = entity->getRef();
  entity->setUser(this);
}

void User::sendAddEntity(const Entity* entity)
{
  if (ent_list.exists(entity))
    return;

  if (!entity || !*(entity->getName()))
    return;

  printf("send addentity '%s' to '%s'\n", *entity->getName(), *this->getName());

  ent_list.addEntity(entity);
  ByteStream bs;
  if (entity->getType() == Entity::DoorEntityType)
  {
    AddDoorEntityMessage msg;
    msg.setName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMesh(entity->getMesh());
    msg.setIsOpen(entity->getDoorEntity()->getOpen());
    msg.setIsLocked(entity->getDoorEntity()->getLocked());
    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::ItemEntityType)
  {
    AddItemEntityMessage msg;
    msg.setEntityId(entity->getId());
    msg.setItemId(entity->getItemEntity()->getItem()->getId());
    msg.setPos(entity->getPos());
    msg.setSectorId(entity->getSector());
    msg.setSector(entity->getSectorName());
    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::PlayerEntityType)
  {
    AddPlayerEntityMessage msg;
    msg.setName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMesh(entity->getMesh());
    msg.setPos(entity->getPos());
    msg.setSectorId(entity->getSector());
    msg.setSector(entity->getSectorName());
    Character* character = entity->getPlayerEntity()->getCharacter()->getLock();
    msg.setDecalColour(character->getDecalColour());
    msg.setHairColour(character->getHairColour());
    msg.setSkinColour(character->getSkinColour());
    Inventory* inv = character->getInventory();
    msg.setEquipmentCount(10);
    for(int i=0; i<10; i++)
    {
      unsigned int item = inv->getItemId(i);
      msg.setItemId(i, item);
    }
    msg.serialise(&bs);
    character->freeLock();
  }
  else if (entity->getType() == Entity::NPCEntityType)
  {
    AddNpcEntityMessage msg;
    msg.setName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMesh(entity->getMesh());
    msg.setPos(entity->getPos());
    msg.setSectorId(entity->getSector());
    msg.setSector(entity->getSectorName());
    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::MountEntityType)
  {
    AddMountEntityMessage msg;
    msg.setName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMesh(entity->getMesh());
    msg.setPos(entity->getPos());
    msg.setSectorId(entity->getSector());
    msg.setSector(entity->getSectorName());
    msg.serialise(&bs);
  }
  else
  {
    printf("Unkown entity type!\n");
  }

  if (connection.get()) connection.get()->send(bs);
}

void User::sendRemoveEntity(const Entity* entity)
{
  printf("send delentity '%s' to '%s'\n", *entity->getName(), *this->getName());
  if (!ent_list.exists(entity))
    return;

  ent_list.removeEntity(entity);

  RemoveMessage msg;
  //msg.setName(entity->getName());
  msg.setEntityId(entity->getId());
  //msg.setType((char)entity->getType());
  ByteStream bs;
  msg.serialise(&bs);
  if (connection.get()) connection.get()->send(bs);
  //delete entity;
}

void User::remove()
{
  if (own_entity.get())
  {
    Server::getServer()->delEntity(own_entity.get()->getEntity());
  }

  Server::getServer()->getUserManager()->delUser(this);
}

