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
#include "server/entity/user.h"
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

  Server* server = Server::getServer();

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
    ptScopedMonitorable<Character> character (c_char);
    Stat* speed_stat = server->getStatManager()->findByName(ptString("Speed", 5));
    speed = (float)character->getStats()->getAmount(speed_stat);
    name_id = c_entity->getEntity()->getId();
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

  Server* server = Server::getServer();

  server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

  int name_id;
  if (ent->getPlayerEntity()->getMount())
  {
    float* pos = request_msg.getPos();

    // Make sure the character is up in the saddle
    // TODO: Maybe use a confirmation message from the client instead
    //const float* mountpos = ent->getPlayerEntity()->getMount()->getEntity()->getPos();
    //if (pos[1]<mountpos[1]+0.5f){ return; }

    pos[1] -= 1.0f; // Adjust the offset from the rider
    ptScopedMonitorable<Entity> user_ent (ent->getPlayerEntity()->getMount()->getEntity());
    user_ent->setPos(pos);
    user_ent->setRotation(request_msg.getRotation());
    user_ent->setSector(request_msg.getSectorId());

    name_id = ent->getPlayerEntity()->getMount()->getEntity()->getId();
  }
  else
  {
    name_id = ent->getId();
  }

  ptScopedMonitorable<Entity> user_ent (ent);
  user_ent->setPos(request_msg.getPos());
  user_ent->setRotation(request_msg.getRotation());
  user_ent->setSector(request_msg.getSectorId());

  DrUpdateMessage response_msg;
  response_msg.setRotation(request_msg.getRotation());
  response_msg.setPos(request_msg.getPos());
  response_msg.setSectorId(request_msg.getSectorId());
  response_msg.setEntityId(name_id);
  ByteStream bs;
  response_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
}

void EntityHandler::handlePickRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  ptString name = user_ent->getName();

  PickRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

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

      ptScopedMonitorable<Character> character (c_char);
      bool retval = character->getInventory()->addItem(entry, slot);

      if (retval)
      {
        bool equip = (slot < 10);
        if (equip && response_msg.getError() == ptString::Null)
        {
          EquipMessage equip_msg;
          equip_msg.setEntityId(user_ent->getId());
          if (item) equip_msg.setItemId(item->getId());
          equip_msg.setSlotId(slot);
          equip_msg.setFile(item->getFile());
          equip_msg.setMesh(item->getMesh());

          ByteStream bs;
          equip_msg.serialise(&bs);
          Server::getServer()->broadCast(bs);
        }

        response_msg.setItemId(item->getId());
        response_msg.setVariation(item_entity->variation);
        response_msg.setSlotId(slot);

        response_msg.setName(item->getName());
        response_msg.setIcon(item->getIcon());
        response_msg.setDescription(item->getDescription());
        response_msg.setWeight(item->getWeight());
        response_msg.setEquipType(item->getEquiptype());

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

  ptScopedMonitorable<Character> character (c_char);

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
    unequip_msg.setFile(ptString::Null);
    unequip_msg.setMesh(ptString::Null);
    ByteStream bs;
    unequip_msg.serialise(&bs);
    NetworkHelper::localcast(bs, user_ent);
  }

  // Create new entity from item.
  ItemEntity* e = new ItemEntity();
  e->createFromItem(item.id, item.variation);

  ptScopedMonitorable<Entity> ent (e->getEntity());
  ent->setPos(user_ent->getPos());
  ent->setRotation(user_ent->getRotation());
  ent->setSector(user_ent->getSector());

  Server::getServer()->addEntity(ent, true);
}

void EntityHandler::handleMoveToRequest(GenericMessage* msg)
{
  const PcEntity* c_entity = NetworkHelper::getPcEntity(msg);
  if (!c_entity) return;

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  ptScopedMonitorable<PcEntity> entity (c_entity);
  ptScopedMonitorable<Character> character (c_char);

  MoveToRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  Stat* speed_stat = server->getStatManager()->findByName(ptString("Speed", 5));

  if (entity->getMount())
  {
    ptScopedMonitorable<MountEntity> mount (entity->getMount());
    float speed = mount->getSpeed();
    // TODO
    // moveEntity must be called without holding the lock.
    // This is ugly since 'speed' could potentinally be
    // changed since there is no lock.
    server->moveEntity(mount, request_msg.getTo(), speed, request_msg.getRun());
  }
  else if (entity->usesFlashStep())
  {
    ptScopedMonitorable<Entity> ent (entity->getEntity());
    ent->setPos(request_msg.getTo());

    TeleportResponseMessage telemsg;
    telemsg.setEntityId(ent->getId());
    telemsg.setSectorId(ent->getSector());
    telemsg.setPos(ent->getPos());
    telemsg.setRotation(ent->getRotation());

    ByteStream bs;
    telemsg.serialise(&bs);
    server->broadCast(bs);
  }
  else
  {
    float speed = (float)character->getStats()->getAmount(speed_stat);
    server->moveEntity(entity, request_msg.getTo(), speed, request_msg.getRun());
  }

  server->getCharacterManager()->checkForSave(entity);
}

void EntityHandler::handleRelocate(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  TeleportResponseMessage telemsg;

  Server* server = Server::getServer();

  const Character* character = NetworkHelper::getCharacter(msg);
  if (!character) return;

  int race_id = character->getRace();
  Race* race = server->getRaceManager()->findById(race_id);

  if (!race) return;

  const Entity* c_ent = 0;
  if (user_ent->getPlayerEntity()->getMount())
  {
    c_ent = user_ent->getPlayerEntity()->getMount()->getEntity();
  }
  else
  {
    c_ent = user_ent;
  }
  ptScopedMonitorable<Entity> ent (c_ent);
  int ent_id = ent->getId();
  ent->setSector(race->getSector());
  ent->setPos(race->getPos());

  server->getCharacterManager()->checkForSave(user_ent->getPlayerEntity());

  telemsg.setEntityId(ent_id);
  unsigned short sector_id = server->getSectorManager()->getSectorId(race->getSector());
  telemsg.setSectorId(sector_id);
  telemsg.setPos(race->getPos());
  telemsg.setRotation(ent->getRotation());

  ByteStream bs;
  telemsg.serialise(&bs);
  server->broadCast(bs);
}

void EntityHandler::handleTeleportRequest(GenericMessage* msg)
{
  const User* user = NetworkHelper::getUser(msg);
  if (!user) return;
  
  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  TeleportRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  const Entity* target_ent = server->getEntityManager()->findById(request_msg.getEntityId());
  if (!target_ent) return;

  const Entity* c_ent = 0;
  if (target_ent->getPlayerEntity()->getMount())
  {
    c_ent = target_ent->getPlayerEntity()->getMount()->getEntity();
  }
  else
  {
    c_ent = target_ent;
  }
  ptScopedMonitorable<Entity> ent (c_ent);
  ent->setSector(request_msg.getSectorId()); 
  ent->setPos(request_msg.getPos());
  ent->setRotation(request_msg.getRotation());

  server->getCharacterManager()->checkForSave(target_ent->getPlayerEntity());

  TeleportResponseMessage response_msg;
  response_msg.setEntityId(ent->getId());
  response_msg.setSectorId(ent->getSector());
  response_msg.setPos(ent->getPos());
  response_msg.setRotation(ent->getRotation());

  ByteStream bs;
  response_msg.serialise(&bs);
  server->broadCast(bs);
}

void EntityHandler::handleMountRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  MountRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

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

  ptScopedMonitorable<PcEntity> e (pc_ent);
  ptScopedMonitorable<MountEntity> mount (c_mount);

  e->setMount(mount);
  mount->addPassenger(e);

  // Don't run into the horse!
  MoveMessage mmsg;
  mmsg.setEntityId(user_ent->getId());
  mmsg.setWalk(0);
  mmsg.setTurn(0);
  mmsg.setJump(false);
  mmsg.setRun(false);

  ByteStream bs1;
  mmsg.serialise(&bs1);

  NetworkHelper::localcast(bs1, user_ent);

  MountMessage umount_msg;
  umount_msg.setMountEntityId(mount_ent->getId());
  umount_msg.setPlayerEntityId(user_ent->getId());
  umount_msg.setCanControl(true);

  ByteStream bs;
  umount_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
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
  ptScopedMonitorable<MountEntity> mount (c_mount);
  mount->delPassenger(pc_ent);

  printf("Reset the player's mount status!\n");
  ptScopedMonitorable<PcEntity> e (pc_ent);
  e->setMount(0);

  printf("Unmount finished\n");

  UnmountMessage umount_msg;
  umount_msg.setMountEntityId(mount_ent->getId());
  umount_msg.setPlayerEntityId(user_ent->getId());

  ByteStream bs;
  umount_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
}

void EntityHandler::handlePoseRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  PoseRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  const PcEntity* pc_ent = user_ent->getPlayerEntity();
  if (!pc_ent) return;

  unsigned char pose_id = request_msg.getPoseId();

  ptScopedMonitorable<PcEntity> e (pc_ent);
  e->setPose(pose_id);

  PoseMessage pose_msg;
  pose_msg.setEntityId(user_ent->getId());
  pose_msg.setPoseId(e->getPose());

  ByteStream bs;
  pose_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
}
