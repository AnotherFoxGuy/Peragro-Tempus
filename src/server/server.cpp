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
#include "entity/npcentity.h"
#include "entity/usermanager.h"
#include "server/entity/entitymanager.h"
#include "server/database/database.h"
#include "server/database/table-entities.h"
#include "server/database/table-characters.h"
#include "common/network/entitymessages.h"
#include "server/network/connection.h"
#include "server/network/networkhelper.h"
#include "common/util/sleep.h"

Server* Server::server;

void Server::addEntity(const Entity* entity, bool presistent)
{
  printf("Add Entity\n");
  ent_mgr->addEntity(entity);

  printf("Added %s with id: %d, mesh: %s\n", *entity->getName(), entity->getId(), *entity->getMesh());

  if (presistent)
  {
    //Only dropped items are persistent Entities at the moment
    const ItemEntity* e = entity->getItemEntity();
    if (!e) return;
    db->getEntityTable()->insert(entity->getId(), entity->getName(), entity->getType(), e->getItem()->getId(), entity->getMesh(), entity->getPos(), entity->getSectorName());
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
    int id = entity->getPlayerEntity()->getCharacter()->getId();
    db->getCharacterTable()->update(entity->getPos(), entity->getSectorName(), id);
  }

  for (size_t i = 0; i < usr_mgr->getUserCount(); i++)
  {
    usr_mgr->getUser(i)->sendRemoveEntity(entity);
  }
  // giving the rest of the world time to finish.
  // Not really nice but hopefully effective
  pt_sleep(100);
  delete entity;
}

void Server::moveEntity(PcEntity* entity, float* pos, float speed)
{
  MoveToMessage response_msg;
  response_msg.setTo(pos);
  response_msg.setFrom(entity->getEntity()->getPos());
  response_msg.setSpeed(speed);
  response_msg.setEntityId(entity->getEntity()->getId());

  entity->walkTo(pos, speed);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::broadcast(bs);
}

void Server::moveEntity(const NpcEntity* entity, float* pos, float speed)
{
  if (!entity) return;

  MoveToMessage response_msg;
  response_msg.setTo(pos);
  response_msg.setFrom(entity->getEntity()->getPos());
  response_msg.setSpeed(speed);
  response_msg.setEntityId(entity->getEntity()->getId());

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
