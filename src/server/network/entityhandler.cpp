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
  printf("Received PickRequest from: '%s' -> '%d' \n", *name, request_msg.getTarget());

  const Entity* e = server->getEntityManager()->findById(request_msg.getTarget());

  PickResponseMessage response_msg;
  response_msg.setName(name);
  response_msg.setItemId(0);
  if (!e)
  {
    response_msg.setTarget(ptString(0,0));
    response_msg.setError(ptString("Entity doesn't exist",20)); // <-- TODO: Error Message Storage
  }
  else if (e->getType() == Entity::ItemEntityType)
  {
    response_msg.setTarget(e->getName());
    response_msg.setError(ptString(0,0));
  }
  else if (e->getType() == Entity::PlayerEntityType)
  {
    response_msg.setTarget(e->getName());
    response_msg.setError(ptString("Don't pick on others!",21)); // <-- TODO: Error Message Storage
  }
  else
  {
    response_msg.setTarget(e->getName());
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

    unsigned char slot = character->getInventory()->getFreeSlot();

    if (slot == Inventory::NoSlot)
    {
      response_msg.setTarget(e->getName());
      response_msg.setError(ptString("No free slot",12)); // <-- TODO: Error Message Storage
    }
    else
    {
      if (character->getInventory()->addItem(item->getId(), slot))
      {
        response_msg.setSlot(slot);
        response_msg.setItemId(item->getId());

        ByteStream bs;
        response_msg.serialise(&bs);
        NetworkHelper::broadcast(bs);

        server->delEntity(e);
        return;
      }
      response_msg.setTarget(e->getName());
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
  printf("Received DropRequest from: '%s' -> '%d' \n", *name, request_msg.getTarget());

  Item* item = server->getItemManager()->findById(request_msg.getTarget());

  DropResponseMessage response_msg;

  if (!item)
  {
    response_msg.setError(ptString("You don't own this Item", strlen("You don't own this Item")));
    return;
  }

  unsigned int slot_id = request_msg.getSlotId();

  // To allow direct drops or not!
  if (slot_id < 10)
  {
    response_msg.setError(ptString("You can't drop an equiped item", strlen("You can't drop an equiped item")));
    return;
  }

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  // Check if in Inventory
  bool couldTake = character->getInventory()->takeItem(slot_id);

  character->freeLock();

  if (!couldTake)
  {
    response_msg.setError(ptString("Failed to drop item", strlen("Failed to drop item")));
    return;
  }

  // Create new entity from item.
  ItemEntity* e = new ItemEntity();
  e->createFromItem(item);

  Entity* ent = e->getEntity()->getLock();
  ent->setPos(user_ent->getPos());
  ent->setSector(user_ent->getSector());
  ent->freeLock();

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::sendMessage(user_ent, bs);

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

void EntityHandler::handleEquipRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  const char* error = 0;

  ptString name = user_ent->getName();
  EquipRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  int equip_slot = request_msg.getNewSlotId();
  int invent_slot = request_msg.getOldSlotId();

  printf("Received EquipRequest from: '%s' | '%d' to '%d' \n", *name, invent_slot, equip_slot);

  bool equip = false;
  if (equip_slot < 10 && equip_slot >= 0)
  {
    equip = true;
  }
  else if (equip_slot < 30)
  {
    // Slot move, no equip
    equip = false;
  }
  else error = "Invalid equip slot";

  // doesn't matter if we overwrite the error
  if (invent_slot >= 30) 
    error = "Invalid inventory slot";

  Character* character = c_char->getLock();
  Inventory* inventory = character->getInventory();

  int new_item_id = inventory->getItemId(invent_slot);
  Item* item = server->getItemManager()->findById(new_item_id);

  if (!item) 
    error = "No such Item";

  if (item && ! error)
  {
    unsigned int amount_old = inventory->getAmount(invent_slot);
    if (amount_old == 0) error = "Character doesn't own this item";
    else if (equip) // equip
    {
      // See if we have already an item in the equip slot.
      int old_item_id = inventory->getItemId(equip_slot);
      Item* old = server->getItemManager()->findById(old_item_id);

      // Take from the inventory slot and...
      inventory->takeItem(invent_slot);

      // ... (if we have) from the equip slot too.
      if (old) inventory->takeItem(equip_slot);

      // Then we add the new item to the equip slot and...
      inventory->addItem(item->getId(), equip_slot);

      // ... (if we have) the old item to the inventory.
      if (old) inventory->addItem(old->getId(), invent_slot);
    }
    else // move
    {

      // See if we have already an item in the equip slot.
      unsigned int amount_new = inventory->getAmount(equip_slot);
      int old_item_id = inventory->getItemId(equip_slot);
      Item* old = server->getItemManager()->findById(old_item_id);

      // Take from the inventory slot and...
      inventory->takeItem(invent_slot);

      // ... (if we have) from the equip slot too.
      if (old) inventory->takeItem(equip_slot);

      // Then we add the new item to the equip slot and...
      inventory->addItem(item->getId(), equip_slot);

      // ... (if we have) the old item to the inventory.
      if (old) inventory->addItem(old->getId(), invent_slot);
    }
  }

  character->freeLock();

  if (error) printf("Equip item error occured: %s\n", error);

  EquipMessage response_msg;
  response_msg.setEntityId(user_ent->getId());
  if (item) response_msg.setItemId(item->getId());
  response_msg.setOldSlotId(invent_slot);
  response_msg.setNewSlotId(equip_slot);
  ptString pt_err = error?ptString(error, strlen(error)):ptString();
  response_msg.setError(pt_err);

  ByteStream bs;
  response_msg.serialise(&bs);

  if (!error && equip)
  {
    // Tell all about success
    Server::getServer()->broadCast(bs);
  }
  else
  {
    // Tell only one about error or equip
    NetworkHelper::sendMessage(user_ent, bs);
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
