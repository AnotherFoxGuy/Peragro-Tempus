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

  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity (msg);
  if (!charEnt) return;

  DrUpdateRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());

  boost::shared_ptr <MountEntity> mountEnt = charEnt->GetMount();
  boost::shared_ptr <Character> replyEnt;

  WFMath::Point<3> pos = requestMsg.getPosition();
  if (mountEnt)
  {
    // Make sure the character is up in the saddle
    // TODO: Maybe use a confirmation message from the client instead
    //WFMath::Point<3> mountpos = ent->getPlayerEntity()->getMount()->getEntity()->GetPosition();
    //if (pos.y < mountpos.y + 0.5f){ return; }

    pos[1] -= 1.0f; // Adjust the offset from the rider
    replyEnt = mountEnt;
  }
  else
  {
    replyEnt = charEnt;
  }

  replyEnt->SetPosition(pos);
  replyEnt->SetRotation(requestMsg.getRotation());
//  replyEnt->SetSector(requestMsg.getSectorId());

  DrUpdateMessage response_msg;
  response_msg.setRotation(requestMsg.getRotation());
  response_msg.setPosition(requestMsg.getPosition());
//  response_msg.SetSectorId(requestMsg.getSectorId());
  response_msg.setEntityId(replyEnt->GetId());
  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::localcast(bs, replyEnt);

  replyEnt->SaveToDB();
}

void EntityHandler::handlePickRequest(GenericMessage* msg)
{
  
  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity (msg);

  PickRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());

  if (charEnt)
  {
    std::string name = charEnt->GetName();
    printf("Received PickRequest from: '%s' -> '%d' to slot %i \n", name.c_str(),
      requestMsg.getItemEntityId(), requestMsg.getSlot());
  }

  Server* server = Server::getServer();

  
  // Get Item
  boost::shared_ptr<ItemEntity> itemEnt = boost::dynamic_pointer_cast<ItemEntity> 
    (server->getEntityManager()->FindById(requestMsg.getItemEntityId()));

  // Check for problems
  PickResponseMessage responseMsg;
  if (!charEnt) 
  {
    responseMsg.setError(ptString("Character Entity doesn't exist",
      strlen("Character Entity doesn't exist"))); // <-- TODO: Error Message Storage 
  }
  else if (!itemEnt)
  {
    responseMsg.setError(ptString("Item Entity doesn't exist",
      strlen("Item Entity doesn't exist"))); // <-- TODO: Error Message Storage
  }
  else if (!itemEnt->GetInWorld())
  {
    responseMsg.setError(ptString("Item Entity not in world",
      strlen("Item Entity not in world"))); // <-- TODO: Error Message Storage
  }

  if (responseMsg.getError().isNull())
  {
    unsigned int slot = requestMsg.getSlot();
    responseMsg.setEntityId(itemEnt->GetId());
    responseMsg.setSlotId(slot);

    if (!charEnt->GetInventory()->HasObjectAt(slot))
    {

      bool retVal = charEnt->GetInventory()->AddObjectAt(slot,itemEnt);
      if (retVal)
      {
        responseMsg.setName(itemEnt->GetName());
        responseMsg.setIconName(itemEnt->GetIcon());
        responseMsg.setDescription(itemEnt->GetDescription());
        responseMsg.setWeight(itemEnt->GetWeight());
        responseMsg.setEquipType(itemEnt->GetEquipType());

        ByteStream bs;
        responseMsg.serialise(&bs);
        NetworkHelper::sendMessage(charEnt,bs);
        return;
      }        
      responseMsg.setError(ptString("Couldn't add item!",strlen("Couldn't add item!"))); // <-- TODO: Error Message Storage
    } else
    {
      responseMsg.setError(ptString("Failed adding inventory,Slot is full!",
        strlen("Failed adding inventory,Slot is full!"))); // <-- TODO: Error Message Storage
    }  // end if slot empty
  }  // end if no errMsg 

  ByteStream bs;
  responseMsg.serialise(&bs);
  NetworkHelper::sendMessage(charEnt, bs); // error msg
  
}

void EntityHandler::handleDropRequest(GenericMessage* msg)
{

  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity(msg);
  if (!charEnt) return;

  std::string name = charEnt->GetName();
  DropRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());
  printf("Received DropRequest from: '%s' -> '%d' \n", name.c_str(), requestMsg.getSlot());

  DropResponseMessage responseMsg;
  unsigned char slotId = requestMsg.getSlot();

  // Get Item
  boost::shared_ptr<ItemEntity> itemEnt = boost::dynamic_pointer_cast<ItemEntity> 
    (charEnt->GetInventory()->RemoveObjectAt(slotId));

  if (!itemEnt)
  {
    responseMsg.setError(ptString("Unable to drop, slot empty!", strlen("Unable to drop, slot empty!")));
    ByteStream bs;
    responseMsg.serialise(&bs);
    NetworkHelper::sendMessage(charEnt, bs);
    return;
  }

  responseMsg.setSlotId(slotId);

  ByteStream bs;
  responseMsg.serialise(&bs);
  NetworkHelper::sendMessage(charEnt, bs);

  itemEnt->SetPosition(charEnt->GetPosition());
  itemEnt->SetRotation(charEnt->GetRotation());
//  itemEnt->SetSector(charEnt->GetSector());
  itemEnt->SetInWorld(true);
  itemEnt->SaveToDB();
  Server::getServer()->getEntityManager()->Add(itemEnt);

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

  TeleportRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());

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

  mover->SetPosition(requestMsg.getPosition());
  mover->SetRotation(requestMsg.getRotation());

  TeleportResponseMessage responseMsg;
  responseMsg.setEntityId(mover->GetId());
  responseMsg.setPosition(requestMsg.getPosition());
  responseMsg.setRotation(mover->GetRotation());

  ByteStream bs;
  responseMsg.serialise(&bs);

  NetworkHelper::localcast(bs, mover);

}

void EntityHandler::handleMountRequest(GenericMessage* msg)
{

  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity(msg);
  if (!charEnt) return;

  MountRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();
  unsigned int mount_id = requestMsg.getMountEntityId();

  boost::shared_ptr<MountEntity> cMount = boost::dynamic_pointer_cast<MountEntity>
    (server->getEntityManager()->FindById(mount_id));
  if (!cMount) return;

  charEnt->SetMount(cMount);
  cMount->AddPassenger(charEnt);

  // Don't run into the horse! 
  MoveMessage mmsg;
  mmsg.setEntityId(charEnt->GetId());

  mmsg.setMoveDirection(0, 0, 0);
  mmsg.setJump(false);
  mmsg.setRun(false);
  ByteStream bs1;
  mmsg.serialise(&bs1);

  NetworkHelper::localcast(bs1, charEnt);

  MountMessage umount_msg;
  umount_msg.setMountEntityId(cMount->GetId());
  umount_msg.setPlayerEntityId(charEnt->GetId());
  umount_msg.setCanControl(true);
  ByteStream bs;
  umount_msg.serialise(&bs);
  NetworkHelper::localcast(bs, charEnt);
}

void EntityHandler::handleUnmountRequest(GenericMessage* msg)
{

  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity(msg);
  if (!charEnt) return;

  UnmountRequestMessage requestMsg;
  requestMsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();
  unsigned int mountId = requestMsg.getMountEntityId();

  boost::shared_ptr<MountEntity> cMount = boost::dynamic_pointer_cast<MountEntity>
    (server->getEntityManager()->FindById(mountId));
  if (!cMount) return;

  //Remove player from mount!
  cMount->RemovePassenger(charEnt);

  //Reset the player's mount status!
  boost::shared_ptr<MountEntity> noMount;
  charEnt->SetMount(noMount);

  // Send unmount message to peers
  UnmountMessage umountMsg;
  umountMsg.setMountEntityId(cMount->GetId());
  umountMsg.setPlayerEntityId(charEnt->GetId());

  ByteStream bs;
  umountMsg.serialise(&bs);

  NetworkHelper::localcast(bs, charEnt);
}

void EntityHandler::handlePoseRequest(GenericMessage* msg)
{
 
  boost::shared_ptr<PcEntity> pcEnt = NetworkHelper::GetEntity(msg);
  if (!pcEnt) return;

  PoseRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  unsigned char poseId = request_msg.getPoseId();

  pcEnt->setPoseId(poseId);

  PoseMessage pose_msg;
  pose_msg.setEntityId(pcEnt->GetId());
  pose_msg.setPoseId(pcEnt->getPoseId());

  ByteStream bs;
  pose_msg.serialise(&bs);

  NetworkHelper::localcast(bs, pcEnt);
 
}
