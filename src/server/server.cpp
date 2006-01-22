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
#include "usermanager.h"
#include "common/entity/entitymanager.h"
#include "server/database/database.h"
#include "server/database/table-entities.h"

Server* Server::server;

void Server::addEntity(Entity* entity, bool presistent)
{
  printf("Add Entity\n");
  ent_mgr->addEntity(entity);

  if (presistent)
  {
    //Only dropped items are persistent Entities at the moment
    db->getEntityTable()->insert(entity->getId(), entity->getName(), entity->getType(), entity->getItem(), entity->getMesh(), entity->getPos(), entity->getSector());
    printf("Added %s with id: %d\n", entity->getName(), entity->getId());
  }

  for (size_t i = 0; i < usr_mgr->getUserCount(); i++)
  {
    usr_mgr->getUser(i)->sendAddEntity(entity);
  }
}

void Server::delEntity(Entity* entity)
{
  printf("Remove Entity\n");
  ent_mgr->delEntity(entity);

  if (entity->getType() == Entity::ItemEntity)
  {
    db->getEntityTable()->remove(entity->getId());
  }

  for (size_t i = 0; i < usr_mgr->getUserCount(); i++)
  {
    usr_mgr->getUser(i)->sendRemoveEntity(entity);
  }
  delete entity;
}
