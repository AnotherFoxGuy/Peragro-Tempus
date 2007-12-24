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

#include "server.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "entity/itementity.h"
#include "entity/pcentity.h"
#include "entity/mountentity.h"
#include "entity/npcentity.h"
#include "entity/usermanager.h"
#include "server/entity/entitymanager.h"
#include "server/database/database.h"
#include "server/database/table-entities.h"
#include "server/database/table-characters.h"
#include "server/database/table-doors.h"
#include "common/network/entitymessages.h"
#include "server/network/connection.h"
#include "server/network/networkhelper.h"
#include "common/util/sleep.h"

Server* Server::server;

void Server::addEntity(const Entity* entity, bool persistent)
{
  printf("Add Entity\n");
  ent_mgr->addEntity(entity);

  printf("Added %s with id: %d, mesh: %s\n", *entity->getName(), entity->getId(), *entity->getMesh());

  if (persistent)
  {
    const ItemEntity* ie = entity->getItemEntity();
    const DoorEntity* de = entity->getDoorEntity();
    if (ie)
    { 
      db->getEntityTable()->insert(entity->getId(), entity->getName(), entity->getType(), ie->getItem()->getId(), ie->variation, entity->getMesh(), entity->getPos(), entity->getRotation(), entity->getSectorName());
    }
    else if (de)
    {
      // Doors don't go to entity table!
      DoorsTableVO vo;
      vo.id = de->getDoorId();
      vo.islocked = de->getLocked();
      vo.isopen = de->getOpen();
      vo.mesh = entity->getMesh();
      vo.name = entity->getName();
      vo.sector = entity->getSectorName();
      vo.x = entity->getPos()[0];
      vo.y = entity->getPos()[1];
      vo.z = entity->getPos()[2];
      db->getDoorsTable()->insert(&vo);
    }
    else
    {
      db->getEntityTable()->insert(entity->getId(), entity->getName(), entity->getType(), 0, 0, entity->getMesh(), entity->getPos(), entity->getRotation(), entity->getSectorName());
    }
  }

  for (size_t i = 0; i < usr_mgr->getUserCount(); i++)
  {
    usr_mgr->getUser(i)->sendAddEntity(entity);
  }
}

void Server::delEntity(const Entity* entity)
{
  printf("Remove Entity\n");

  ent_mgr->removeEntity(entity);

  if (entity->getType() == Entity::ItemEntityType)
  {
    db->getEntityTable()->remove(entity->getId());
  }
  if (entity->getType() == Entity::PlayerEntityType)
  {
    const PcEntity* pc_ent = entity->getPlayerEntity();
    int id = pc_ent->getCharacter()->getId();
    db->getCharacterTable()->update(entity->getPos(), entity->getRotation(), entity->getSectorName(), id);

    // check if player was on mount
    const MountEntity* c_mount = pc_ent->getMount();
    if (c_mount)
    {
      PcEntity* e = pc_ent->getLock();
      MountEntity* mount = c_mount->getLock();

      e->setMount(0);
      mount->delPassenger(e);

      e->freeLock();
      mount->freeLock();
    }
  }

  for (size_t i = 0; i < usr_mgr->getUserCount(); i++)
  {
    usr_mgr->getUser(i)->sendRemoveEntity(entity);
  }
  // giving the rest of the world time to finish.
  // Not really nice but hopefully effective
  pt_sleep(100);

  // Unlocks on destruction (I hope)
  entity->getLock();
  delete entity;
}

void Server::moveEntity(PcEntity* entity, float* pos, float speed, bool run)
{
  MoveToMessage response_msg;
  response_msg.setTo(pos);
  response_msg.setFrom(entity->getEntity()->getPos());
  response_msg.setSpeed(speed);
  response_msg.setEntityId(entity->getEntity()->getId());
  response_msg.setRun(run);

  entity->walkTo(pos, speed);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::broadcast(bs);
}

void Server::moveEntity(MountEntity* entity, float* pos, float speed, bool run)
{
  MoveToMessage response_msg;
  response_msg.setTo(pos);
  response_msg.setFrom(entity->getEntity()->getPos());
  response_msg.setSpeed(speed);
  response_msg.setEntityId(entity->getEntity()->getId());
  response_msg.setRun(run);

  entity->walkTo(pos, speed);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::broadcast(bs);
}

void Server::moveEntity(const NpcEntity* entity, float* pos, float speed, bool run)
{
  if (!entity) return;

  MoveToMessage response_msg;
  response_msg.setTo(pos);
  response_msg.setFrom(entity->getEntity()->getPos());
  response_msg.setSpeed(speed);
  response_msg.setEntityId(entity->getEntity()->getId());
  response_msg.setRun(run);

  NpcEntity* npc = entity->getLock();
  npc->walkTo(pos, speed);
  npc->freeLock();

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::broadcast(bs);
}

void Server::broadCast(const ByteStream& bs)
{
  for (size_t i=0; i<getUserManager()->getUserCount(); i++)
  {
    User* user = getUserManager()->getUser(i);
    if (user && user->getConnection())
      user->getConnection()->send(bs);
  }
}
