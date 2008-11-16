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
#include "npcentity.h"
#include "itementity.h"
#include "itemmanager.h"
#include "user.h"
#include "usermanager.h"
#include "sectormanager.h"

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

  if (!getEntity())
    return;

  SectorManager* sectormanager = Server::getServer()->getSectorManager();
  const ptString& entity_region =
    sectormanager->getRegionName( entity->getSector() );
  const ptString& player_region =
    sectormanager->getRegionName(getEntity()->getEntity()->getSector());

  if (!(player_region == entity_region)) return;

  printf("send addentity '%s' to '%s'\n", *entity->getName(), *this->getName());

  ent_list.addEntity(entity);
  ByteStream bs;
  if (entity->getType() == Entity::DoorEntityType)
  {
    AddDoorEntityMessage msg;
    msg.setDoorId(entity->getDoorEntity()->getDoorId());
    msg.setEntityId(entity->getId());
    msg.setIsOpen(entity->getDoorEntity()->getOpen());
    msg.setIsLocked(entity->getDoorEntity()->getLocked());
    msg.setAnimationName(entity->getDoorEntity()->getAnimation());
    msg.setMeshId(entity->getMesh()->getId()); // Not used yet!
    msg.setMeshName(entity->getMesh()->getName());
    msg.setFileName(entity->getMesh()->getFile());
    msg.setSectorId(entity->getSector());
    msg.setEntityName(entity->getName());
    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::ItemEntityType)
  {
    const Item* item = entity->getItemEntity()->getItem();

    AddItemEntityMessage msg;
    msg.setEntityId(entity->getId());
    msg.setItemId(item->getId());
    msg.setPos(entity->getPos());
    msg.setRotation(entity->getRotation());
    msg.setSectorId(entity->getSector());

    msg.setEntityName(item->getName());
    msg.setMeshId(item->getMesh()->getId()); // Not used yet!
    msg.setMeshName(item->getMesh()->getName());
    msg.setFileName(item->getMesh()->getFile());

    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::PlayerEntityType)
  {
    AddPlayerEntityMessage msg;
    msg.setEntityName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMeshId(entity->getMesh()->getId()); // Not used yet!
    msg.setMeshName(entity->getMesh()->getName());
    msg.setFileName(entity->getMesh()->getFile());
    msg.setPos(entity->getPos());
    msg.setPos(entity->getPos());
    msg.setRotation(entity->getRotation());
    msg.setSectorId(entity->getSector());
    msg.setPoseId(entity->getPlayerEntity()->getPose());

    ptScopedMonitorable<Character> character (entity->getPlayerEntity()->getCharacter());

    msg.setDecalColour(character->getDecalColour());
    msg.setHairColour(character->getHairColour());
    msg.setSkinColour(character->getSkinColour());
    Inventory* inv = character->getInventory();
    inv->addEquipment<AddPlayerEntityMessage>(msg);
    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::NPCEntityType)
  {
    AddNpcEntityMessage msg;
    msg.setEntityName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMeshId(entity->getMesh()->getId()); // Not used yet!
    msg.setMeshName(entity->getMesh()->getName());
    msg.setFileName(entity->getMesh()->getFile());
    msg.setPos(entity->getPos());
    msg.setRotation(entity->getRotation());
    msg.setSectorId(entity->getSector());
    ptScopedMonitorable<Character> character (entity->getNpcEntity()->getCharacter());
    Inventory* inv = character->getInventory();
    inv->addEquipment(msg);
    msg.serialise(&bs);
  }
  else if (entity->getType() == Entity::MountEntityType)
  {
    AddMountEntityMessage msg;
    msg.setEntityName(entity->getName());
    msg.setEntityId(entity->getId());
    msg.setMeshId(entity->getMesh()->getId()); // Not used yet!
    msg.setMeshName(entity->getMesh()->getName());
    msg.setFileName(entity->getMesh()->getFile());
    msg.setPos(entity->getPos());
    msg.setPos(entity->getPos());
    msg.setRotation(entity->getRotation());
    msg.setSectorId(entity->getSector());
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
  if (!ent_list.exists(entity))
    return;

  printf("send delentity '%s' to '%s'\n", *entity->getName(), *this->getName());

  ent_list.removeEntity(entity);

  RemoveEntityMessage msg;
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
    printf("own_entity.get()!\n");
    Server::getServer()->delEntity(own_entity.get()->getEntity());
  }

  Server::getServer()->getUserManager()->delUser(this);
}

