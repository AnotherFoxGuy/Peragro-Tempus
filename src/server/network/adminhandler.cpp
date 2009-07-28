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

#include <sstream>

#include "server/zone/zonemanager.h"
#include "server/zone/locationmanager.h"
#include "server/quest/npcdialogmanager.h"
#include "server/species/speciesmanager.h"
#include "server/entity/entitymanager.h"
#include "server/entity/user.h"
#include "server/entity/pcentity.h"
#include "server/entity/mountentity.h"

#include "common/database/database.h"
#include "server/database/tablemanager.h"
#include "server/database/table-npcentities.h"
#include "server/database/table-npcaisetting.h"
#include "server/database/table-zones.h"
#include "server/database/table-zonenodes.h"
#include "server/database/table-config.h"
#include "server/database/table-channels.h"
#include "server/database/table-defaultchannels.h"
#include "server/database/table-meshes.h"
#include "server/database/table-species.h"

#include "server/spawn/spawner.h"

#include "server/environment/clock.h"
#include "server/environment/environmentmanager.h"

bool CheckAdminLevel(GenericMessage* msg, size_t requiredLevel)
{
  boost::shared_ptr<User> user = NetworkHelper::getUser(msg);
  if (!user) return false;

  const size_t userLevel = user->getPermissionList().getLevel(Permission::Admin);
  const size_t levelToReply = 1;
  if (userLevel < requiredLevel)
  {
    if (userLevel < levelToReply) return false;

    std::stringstream ss;
    ss << "Insufficient admin level (" << userLevel << ") for command (" <<
      requiredLevel << ")";

    ChatMessage errorMsg;
    errorMsg.setSpeakerName(ptString::Null);
    errorMsg.setVolume(1);
    errorMsg.setMessage(ss.str().c_str());
    ByteStream bs;
    errorMsg.serialise(&bs);
    msg->getConnection()->send(bs);

    return false;
  }
  else
  {
    return true;
  }
}

void AdminHandler::handleSetConfig(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  SetConfigMessage scmsg;
  scmsg.deserialise(msg->getByteStream());

  ConfigTable* ct = Server::getServer()->GetTableManager()->Get<ConfigTable>();
  ct->Insert( *(scmsg.getOption()), *(scmsg.getValue()) );
}

void AdminHandler::handleCreateMesh(GenericMessage* msg)
{
/*
  if (CheckAdminLevel(msg, 2) == false) return;

  CreateMeshMessage meshMsg;
  meshMsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  printf("AdminHandler::handleCreateMesh unimplemented\n");

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
  if (CheckAdminLevel(msg, 1) == false) return;

  SpawnItemMessage itemmsg;
  itemmsg.deserialise(msg->getByteStream());

  try
  {
    Server* server = Server::getServer();
    boost::shared_ptr<ItemEntity> item = server->GetItemTemplatesManager()
      ->CreateItemFromTemplate(itemmsg.getItemTemplateId());

    item->SetPosition(itemmsg.getPosition());
    item->SetInWorld(true);
    item->SaveToDB();
    server->getEntityManager()->Add(item);
  }
  catch (InvalidItemTemplate& )
  {
    printf("Error: item template (%i) does not exist in database\n",
      itemmsg.getItemTemplateId());
    ///@TODO maybe add a say msg back to client letting him know why
    ///      it failed.
  }
}


void AdminHandler::handleSpawnMount(GenericMessage* msg)
{

  if (CheckAdminLevel(msg, 1) == false) return;

  Server* server = Server::getServer();

  SpawnMountMessage mountmsg;
  mountmsg.deserialise(msg->getByteStream());

  try
  {
    SpeciesTable* stable = server->GetTableManager()->Get<SpeciesTable>();
    size_t speciesid = stable->FindByName( *mountmsg.getSpecies());
    if (!speciesid)
    {
      printf("Error spawning mount, species '%s' does not exist.", *mountmsg.getSpecies());
      return;
    }

    SpeciesTableVOp speciesvo = stable->GetSingle(speciesid);

    // Create the Mount entity

    boost::shared_ptr<MountEntity> mount = server->GetSpeciesManager()
      ->CreateMountFromSpecies(speciesid);

    mount->SetName(*mountmsg.getName());
    mount->SetPosition(mountmsg.getPosition());
    mount->SetRotation(mountmsg.getRotation());

    mount->SaveToDB();
    server->getEntityManager()->Add(mount);
  }
  catch (InvalidSpeciesId&)
  {
    printf("Error spawning mount, species '%s' does not exist in database\n",
      *mountmsg.getSpecies());
    ///@TODO maybe add a say msg back to client letting him know why
    ///      it failed.
  }
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
  if (CheckAdminLevel(msg, 1) == false) return;

  RemoveSpawnedEntityMessage rmmsg;
  rmmsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();
  EntityManager* entmgr = server->getEntityManager();
  Common::Entity::Entityp ent = entmgr->FindById(rmmsg.getEntityId());

  if (ent == 0) return;

  if (ent->GetType() == Common::Entity::ItemEntityType ||
      ent->GetType() == Common::Entity::MountEntityType)
  {
    boost::dynamic_pointer_cast<Entity>(ent)->DeleteFromDB();
    server->getEntityManager()->Remove(ent);
  }
}

void AdminHandler::handleToggleFlashStep(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 1) == false) return;

  boost::shared_ptr<PcEntity> pcent = NetworkHelper::GetEntity(msg);
  if (!pcent) return;
  pcent->toggleFlashStep();
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

void AdminHandler::handleCreateLocation(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity(msg);
  if (!charEnt) return;

  CreateLocationMessage locMsg;
  locMsg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();
  server->getLocationManager()->Insert( *locMsg.getName(), charEnt->GetPosition());

}

void AdminHandler::handleTeleportLocation(GenericMessage* msg)
{
  if (CheckAdminLevel(msg, 2) == false) return;

  TeleportLocationMessage teleMsg;
  teleMsg.deserialise(msg->getByteStream());

  boost::shared_ptr<Character> charEnt = NetworkHelper::GetEntity(msg);
  if (!charEnt) return;

  Server* server = Server::getServer();

  WFMath::Point<3> location(0), nowhere(0);
  location = server->getLocationManager()->GetLocation(*teleMsg.getName());

  charEnt->SetPosition(location);
  charEnt->SetRotation(charEnt->GetRotation());

  if (location!=nowhere)
  {
    TeleportResponseMessage teleRqstMsg;
    teleRqstMsg.setEntityId(charEnt->GetId());
    teleRqstMsg.setPosition(location);
    teleRqstMsg.setRotation(charEnt->GetRotation());

    ByteStream bs;
    teleRqstMsg.serialise(&bs);
    NetworkHelper::localcast(bs, charEnt);
  }
}
