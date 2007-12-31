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

#include "network.h"
#include "networkhelper.h"

#include "server/entity/charactermanager.h"
#include "server/entity/doormanager.h"
#include "server/entity/itemmanager.h"
#include "server/entity/sectormanager.h"

#include "server/database/database.h"
#include "server/database/table-npcentities.h"
#include "server/database/table-npcaisetting.h"

#include "server/spawner.h"

void AdminHandler::handleCreateSector(GenericMessage* msg)
{
  CreateSectorMessage sectormsg;
  sectormsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  SectorManager* sectors = server->getSectorManager();
  sectors->addSector(sectormsg.getSectorId(), sectormsg.getName(), sectormsg.getRegion());
}

void AdminHandler::handleCreateItem(GenericMessage* msg)
{
  CreateItemMessage itemmsg;
  itemmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  ItemManager* items = server->getItemManager();
  ItemTable* it = server->getDatabase()->getItemTable();
  it->insert(itemmsg.getName(), itemmsg.getIcon(), itemmsg.getDescription(),
             itemmsg.getFile(), itemmsg.getMesh(), itemmsg.getWeight(),
             itemmsg.getEquipType());

  Item* item = it->getItem(itemmsg.getName());

  items->addItem(item);
}

void AdminHandler::handleRemoveAll(GenericMessage* msg)
{
  RemoveAllMessage rmmsg;
  rmmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  if (rmmsg.getDataType() == ptString::create("npc-dialogs"))
  {
  }
  else if (rmmsg.getDataType() == ptString::create("npc-entities"))
  {
    //EntityManager* entities = server->getEntityManager();
    //Array<const Entity*> npcs;
    //for (int i = 0; i < entities->getEntityCount(); i++)
    //{
    //  if (entities->getEntity(i)->getNpcEntity())
    //  {
    //    const Entity* e = entities->getEntity(i);
    //    npcs.add(e);
    //  }
    //}
    //for (int i = 0; i < npcs.getCount(); i++)
    //{
    //  const Character* c = npcs.get(i)->getNpcEntity()->getCharacter();
    //  server->getCharacterManager()->delCharacter(c);

    //  NpcEntitiesTable* net = server->getDatabase()->getNpcEntitiesTable();
    //  net->remove(npcs.get(i)->getId());
    //  
    //  NpcAiSettingTable* nast = server->getDatabase()->getNpcAiSettingTable();
    //  nast->removeAll(npcs.get(i)->getId());
    //  
    //  server->delEntity(npcs.get(i));
    //}
  }
  else if (rmmsg.getDataType() == ptString::create("item-entities"))
  {
    //EntityManager* entities = server->getEntityManager();
    //Array<const Entity*> items;
    //for (int i = 0; i < entities->getEntityCount(); i++)
    //{
    //  if (entities->getEntity(i)->getItemEntity())
    //  {
    //    const Entity* e = entities->getEntity(i);
    //    items.add(e);
    //  }
    //}
    //for (int i = 0; i < items.getCount(); i++)
    //{
    //  server->delEntity(items.get(i));
    //}
  }
  else if (rmmsg.getDataType() == ptString::create("mount-entities"))
  {
    //EntityManager* entities = server->getEntityManager();
    //Array<const Entity*> mounts;
    //for (int i = 0; i < entities->getEntityCount(); i++)
    //{
    //  if (entities->getEntity(i)->getMountEntity())
    //  {
    //    const Entity* e = entities->getEntity(i);
    //    mounts.add(e);
    //  }
    //}
    //for (int i = 0; i < mounts.getCount(); i++)
    //{
    //  server->delEntity(mounts.get(i));
    //}
  }
  else if (rmmsg.getDataType() == ptString::create("doors"))
  {
    DoorManager* doors = server->getDoorManager();
    while (doors->getDoorCount() > 0)
    {
      DoorEntity* entity = doors->getDoor(0);
      doors->delDoor(entity);
      server->getDatabase()->getDoorsTable()->remove(entity->getDoorId());
      server->delEntity(entity->getEntity());
    }
  }
  else if (rmmsg.getDataType() == ptString::create("spawnpoints"))
  {
    Spawner* spawner = server->getSpawner();
    spawner->removeAllSpawnPoints();
  }
  else if (rmmsg.getDataType() == ptString::create("items"))
  {
    ItemManager* items = server->getItemManager();
    while (items->getItemCount() > 0)
    {
      items->delItem((size_t)0);
    }
  }
  else if (rmmsg.getDataType() == ptString::create("sectors"))
  {
    SectorManager* sectors = server->getSectorManager();
    sectors->delAll();
  }
}

void AdminHandler::handleCreateNpc(GenericMessage* msg)
{
  //TODO: Implement it! :)
}

void AdminHandler::handleCreateSpawnPoint(GenericMessage* msg)
{
  CreateSpawnPointMessage spawnmsg;
  spawnmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  float x = spawnmsg.getPos()[0];
  float y = spawnmsg.getPos()[1];
  float z = spawnmsg.getPos()[2];

  SectorManager* sectormgr = server->getSectorManager();

  ptString sector = sectormgr->getSectorName(spawnmsg.getSectorId());

  unsigned int item = spawnmsg.getItemId();
  //unsigned int variation = spawnmsg.getVariation();

  unsigned int interval = spawnmsg.getInterval();

  Spawner* spawner = server->getSpawner();
  spawner->createSpawnPoint(x, y, z, sector, item, interval);
}
