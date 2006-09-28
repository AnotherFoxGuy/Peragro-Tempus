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

#include "common/entity/itemmanager.h"
#include "common/entity/statmanager.h"
#include "common/entity/racemanager.h"
#include "common/entity/race.h"
#include "server/network/network.h"
#include "server/usermanager.h"
#include "server/charactermanager.h"

void EntityHandler::handleMoveRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  int name_id = conn->getUser()->getEntity()->getId();
  MoveEntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("Received MoveRequest from: '%s' w(%d) t(%d)\n", name, request_msg.getWalk(), request_msg.getRot());

  MoveEntityMessage response_msg;
  response_msg.setWalk((float)request_msg.getWalk()*3);
  response_msg.setRot((float)request_msg.getRot());
  response_msg.setId(name_id);
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
  int name_id = user_ent->getId();

  UpdateDREntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("DR of %s: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", name, request_msg.getSpeed(), request_msg.getPos()[0], request_msg.getPos()[1], request_msg.getPos()[2], request_msg.getRot());
  
  user_ent->setPos(request_msg.getPos());
  user_ent->setSector(request_msg.getSector());

  server->getCharacterManager()->checkForSave((PcEntity*)user_ent);

  UpdateDREntityMessage response_msg;
  response_msg.setSpeed(request_msg.getSpeed());
  response_msg.setRot(request_msg.getRot());
  response_msg.setAVel(request_msg.getAVel());
  response_msg.setVel(request_msg.getVel());
  response_msg.setWVel(request_msg.getWVel());
  response_msg.setPos(request_msg.getPos());
  response_msg.setOnGround(request_msg.getOnGround());
  response_msg.setSector(request_msg.getSector());
  response_msg.setId(name_id);
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
  CharacterEntity* user_ent = conn->getUser()->getEntity();
  ptString name = user_ent->getName();
  PickEntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received PickRequest from: '%s' -> '%d' \n", *name, request_msg.getTargetId());

  Entity* e = server->getEntityManager()->findById(request_msg.getTargetId());

  PickEntityResponseMessage response_msg;
  response_msg.setName(name);
  response_msg.setItemId(0);
  if (!e)
  {
    response_msg.setTarget(ptString(0,0));
    response_msg.setError(ptString("Entity doesn't exist",20)); // <-- TODO: Error Message Storage
  }
  else if (e->getType() == Entity::ItemEntity)
  {
    response_msg.setTarget(e->getName());
    response_msg.setError(ptString(0,0));
  }
  else if (e->getType() == Entity::PlayerEntity)
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
    Item* item = server->getItemManager()->findById(((ItemEntity*)e)->getItem());

    if (!item) return; //send Error message?

    response_msg.setItemId(item->getId());

    ByteStream bs;
    response_msg.serialise(&bs);
    for (size_t i=0; i<server->getUserManager()->getUserCount(); i++)
    {
      User* user = server->getUserManager()->getUser(i);
      if (user && user->getConnection())
        user->getConnection()->send(bs);
    }

    user_ent->getInventory()->addItem(item,1, -1);

    server->delEntity(e);
  }
}

void EntityHandler::handleDropRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;
  CharacterEntity* user_ent = conn->getUser()->getEntity();
  ptString name = user_ent->getName();
  DropEntityRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received DropRequest from: '%s' -> '%d' \n", *name, request_msg.getTargetId());

  Item* item = server->getItemManager()->findById(request_msg.getTargetId());

  if (!item)
  {
    //send Error message?
    return;
  }

  unsigned int slot_id = request_msg.getSlotId();

  if (slot_id < 10)
  {
    //item is equiped
    //send Error message?
    return;
  }
  else if (slot_id >= 30)
  {
    // invalid slot
    return;
  }

  // Check if in Inventory
  bool canTake = user_ent->getInventory()->takeItem(item,1, slot_id);

  if (!canTake)
  {
    //send Error message?
    return;
  }

  // Create new entity from item.
  ItemEntity* e = new ItemEntity();
  e->createFromItem(item);
  e->setPos(user_ent->getPos());
  e->setSector(user_ent->getSector());

  server->addEntity(e, true);
}

void EntityHandler::handleMoveEntityToRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;

  PcEntity* entity = (PcEntity*) conn->getUser()->getEntity();

  MoveEntityToRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Stat* speed = server->getStatManager()->findByName(ptString("Speed", 5));

  server->moveEntity(entity, request_msg.getPos(), (float)entity->getStats()->getAmount(speed));
  server->getCharacterManager()->checkForSave(entity);
}

void EntityHandler::handleEquipRequest(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;

  const char* error = 0;

  CharacterEntity* user_ent = conn->getUser()->getEntity();
  ptString name = user_ent->getName();
  EquipRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  int equip_slot = request_msg.getEquipSlotID();
  int invent_slot = request_msg.getInventorySlotID();

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

  int new_item_id = user_ent->getInventory()->getItemIdFromSlot(equip_slot);
  Item* item = server->getItemManager()->findById(new_item_id);

  if (!item->isEquipable()) 
  {
    // doesn't matter if we overwrite the error
    error = "Item not equipable";
  }

  if (item && ! error)
  {
    unsigned int amount = user_ent->getInventory()->getAmount(item, invent_slot);
    if (amount == 1)
    {
      // See if we have already an item in the equip slot.
      int old_item_id = user_ent->getInventory()->getItemIdFromSlot(equip_slot);
      Item* old = server->getItemManager()->findById(old_item_id);

      // Take from the inventory slot and...
      user_ent->getInventory()->takeItem(item, 1, invent_slot);

      // ... (if we have) from the equip slot too.
      if (old) user_ent->getInventory()->takeItem(old, 1, equip_slot);

      // Then we add the new item to the equip slot and...
      user_ent->getInventory()->addItem(item, 1, equip_slot);

      // ... (if we have) the old item to the inventory.
      if (old) user_ent->getInventory()->addItem(old, 1, invent_slot);
    }
    else if (amount == 0) error = "Character doesn't own this item";
    else if (amount > 1) error = "Can't equip stacked items";
  }
  else error = "No such Item";

  EquipMessage response_msg;
  response_msg.setEntityID(user_ent->getId());
  response_msg.setItemID(item->getId());
  response_msg.setSlotID(equip_slot);
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
    conn->send(bs);
  }
}

void EntityHandler::handleOpenDoor(GenericMessage* msg)
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

void EntityHandler::handleCloseDoor(GenericMessage* msg)
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

void EntityHandler::handleRelocate(GenericMessage* msg)
{
  const Connection* conn = msg->getConnection();
  if (!conn) return;

  PcEntity* user_ent = conn->getUser()->getEntity();
  int name_id = user_ent->getId();

  TeleportMessage telemsg;

  int race_id = user_ent->getCharacter()->getRace();
  Race* race = server->getRaceManager()->findById(race_id);

  if (!race) return;

  user_ent->setPos(race->getPos());
  user_ent->setSector(race->getSector());

  server->getCharacterManager()->checkForSave((PcEntity*)user_ent);

  telemsg.setId(name_id);
  telemsg.setSector(race->getSector());
  telemsg.setPos(race->getPos());

  ByteStream bs;
  telemsg.serialise(&bs);
  server->broadCast(bs);
}
