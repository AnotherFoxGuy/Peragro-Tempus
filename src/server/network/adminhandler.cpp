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

#include "server/entity/entitymanager.h"
#include "server/zone/zonemanager.h"
#include "server/entity/user.h"

#include "server/quest/npcdialogmanager.h"

#include "common/database/database.h"
#include "server/database/tablemanager.h"
#include "server/database/table-npcentities.h"
#include "server/database/table-npcaisetting.h"
#include "server/database/table-zones.h"
#include "server/database/table-zonenodes.h"
#include "server/database/table-config.h"
#include "server/database/table-channels.h"
#include "server/database/table-defaultchannels.h"

#include "server/spawn/spawner.h"

#include "server/environment/clock.h"
#include "server/environment/environmentmanager.h"

bool CheckAdminLevel(GenericMessage* msg, size_t level)
{
  boost::shared_ptr<User> user = NetworkHelper::getUser(msg);
  if (!user) return false;

  size_t admin = user->getPermissionList().getLevel(Permission::Admin);
  if (admin < level) return false;
  else return true;
}

void AdminHandler::handleRemoveAll(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 2) == false) return;

  RemoveAllMessage rmmsg;
  rmmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  if (rmmsg.getDataType() == ptString::create("npc-dialogs"))
  {
      NPCDialogManager::getDialogManager().delAll();
  }
  else if (rmmsg.getDataType() == ptString::create("npc-entities"))
  {
    EntityManager* entities = server->getEntityManager();
    Array<const Entity*> npcs;
    for (size_t i = 0; i < entities->getEntityCount(); i++)
    {
      if (entities->getEntity(i)->getNpcEntity())
      {
        const Entity* e = entities->getEntity(i);
        npcs.add(e);
      }
    }
    for (size_t i = 0; i < npcs.getCount(); i++)
    {
      const Character* c = npcs.get(i)->getNpcEntity()->getCharacter();
      server->getCharacterManager()->delCharacter(c);

      NpcEntitiesTable* net = server->getTables()->getNpcEntitiesTable();
      net->remove(npcs.get(i)->GetId());

      NpcAiSettingTable* nast = server->getTables()->getNpcAiSettingTable();
      nast->removeAll(npcs.get(i)->GetId());

      server->delEntity(npcs.get(i));
    }
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
      server->getTables()->getDoorsTable()->remove(entity->getDoorId());
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
    SectorManager* sectors = server->GetSectorManager();
    sectors->delAll();
  }
  else if (rmmsg.getDataType() == ptString::create("zones"))
  {
    ZoneManager* zones = server->getZoneManager();
    zones->delAll();
  }
  */
}

void AdminHandler::handleSetConfig(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  SetConfigMessage scmsg;
  scmsg.deserialise(msg->getByteStream());
  
  ConfigTable* ct = Server::getServer()->GetTableManager()->Get<ConfigTable>();
  ct->Insert( *(scmsg.getOption()), *(scmsg.getValue()) );
}

void AdminHandler::handleCreateItem(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 2) == false) return;

  CreateItemMessage itemmsg;
  itemmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  const Mesh* mesh = server->getMeshManager()->addMeshUpdate(itemmsg.getMesh(), itemmsg.getFile());

  ItemManager* items = server->getItemManager();
  ItemTable* it = server->getTables()->getItemTable();
  it->insert(itemmsg.getItemId(), itemmsg.getName(), itemmsg.getIcon(),
             itemmsg.getDescription(), mesh->GetId(),
             itemmsg.getWeight(), itemmsg.getEquipType());

  Item* item = it->getItem(itemmsg.getName(), server->getMeshManager());

  items->addItem(item);
  */
}

void AdminHandler::handleCreateSpawnPoint(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 2) == false) return;

  CreateSpawnPointMessage spawnmsg;
  spawnmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  const float x = spawnmsg.GetPosition()[0];
  const float y = spawnmsg.GetPosition()[1];
  const float z = spawnmsg.GetPosition()[2];

  SectorManager* sectormgr = server->GetSectorManager();

  ptString sector = sectormgr->GetSectorName(spawnmsg.GetSectorId());

  unsigned int item = spawnmsg.getItemId();

  unsigned int interval = spawnmsg.getInterval();

  Spawner* spawner = server->getSpawner();
  spawner->createSpawnPoint(x, y, z, sector, item, interval);
  */
}

void AdminHandler::handleSpawnItem(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 1) == false) return;

  SpawnItemMessage itemmsg;
  itemmsg.deserialise(msg->getByteStream());

  ItemEntity* item_ent = new ItemEntity();
  item_ent->createFromItem(itemmsg.getItemId());

  ptScopedMonitorable<Entity> e (item_ent->getEntity());
  e->SetPosition(itemmsg.GetPosition());
  e->SetSector(itemmsg.GetSectorId());
  e->SetRotation(0.0f);

  Server::getServer()->addEntity(item_ent->getEntity(), true);
  */
}

void AdminHandler::handleSpawnMount(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 1) == false) return;

  Server* server = Server::getServer();

  SpawnMountMessage mountmsg;
  mountmsg.deserialise(msg->getByteStream());

  const Mesh* mesh = server->getMeshManager()->findByName(mountmsg.getMesh());
  if (mesh == 0) mesh = server->getMeshManager()->addMeshUpdate(mountmsg.getMesh(), ptString::Null);

  MountEntity* mount_ent = new MountEntity();

  ptScopedMonitorable<Entity> e (mount_ent->getEntity());
  e->SetName(*mountmsg.getName());
  e->setMesh(mesh);
  e->SetPosition(mountmsg.GetPosition());
  e->SetRotation(mountmsg.GetRotation());
  e->SetSector(mountmsg.GetSectorId());

  Server::getServer()->addEntity(mount_ent->getEntity(), true);
  */
}

void AdminHandler::handleSpawnDoor(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 2) == false) return;

  Server* server = Server::getServer();

  SpawnDoorMessage doormsg;
  doormsg.deserialise(msg->getByteStream());

  const Mesh* mesh = server->getMeshManager()->findByName(doormsg.getMesh());
  if (mesh == 0) mesh = server->getMeshManager()->addMeshUpdate(doormsg.getMesh(), ptString::Null);

  DoorEntity* door_ent = new DoorEntity();
  door_ent->setDoorId(doormsg.getDoorId());
  door_ent->setLocked(doormsg.getIsLocked());
  door_ent->setOpen(doormsg.getIsOpen());
  door_ent->setAnimation(doormsg.getAnimation());

  ptScopedMonitorable<Entity> e (door_ent->getEntity());
  e->SetName(*doormsg.getName());
  e->setMesh(mesh);
  e->SetPosition(doormsg.GetPosition());
  e->SetRotation(0.0f);
  e->SetSector(doormsg.GetSectorId());

  Server::getServer()->addEntity(door_ent->getEntity(), true);
  */
}

void AdminHandler::handleRemoveSpawnedEntity(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 1) == false) return;

  RemoveSpawnedEntityMessage rmmsg;
  rmmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  unsigned int entid = rmmsg.getEntityId();
  const Entity* e = server->getEntityManager()->findById(entid);
  if (e == 0) return;
  if (e->GetType() == Common::Entity::ItemEntityType ||
      e->GetType() == Common::Entity::MountEntityType)
  {
    server->delEntity(e);
  }
  */
}

void AdminHandler::handleToggleFlashStep(GenericMessage* msg)
{
  /*
  if (CheckAdminLevel(msg, 1) == false) return;

  const PcEntity* c_pcent = NetworkHelper::getPcEntity(msg);
  if (!c_pcent) return;

  ptScopedMonitorable<PcEntity> pcent (c_pcent);
  pcent->toggleFlashStep();
  */
}

void AdminHandler::handleCreateZone(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  CreateZoneMessage zonemsg;
  zonemsg.deserialise(msg->getByteStream());

  ZoneManager* zonemgr = Server::getServer()->getZoneManager();
  ZonesTable* ztbl = Server::getServer()->GetTableManager()->Get<ZonesTable>();
  ZoneNodesTable* zntbl = Server::getServer()->GetTableManager()->Get<ZoneNodesTable>();

  ZoneManager::Zone zone;
  zone.type = *zonemsg.getZoneType();
  ztbl->Insert(zonemsg.getZoneId(), *zonemsg.getZoneType());
  //The nodes need to be reversed here to be in the right order for GetZone(..)
  for(int i=zonemsg.getNodesCount()-1; i>-1; i--)
  {
    zntbl->Insert(zonemsg.getZoneId(), WFMath::Point<2>(zonemsg.getX(i), zonemsg.getZ(i)) );
    zone.coords.push_back(WFMath::Point<2>(zonemsg.getX(i), zonemsg.getZ(i)));
  }
  zonemgr->AddZone(zonemsg.getZoneId(), zone);
}

void AdminHandler::handleSetDate(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  SetDateMessage dateMsg;
  dateMsg.deserialise(msg->getByteStream());

  PT::Date::IntegerDate date(dateMsg.getSeconds());

  PT::Server::Environment::Clock* clock =
    Server::getServer()->getEnvironmentManager()->GetClock();
  clock->SetDate(date);
  clock->BroadcastTime();
}

void AdminHandler::handleCreateChanDefault(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  CreateChanDefaultMessage ccdmsg;
  ccdmsg.deserialise(msg->getByteStream());
  
  std::string channel = *(ccdmsg.getGroup());
  TableManager* tablemgr = Server::getServer()->GetTableManager();

  if (ccdmsg.getIsInitChan())
    tablemgr->Get<ChannelsTable>()->Insert(channel);
  else
    tablemgr->Get<DefaultChannelsTable>()->Insert(channel, ccdmsg.getPermanent());

}

void AdminHandler::handleCreateChanSpace(GenericMessage* msg)
{
/* TODO */
}

