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
#include "server/entity/mountentity.h"
#include "server/entity/sectormanager.h"
#include "server/entity/statmanager.h"
#include "server/entity/racemanager.h"
#include "server/entity/race.h"
#include "network.h"
#include "networkhelper.h"
#include "server/entity/usermanager.h"
#include "server/entity/charactermanager.h"

void EntityHandler::handleMoveRequest(GenericMessage* msg)
{
  const PcEntity* c_entity = NetworkHelper::getPcEntity(msg);
  if (!c_entity) return;

  const Character* c_char = c_entity->getCharacter();
  if (!c_char) return;

  int name_id;

  MoveRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  //printf("Received MoveRequest from: '%s' w(%d) t(%d)\n", name, request_msg.getWalk(), request_msg.getRot());

  float speed = 0;
  float acc = 1;

  if (c_entity->getMount())
  {
    const MountEntity* mount = c_entity->getMount();
    speed = mount->getSpeed();
    name_id = mount->getEntity()->getId();
  }
  else
  {
    Character* character = c_char->getLock();
    Stat* speed_stat = server->getStatManager()->findByName(ptString("Speed", 5));
    speed = (float)character->getStats()->getAmount(speed_stat);
    name_id = c_entity->getEntity()->getId();
    character->freeLock();
  }

  MoveMessage response_msg;
  if (request_msg.getRun())
  {
    // TODO this should be based on character and need to check endurance
    acc = 2;
    response_msg.setRun(true);
  }
  else
  {
    response_msg.setRun(false);
  }

  response_msg.setWalk((float)(request_msg.getWalk()-1)*speed*acc);
  response_msg.setTurn((float)(request_msg.getTurn()-1));
  response_msg.setJump(request_msg.getJump());
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

  DrUpdateRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

  int name_id;
  if (ent->getPlayerEntity()->getMount())
  {
    float* pos = request_msg.getPos();
    pos[1] -= 1.0f; // Adjust the offset from the rider
    Entity* user_ent = ent->getPlayerEntity()->getMount()->getEntity()->getLock();
    user_ent->setPos(pos);
    user_ent->setSector(request_msg.getSectorId());
    user_ent->freeLock();

    name_id = ent->getPlayerEntity()->getMount()->getEntity()->getId();
  }
  else
  {
    name_id = ent->getId();
  }

  Entity* user_ent = ent->getLock();
  user_ent->setPos(request_msg.getPos());
  user_ent->setSector(request_msg.getSectorId());
  user_ent->freeLock();

  DrUpdateMessage response_msg;
  response_msg.setRotation(request_msg.getRotation());
  response_msg.setPos(request_msg.getPos());
  response_msg.setSectorId(request_msg.getSectorId());
  response_msg.setEntityId(name_id);
  ByteStream bs;
  response_msg.serialise(&bs);

  NetworkHelper::broadcast(bs);
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

    //request_msg.getInventoryId();
    unsigned char slot = request_msg.getSlot();

    if (c_char->getInventory()->getItem(slot)->id == Item::NoItem)
    {
      const InventoryEntry entry(item->getId(), item_entity->variation);

      Character* character = c_char->getLock();
      bool retval = character->getInventory()->addItem(entry, slot);
      character->freeLock();

      if (retval)
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
  printf("Received DropRequest from: '%s' -> '%d' \n", *name, request_msg.getSlot());

  DropResponseMessage response_msg;
  unsigned char slot_id = request_msg.getSlot();

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  Character* character = c_char->getLock();

  const InventoryEntry item = *character->getInventory()->getItem(slot_id);
  if (item.id == Item::NoItem)
  {
    response_msg.setError(ptString("Unexpected item", strlen("Unexpected item")));
    ByteStream bs;
    response_msg.serialise(&bs);
    NetworkHelper::sendMessage(user_ent, bs);
    return;
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

  response_msg.setSlotId(slot_id);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::sendMessage(user_ent, bs);

  if (slot_id < 10)
  {
    printf("Dropped an equiped item, so unequip it!\n");
    // Tell the world to unequip it!
    EquipMessage unequip_msg;
    unequip_msg.setEntityId(user_ent->getId());
    unequip_msg.setSlotId(slot_id);
    unequip_msg.setItemId(Item::NoItem); // No Item!
    ByteStream bs;
    response_msg.serialise(&bs);
    NetworkHelper::broadcast(bs);
  }

  // Create new entity from item.
  ItemEntity* e = new ItemEntity();
  e->createFromItem(item.id, item.variation);

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

  Stat* speed_stat = server->getStatManager()->findByName(ptString("Speed", 5));

  if (entity->getMount())
  {
    MountEntity* mount = entity->getMount()->getLock();
    float speed = mount->getSpeed();
    // TODO
    // moveEntity must be called without holding the lock.
    // This is ugly since 'speed' could potentinally be
    // changed since there is no lock.
    mount->freeLock();
    server->moveEntity(mount, request_msg.getTo(), speed, request_msg.getRun());
  }
  else
  {
    float speed = (float)character->getStats()->getAmount(speed_stat);
    server->moveEntity(entity, request_msg.getTo(), speed, request_msg.getRun());
  }

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

  const InventoryEntry new_item = *inventory->getItem(invent_slot);
  Item* item = server->getItemManager()->findById(new_item.id);

  if (!item)
    error = "No such Item";

  // See if we have already an item in the equip slot.
  const InventoryEntry old_item = *inventory->getItem(equip_slot);
  Item* old = server->getItemManager()->findById(old_item.id);

  if (item && ! error)
  {
    // Take from the inventory slot and...
    inventory->takeItem(invent_slot);

    // ... (if we have) from the equip slot too.
    if (old) inventory->takeItem(equip_slot);

    // Then we add the new item to the equip slot and...
    inventory->addItem(new_item, equip_slot);

    // ... (if we have) the old item to the inventory.
    if (old) inventory->addItem(old_item, invent_slot);
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
      response_msg.setItemId(old_item.id);
      response_msg.setSlotId(invent_slot);

      ByteStream bs;
      response_msg.serialise(&bs);
      Server::getServer()->broadCast(bs);
    }

    if (equip_slot < 10)
    {
      EquipMessage response_msg;
      response_msg.setEntityId(user_ent->getId());
      response_msg.setItemId(new_item.id);
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

  TeleportMessage telemsg;

  const Character* character = NetworkHelper::getCharacter(msg);
  if (!character) return;

  int race_id = character->getRace();
  Race* race = server->getRaceManager()->findById(race_id);

  if (!race) return;

  Entity* ent = 0;
  if (user_ent->getPlayerEntity()->getMount())
  {
    ent = user_ent->getPlayerEntity()->getMount()->getEntity()->getLock();
  }
  else
  {
    ent = user_ent->getLock();
  }
  int ent_id = ent->getId();
  ent->setPos(race->getPos());
  ent->setSector(race->getSector());
  ent->freeLock();

  server->getCharacterManager()->checkForSave(user_ent->getPlayerEntity());

  telemsg.setEntityId(ent_id);
  unsigned short sector_id = server->getSectorManager()->getSectorId(race->getSector());
  telemsg.setSectorId(sector_id);
  telemsg.setPos(race->getPos());

  ByteStream bs;
  telemsg.serialise(&bs);
  server->broadCast(bs);
}

void EntityHandler::handleMountRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  MountRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  unsigned int mount_id = request_msg.getMountEntityId();

  const PcEntity* pc_ent = user_ent->getPlayerEntity();
  if (!pc_ent) return;

  const MountEntity* c_mount = pc_ent->getMount();
  if (c_mount) return;

  const Entity* mount_ent = server->getEntityManager()->findById(mount_id);
  if (!mount_ent) return;

  c_mount = mount_ent->getMountEntity();
  if (!c_mount) return;

  if (c_mount->getPassengerCount() >= c_mount->getMaxPassengers())
    return;

  PcEntity* e = pc_ent->getLock();
  MountEntity* mount = c_mount->getLock();

  e->setMount(mount);
  mount->addPassenger(e);

  e->freeLock();
  mount->freeLock();

  // Do something!

  MountMessage umount_msg;
  umount_msg.setMountEntityId(mount_ent->getId());
  umount_msg.setPlayerEntityId(user_ent->getId());
  umount_msg.setCanControl(true);

  ByteStream bs;
  umount_msg.serialise(&bs);

  NetworkHelper::broadcast(bs);
}

void EntityHandler::handleUnmountRequest(GenericMessage* msg)
{
  printf("Start unmount character\n");

  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  UnmountRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  const PcEntity* pc_ent = user_ent->getPlayerEntity();
  if (!pc_ent) return;

  unsigned int mount_id = request_msg.getMountEntityId();

  const MountEntity* c_mount = pc_ent->getMount();
  if (!c_mount) return;

  const Entity* mount_ent = c_mount->getEntity();
  if (!mount_ent) return;

  if (mount_ent->getId() != mount_id) return;

  printf("Remove player from mount!\n");
  MountEntity* mount = c_mount->getLock();
  mount->delPassenger(pc_ent);
  mount->freeLock();

  printf("Reset the player's mount status!\n");
  PcEntity* e = pc_ent->getLock();
  e->setMount(0);
  e->freeLock();

  printf("Unmount finished\n");

  UnmountMessage umount_msg;
  umount_msg.setMountEntityId(mount_ent->getId());
  umount_msg.setPlayerEntityId(user_ent->getId());

  ByteStream bs;
  umount_msg.serialise(&bs);

  NetworkHelper::broadcast(bs);
}

void EntityHandler::handlePoseRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  PoseRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  const PcEntity* pc_ent = user_ent->getPlayerEntity();
  if (!pc_ent) return;

  unsigned int pose_id = request_msg.getPoseId();

  PcEntity* e = pc_ent->getLock();
  e->setPose(pose_id);
  e->freeLock();

  PoseMessage pose_msg;
  pose_msg.setEntityId(user_ent->getId());
  pose_msg.setPoseId(e->getPose());

  ByteStream bs;
  pose_msg.serialise(&bs);

  NetworkHelper::broadcast(bs);
}
