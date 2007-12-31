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
#include "server/entity/entitymanager.h"
#include "server/entity/itemmanager.h"
#include "server/entity/sectormanager.h"
#include "server/entity/user.h"

#include "server/database/database.h"
#include "server/database/table-npcentities.h"
#include "server/database/table-npcaisetting.h"

#include "server/spawner.h"

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

void AdminHandler::handleCreateNpc(GenericMessage* msg)
{
  CreateNpcMessage npcmsg;
  npcmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  CharacterManager* charmgr = server->getCharacterManager();

  int charid = -1;
  ptString error = charmgr->createCharacter(npcmsg.getName(), 0, charid, 
    npcmsg.getHairColour(), npcmsg.getSkinColour(), npcmsg.getDecalColour());

  if (! error.isNull())
  {
    printf(*error);
    return;
  }

  Database* db = Server::getServer()->getDatabase();

  Character* character = charmgr->getCharacter(charid, 0);
  character->getInventory()->loadFromDatabase(db->getInventoryTable(), charid);

  for (unsigned char i = 0; i < npcmsg.getInventoryCount(); i++)
  {
    InventoryEntry entry(npcmsg.getItemId(i), npcmsg.getVariation(i));
    character->getInventory()->addItem(entry, npcmsg.getSlotId(i));
  }

  NpcEntity* npcentity = new NpcEntity();
  npcentity->setCharacter(character);
  npcentity->setStartDialog(npcmsg.getDialog());

  Entity* entity = npcentity->getEntity()->getLock();
  entity->setSector(npcmsg.getSectorId());
  entity->setPos(npcmsg.getPos());
  entity->setRotation(npcmsg.getRotation());
  entity->setMesh(npcmsg.getMesh());
  entity->setName(npcmsg.getName());
  entity->freeLock();

  server->addEntity(npcentity->getEntity(), true);

  db->getNpcEntitiesTable()->insert(entity->getId(), charid, npcmsg.getAi(), npcmsg.getDialog());

  for (unsigned char i = 0; i < npcmsg.getAiSettingCount(); i++)
  {
    db->getNpcAiSettingTable()->insert(charid, npcmsg.getKey(i), *npcmsg.getValue(i));
  }
  npcentity->setAI(AI::createAI(npcmsg.getAi()));
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

void AdminHandler::handleSpawnItem(GenericMessage* msg)
{
  const User* user = NetworkHelper::getUser(msg);
  if (!user) return;
  
  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  SpawnItemMessage itemmsg;
  itemmsg.deserialise(msg->getByteStream());

  ItemEntity* item_ent = new ItemEntity();
  item_ent->createFromItem(itemmsg.getItemId(), itemmsg.getVariation());

  Entity* e = item_ent->getEntity()->getLock();
  e->setPos(itemmsg.getPos());
  e->setSector(itemmsg.getSectorId());
  e->setRotation(0);
  e->freeLock();

  Server::getServer()->addEntity(item_ent->getEntity(), true);
}

void AdminHandler::handleSpawnMount(GenericMessage* msg)
{
  const User* user = NetworkHelper::getUser(msg);
  if (!user) return;
  
  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  SpawnMountMessage mountmsg;
  mountmsg.deserialise(msg->getByteStream());

  MountEntity* mount_ent = new MountEntity();

  Entity* e = mount_ent->getEntity()->getLock();
  e->setName(mountmsg.getName());
  e->setMesh(mountmsg.getMesh());
  e->setPos(mountmsg.getPos());
  e->setRotation(mountmsg.getRotation());
  e->setSector(mountmsg.getSectorId());
  e->freeLock();

  Server::getServer()->addEntity(mount_ent->getEntity(), true);
}

void AdminHandler::handleSpawnDoor(GenericMessage* msg)
{
  const User* user = NetworkHelper::getUser(msg);
  if (!user) return;
  
  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  SpawnDoorMessage doormsg;
  doormsg.deserialise(msg->getByteStream());
  
  DoorEntity* door_ent = new DoorEntity();
  door_ent->setDoorId(doormsg.getDoorId());
  door_ent->setLocked(doormsg.getIsLocked());
  door_ent->setOpen(doormsg.getIsOpen());
  door_ent->setAnimation(doormsg.getAnimation());

  Entity* e = door_ent->getEntity()->getLock();
  e->setName(doormsg.getName());
  e->setMesh(doormsg.getMesh());
  e->setPos(doormsg.getPos());
  e->setRotation(0.0f);
  e->setSector(doormsg.getSectorId());
  e->freeLock();

  Server::getServer()->addEntity(door_ent->getEntity(), true);
}

void AdminHandler::handleRemoveSpawnedEntity(GenericMessage* msg)
{
  const User* user = NetworkHelper::getUser(msg);
  if (!user) return;
  
  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  RemoveSpawnedEntityMessage rmmsg;
  rmmsg.deserialise(msg->getByteStream());
  
  Server* server = Server::getServer();

  unsigned int entid = rmmsg.getEntityId();
  const Entity* e = server->getEntityManager()->findById(entid);
  if (e == 0) return;
  if (e->getType() == Entity::ItemEntityType || 
      e->getType() == Entity::MountEntityType)
  {
    server->delEntity(e);
  }
}

void AdminHandler::handleToggleFlashStep(GenericMessage* msg)
{
  const User* user = NetworkHelper::getUser(msg);
  if (!user) return;
  
  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin == 0) return;

  const PcEntity* c_pcent = NetworkHelper::getPcEntity(msg);
  if (!user) return;

  PcEntity* pcent = c_pcent->getLock();
  pcent->toggleFlashStep();
  pcent->freeLock();
}
