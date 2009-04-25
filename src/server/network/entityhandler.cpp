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
#include "server/entity/entity.h"
#include "server/entity/character/movementmanager.h"
#include "server/entity/character/character.h"
#include "server/entity/pcentity.h"
#include "server/entity/itementity.h"
#include "server/entity/mountentity.h"
#include "server/entity/user.h"
#include "network.h"
#include "networkhelper.h"
#include "server/entity/usermanager.h"

void EntityHandler::handleMoveRequest(GenericMessage* msg)
{
  MoveRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  
  boost::shared_ptr<PcEntity> entity = NetworkHelper::GetEntity(msg);
  if (!entity) return; 

  Server* server = Server::getServer();

  // Some speed hacking prevention.
  WFMath::Point<3> direction = request_msg.getMoveDirection();
  if (direction.x() > 2)
  {
    direction[0] = 2;
    request_msg.setMoveDirection(direction);
  }
  if (direction.z() > 2)
  {
    direction[2] = 2;
    request_msg.setMoveDirection(direction);
  }

  boost::shared_ptr<Character> mover;
  if (entity->GetMount())
  {
    mover = entity->GetMount();
  }
  else
  {
    mover = entity;
  }

  MoveMessage response_msg;

  if ((int)request_msg.getMoveDirection().x()-1)
  {
    if (request_msg.getRun())
    {
      response_msg.setRun(true);
      mover->SetState(Character::StateRunning);
    }
    else
    {
      response_msg.setRun(false);
      mover->SetState(Character::StateWalking);
    }
  }
  else
  {
    response_msg.setRun(false);
    mover->SetState(Character::StateStanding);
  }

  float speed = server->GetMovementManager()->GetMovementSpeed(mover);

  float move_x = (float)(request_msg.getMoveDirection().x()-1)*speed;
  float move_z = (float)(request_msg.getMoveDirection().z()-1);
  response_msg.setMoveDirection(move_x, 0, move_z);

  response_msg.setJump(request_msg.getJump());
  response_msg.setEntityId(mover->GetId());
  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::localcast(bs, mover);

  server->GetMovementManager()->Register(mover);
} // handleMoveRequest

void EntityHandler::handleDrUpdateRequest(GenericMessage* msg)
{
  /*
  const Entity* ent = NetworkHelper::getEntity(msg);
  if (!ent) return;

  DrUpdateRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  server->getCharacterManager()->checkForSave(ent->getPlayerEntity());

  int name_id;
  if (ent->getPlayerEntity()->getMount())
  {
    WFMath::Point<3> pos = request_msg.GetPosition();

    // Make sure the character is up in the saddle
    // TODO: Maybe use a confirmation message from the client instead
    //WFMath::Point<3> mountpos = ent->getPlayerEntity()->getMount()->getEntity()->GetPosition();
    //if (pos.y < mountpos.y + 0.5f){ return; }

    pos[1] -= 1.0f; // Adjust the offset from the rider
    ptScopedMonitorable<Entity> user_ent (ent->getPlayerEntity()->getMount()->getEntity());
    user_ent->SetPosition(pos);
    user_ent->SetRotation(request_msg.GetRotation());
    user_ent->SetSector(request_msg.GetSectorId());

    name_id = ent->getPlayerEntity()->getMount()->getEntity()->GetId();
  }
  else
  {
    name_id = ent->GetId();
  }

  ptScopedMonitorable<Entity> user_ent (ent);
  user_ent->SetPosition(request_msg.GetPosition());
  user_ent->SetRotation(request_msg.GetRotation());
  user_ent->SetSector(request_msg.GetSectorId());

  DrUpdateMessage response_msg;
  response_msg.SetRotation(request_msg.GetRotation());
  response_msg.SetPosition(request_msg.GetPosition());
  response_msg.SetSectorId(request_msg.GetSectorId());
  response_msg.setEntityId(name_id);
  ByteStream bs;
  response_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
  */
}

void EntityHandler::handlePickRequest(GenericMessage* msg)
{
  /*
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  std::string name = user_ent->GetName();

  PickRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  printf("Received PickRequest from: '%s' -> '%d' \n", name.c_str(), request_msg.getItemEntityId());

  const Entity* e = server->getEntityManager()->findById(request_msg.getItemEntityId());

  PickResponseMessage response_msg;
  if (!e)
  {
    response_msg.setError(ptString("Entity doesn't exist",20)); // <-- TODO: Error Message Storage
  }
  else if (e->GetType() == Common::Entity::ItemEntityType)
  {
    response_msg.setError(ptString(0,0));
  }
  else if (e->GetType() == Common::Entity::PlayerEntityType)
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
      const InventoryEntry entry(item->GetId());

      ptScopedMonitorable<Character> character (c_char);
      bool retval = character->getInventory()->addItem(entry, slot);

      if (retval)
      {
        bool equip = (slot < 10);
        if (equip && response_msg.getError() == ptString::Null)
        {
          EquipMessage equip_msg;
          equip_msg.setEntityId(user_ent->GetId());
          if (item) equip_msg.setItemId(item->GetId());
          equip_msg.setSlotId(slot);
          equip_msg.setMeshId(item->getMesh()->GetId()); // Not used yet!
          equip_msg.setFileName(item->getMesh()->getFile());
          equip_msg.setMeshName(item->getMesh()->getName());

          ByteStream bs;
          equip_msg.serialise(&bs);
          Server::getServer()->broadCast(bs);
        }

        response_msg.setItemId(item->GetId());
        response_msg.setSlotId(slot);

        response_msg.setName(item->getName());
        response_msg.setIconName(item->getIcon());
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
  */
}

void EntityHandler::handleDropRequest(GenericMessage* msg)
{
  /*
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  std::string name = user_ent->GetName();
  DropRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());
  printf("Received DropRequest from: '%s' -> '%d' \n", name.c_str(), request_msg.getSlot());

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
    unequip_msg.setEntityId(user_ent->GetId());
    unequip_msg.setSlotId(slot_id);
    unequip_msg.setItemId(Item::NoItem); // No Item!
    unequip_msg.setMeshId(0); // Not used yet!
    unequip_msg.setFileName(ptString::Null);
    unequip_msg.setMeshName(ptString::Null);
    ByteStream bs;
    unequip_msg.serialise(&bs);
    NetworkHelper::localcast(bs, user_ent);
  }

  // Create new entity from item.
  ItemEntity* e = new ItemEntity();
  e->createFromItem(item.id);

  ptScopedMonitorable<Entity> ent (e->getEntity());
  ent->SetPosition(user_ent->GetPosition());
  ent->SetRotation(user_ent->GetRotation());
  ent->SetSector(user_ent->GetSector());

  Server::getServer()->addEntity(ent, true);
  */
} // handleDropRequest

void EntityHandler::handleMoveToRequest(GenericMessage* msg)
{
  boost::shared_ptr<PcEntity> pcentity = NetworkHelper::GetEntity(msg);
  if (!pcentity) return;

  MoveToRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  boost::shared_ptr<Character> mover;
  if (pcentity->GetMount())
  {
    mover = pcentity->GetMount();
  }
  else
  {
    mover = pcentity;
  }

  mover->SetState(Character::StateWalking);

  float speed = server->GetMovementManager()->GetMovementSpeed(mover); 

  if (pcentity->usesFlashStep())
  {
    // calc rotation
    WFMath::Vector<3> direction = mover->GetPosition() - request_msg.getTo();
    direction = WFMath::Vector<3>(direction.x(), 0, direction.z()).normalize();
    float rotation = atan2(direction.x(), direction.z());

    mover->SetPosition(request_msg.getTo());
    float oldRot = mover->GetRotation();
    mover->SetRotation(rotation);

    TeleportResponseMessage telemsg;
    telemsg.setEntityId(mover->GetId());
    telemsg.setPosition(request_msg.getTo());
    telemsg.setRotation(mover->GetRotation());
    ByteStream bs;
    telemsg.serialise(&bs);
    NetworkHelper::localcast(bs, mover);

  }
  else
  {
  
    server->GetMovementManager()->Register(mover);  
    server->moveEntity(mover, request_msg.getTo(), speed, request_msg.getRun());
  } // end if usesFlashStep

//  server->getCharacterManager()->checkForSave(pcentity);
  
} // handleMoveToRequest

void EntityHandler::handleRelocate(GenericMessage* msg)
{
  /*
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
  unsigned int ent_id = ent->GetId();
  ent->SetSectorName(*race->GetSector());
  ent->SetPosition(race->GetPosition());

  server->getCharacterManager()->checkForSave(user_ent->getPlayerEntity());

  telemsg.setEntityId(ent_id);
  unsigned short sector_id = server->GetSectorManager()->GetSectorId(race->GetSector());
  telemsg.SetSectorId(sector_id);
  telemsg.SetPosition(race->GetPosition());
  telemsg.SetRotation(ent->GetRotation());

  ByteStream bs;
  telemsg.serialise(&bs);
  server->broadCast(bs);
  */
}

void EntityHandler::handleTeleportRequest(GenericMessage* msg)
{
  boost::shared_ptr<User> user = NetworkHelper::getUser(msg);
  if (!user) return;

  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  TeleportRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  boost::shared_ptr<PcEntity> target_ent = NetworkHelper::GetEntity(msg);
  if (!target_ent) return;

  boost::shared_ptr<Character> mover;
  if (target_ent->GetMount())
  {
    mover = target_ent->GetMount();
  }
  else
  {
    mover = target_ent;
  }

  mover->SetPosition(request_msg.getPosition());
  mover->SetRotation(request_msg.getRotation());

//  server->getCharacterManager()->checkForSave(target_ent->getPlayerEntity());

  TeleportResponseMessage response_msg;
  response_msg.setEntityId(mover->GetId());
  response_msg.setPosition(request_msg.getPosition());
  response_msg.setRotation(mover->GetRotation());

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::localcast(bs, mover);
}

void EntityHandler::handleMountRequest(GenericMessage* msg)
{
  /*
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
  mmsg.setEntityId(user_ent->GetId());
  mmsg.setMoveDirection(0, 0, 0);
  mmsg.setJump(false);
  mmsg.setRun(false);

  ByteStream bs1;
  mmsg.serialise(&bs1);

  NetworkHelper::localcast(bs1, user_ent);

  MountMessage umount_msg;
  umount_msg.setMountEntityId(mount_ent->GetId());
  umount_msg.setPlayerEntityId(user_ent->GetId());
  umount_msg.setCanControl(true);

  ByteStream bs;
  umount_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
  */
}

void EntityHandler::handleUnmountRequest(GenericMessage* msg)
{
  /*
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

  if (mount_ent->GetId() != mount_id) return;

  printf("Remove player from mount!\n");
  ptScopedMonitorable<MountEntity> mount (c_mount);
  mount->delPassenger(pc_ent);

  printf("Reset the player's mount status!\n");
  ptScopedMonitorable<PcEntity> e (pc_ent);
  e->setMount(0);

  printf("Unmount finished\n");

  UnmountMessage umount_msg;
  umount_msg.setMountEntityId(mount_ent->GetId());
  umount_msg.setPlayerEntityId(user_ent->GetId());

  ByteStream bs;
  umount_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
  */
}

void EntityHandler::handlePoseRequest(GenericMessage* msg)
{
  /*
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  PoseRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  const PcEntity* pc_ent = user_ent->getPlayerEntity();
  if (!pc_ent) return;

  unsigned char pose_id = request_msg.getPoseId();

  ptScopedMonitorable<PcEntity> e (pc_ent);
  e->setPoseId(pose_id);

  PoseMessage pose_msg;
  pose_msg.setEntityId(user_ent->GetId());
  pose_msg.setPoseId(e->getPoseId());

  ByteStream bs;
  pose_msg.serialise(&bs);

  NetworkHelper::localcast(bs, user_ent);
  */
}
