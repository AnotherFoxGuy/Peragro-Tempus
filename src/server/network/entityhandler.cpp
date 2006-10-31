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

#include "server/entity/entitymanager.h"
#include "server/entity/itemmanager.h"
#include "server/entity/entity.h"
#include "server/entity/itementity.h"
#include "server/entity/statmanager.h"
#include "server/entity/racemanager.h"
#include "server/entity/race.h"
#include "network.h"
#include "networkhelper.h"
#include "server/entity/usermanager.h"
#include "server/entity/charactermanager.h"

void EntityHandler::handleMoveRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  int name_id = user_ent->getId();

  MoveRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("Received MoveRequest from: '%s' w(%d) t(%d)\n", name, request_msg.getWalk(), request_msg.getRot());

  MoveMessage response_msg;
  response_msg.setWalk((float)(request_msg.getWalk()-1)*3);
  response_msg.setTurn((float)(request_msg.getTurn()-1));
  response_msg.setEntityId(name_id);
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
  const Entity* ent = NetworkHelper::getEntity(msg);
  if (!ent) return;

  Entity* user_ent = ent->getLock();

  int name_id = user_ent->getId();

  DrUpdateRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("DR of %s: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", name, request_msg.getSpeed(), request_msg.getPos()[0], request_msg.getPos()[1], request_msg.getPos()[2], request_msg.getRot());
  
  user_ent->setPos(request_msg.getPos());
  user_ent->setSector(request_msg.getSector());

  server->getCharacterManager()->checkForSave(user_ent->getPlayerEntity());

  DrUpdateMessage response_msg;
  response_msg.setRotation(request_msg.getRotation());
  response_msg.setPos(request_msg.getPos());
  response_msg.setSector(request_msg.getSector());
  response_msg.setEntityId(name_id);
  ByteStream bs;
  response_msg.serialise(&bs);

  NetworkHelper::broadcast(bs);

  user_ent->freeLock();
}

void EntityHandler::handlePickRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  ptString name = user_ent->getName();
  PickRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received PickRequest from: '%s' -> '%d' \n", *name, request_msg.getItemEntityId());

  const Entity* e = server->getEntityManager()->findById(request_msg.getItemEntityId());

  PickResponseMessage response_msg;
  if (!e)
  {
    response_msg.setError(ptString("Entity doesn't exist",20)); // <-- TODO: Error Message Storage
  }
  else if (e->getType() == Entity::ItemEntityType)
  {
    response_msg.setError(ptString(0,0));
  }
  else if (e->getType() == Entity::PlayerEntityType)
  {
    response_msg.setError(ptString("Don't pick on others!",21)); // <-- TODO: Error Message Storage
  }
  else
  {
    response_msg.setError(ptString("Unpickable",10)); // <-- TODO: Error Message Storage
  }

  if (response_msg.getError().isNull())
  {
    const ItemEntity* item_entity = e->getItemEntity();
    if (!item_entity) return;

    const Item* item = item_entity->getItem();

    if (!item) return; //send Error message?

    const Character* c_char = NetworkHelper::getCharacter(msg);
    Character* character = c_char->getLock();

    //request_msg.getInventoryId();
    unsigned char slot = request_msg.getSlot();

    if (character->getInventory()->getItemId(slot) == Item::NoItem)
    {
      if (character->getInventory()->addItem(item->getId(), slot))
      {
        bool equip = (slot < 10);
        if (equip && response_msg.getError() == ptString::Null)
        {
          EquipMessage equip_msg;
          equip_msg.setEntityId(user_ent->getId());
          if (item) equip_msg.setItemId(item->getId());
          equip_msg.setSlotId(slot);

          ByteStream bs;
          equip_msg.serialise(&bs);
          Server::getServer()->broadCast(bs);
        }

        response_msg.setItemId(item->getId());
        response_msg.setSlotId(slot);

        ByteStream bs;
        response_msg.serialise(&bs);
        NetworkHelper::sendMessage(user_ent, bs);

        server->delEntity(e);
        return;
      }
      response_msg.setError(ptString("Couldn't add item!",18)); // <-- TODO: Error Message Storage
    }
  }
  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::sendMessage(user_ent, bs);
}

void EntityHandler::handleDropRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  ptString name = user_ent->getName();
  DropRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received DropRequest from: '%s' -> '%d' \n", *name, request_msg.getItemId());

  DropResponseMessage response_msg;
  unsigned char slot_id = request_msg.getSlot();

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  unsigned int item = character->getInventory()->getItemId(slot_id);
  if (item != request_msg.getItemId())
  {
    response_msg.setError(ptString("Unexpected item", strlen("Unexpected item")));
    ByteStream bs;
    response_msg.serialise(&bs);
    NetworkHelper::sendMessage(user_ent, bs);
    return;
  }

  if (slot_id < 10)
  {
    // Tell the world to unequip it!
    EquipMessage unequip_msg;
    unequip_msg.setEntityId(user_ent->getId());
    unequip_msg.setSlotId(slot_id);
    unequip_msg.setItemId(item); // No Item!
    ByteStream bs;
    response_msg.serialise(&bs);
    NetworkHelper::broadcast(bs);
  }

  // Check if in Inventory
  bool couldTake = character->getInventory()->takeItem(slot_id);

  character->freeLock();

  if (!couldTake)
  {
    response_msg.setError(ptString("Failed to drop item", strlen("Failed to drop item")));
    ByteStream bs;
    response_msg.serialise(&bs);
    NetworkHelper::sendMessage(user_ent, bs);
    return;
  }

  response_msg.setItemId(item);
  response_msg.setSlotId(slot_id);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::sendMessage(user_ent, bs);

  bool equip = (slot_id < 10);
  if (equip && response_msg.getError() == ptString::Null)
  {
    EquipMessage response_msg;
    response_msg.setEntityId(user_ent->getId());
    if (item) response_msg.setItemId(0);
    response_msg.setSlotId(slot_id);

    ByteStream bs;
    response_msg.serialise(&bs);
    Server::getServer()->broadCast(bs);
  }

  // Create new entity from item.
  ItemEntity* e = new ItemEntity();
  e->createFromItem(item);

  Entity* ent = e->getEntity()->getLock();
  ent->setPos(user_ent->getPos());
  ent->setSector(user_ent->getSector());
  ent->freeLock();

  server->addEntity(ent, true);
}

void EntityHandler::handleMoveToRequest(GenericMessage* msg)
{
  const PcEntity* c_entity = NetworkHelper::getPcEntity(msg);
  if (!c_entity) return;

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  PcEntity* entity = c_entity->getLock();
  Character* character = c_char->getLock();

  MoveToRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Stat* speed = server->getStatManager()->findByName(ptString("Speed", 5));

  server->moveEntity(entity, request_msg.getTo(), (float)character->getStats()->getAmount(speed));
  server->getCharacterManager()->checkForSave(entity);
  entity->freeLock();
  character->freeLock();
}

void EntityHandler::handleInventoryMoveItemRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  const char* error = 0;

  ptString name = user_ent->getName();
  InventoryMoveItemRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  unsigned char equip_slot = request_msg.getNewSlot();
  unsigned char invent_slot = request_msg.getOldSlot();

  printf("Received EquipRequest from: '%s' | '%d' to '%d' \n", *name, invent_slot, equip_slot);

  bool equip = false;
  if (equip_slot < 10 || invent_slot < 10)
  {
    equip = true;
  }

  if (equip_slot >= 30 || invent_slot >= 30)
  {
    error = "Invalid slot";
  }

  Character* character = c_char->getLock();
  Inventory* inventory = character->getInventory();

  int new_item_id = inventory->getItemId(invent_slot);
  Item* item = server->getItemManager()->findById(new_item_id);

  if (!item) 
    error = "No such Item";

  // See if we have already an item in the equip slot.
  int old_item_id = inventory->getItemId(equip_slot);
  Item* old = server->getItemManager()->findById(old_item_id);

  if (item && ! error)
  {
    // Take from the inventory slot and...
    inventory->takeItem(invent_slot);

    // ... (if we have) from the equip slot too.
    if (old) inventory->takeItem(equip_slot);

    // Then we add the new item to the equip slot and...
    inventory->addItem(item->getId(), equip_slot);

    // ... (if we have) the old item to the inventory.
    if (old) inventory->addItem(old->getId(), invent_slot);
  }

  character->freeLock();

  InventoryMoveItemMessage response_msg;

  if (error)
  {
    response_msg.setError(ptString(error, strlen(error)));
  }

  response_msg.setOldSlot(invent_slot);
  response_msg.setNewSlot(equip_slot);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::sendMessage(user_ent, bs);

  if (equip && !error)
  {
    if (invent_slot < 10)
    {
      EquipMessage response_msg;
      response_msg.setEntityId(user_ent->getId());
      response_msg.setItemId(old_item_id);
      response_msg.setSlotId(invent_slot);

      ByteStream bs;
      response_msg.serialise(&bs);
      Server::getServer()->broadCast(bs);
    }

    if (equip_slot < 10)
    {
      EquipMessage response_msg;
      response_msg.setEntityId(user_ent->getId());
      response_msg.setItemId(new_item_id);
      response_msg.setSlotId(equip_slot);

      ByteStream bs;
      response_msg.serialise(&bs);
      Server::getServer()->broadCast(bs);
    }
  }
}

void EntityHandler::handleRelocate(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  int name_id = user_ent->getId();

  TeleportMessage telemsg;

  const Character* character = NetworkHelper::getCharacter(msg);
  if (!character) return;

  int race_id = character->getRace();
  Race* race = server->getRaceManager()->findById(race_id);

  if (!race) return;

  Entity* ent = user_ent->getLock();
  ent->setPos(race->getPos());
  ent->setSector(race->getSector());
  ent->freeLock();

  server->getCharacterManager()->checkForSave(user_ent->getPlayerEntity());

  telemsg.setEntityId(name_id);
  telemsg.setSector(race->getSector());
  telemsg.setPos(race->getPos());

  ByteStream bs;
  telemsg.serialise(&bs);
  server->broadCast(bs);
}

void EntityHandler::handleMountRequest(GenericMessage* msg)
{
}

void EntityHandler::handleUnmountRequest(GenericMessage* msg)
{
}
