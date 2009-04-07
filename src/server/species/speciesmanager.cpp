/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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


#include "speciesmanager.h"

#include <math.h>

#include "server/server.h"

#include "common/util/printhelper.h"

#include "server/entity/entitymanager.h"
#include "server/entity/entity.h"
#include "server/entity/character/character.h"
#include "server/entity/pcentity.h"
#include "server/entity/npcentity.h"

#include "server/database/tablemanager.h"

#include "server/database/table-species.h"
#include "server/database/table-speciesinventory.h"
#include "server/database/table-specieszones.h"
/*
#include "server/database/table-speciesabilities.h"
#include "server/database/table-speciesskills.h"
#include "server/database/table-speciesreputations.h"
#include "server/database/table-speciesequipment.h"

#include "server/database/table-itemtemplates.h"
#include "server/database/table-equiptypes.h"
*/
#include "server/database/table-meshes.h"
#include "server/zone/zonemanager.h"

#include "server/combat/progressionmanager.h"


SpeciesManager::SpeciesManager()
{
}

SpeciesManager::~SpeciesManager()
{
}

WFMath::Point<3> SpeciesManager::GetRandomPosition(size_t speciesId)
{
  Server* server = Server::getServer();

  SpeciesZonesTable* table = server->GetTableManager()->Get<SpeciesZonesTable>();
  SpeciesZonesTableVOArray s = table->Get(speciesId);
  if (s.size() == 0) return WFMath::Point<3>(900.765f, 8.26544f, 11.1211f); //TODO.
  size_t index = ProgressionManager::RollDice(0, s.size()-1);

  return server->getZoneManager()->GetRandomPosition(s[index]->zone_id);
}

std::list<boost::shared_ptr<Character> > SpeciesManager::GetEntitiesOfSpecies(size_t speciesId)
{
  std::list<boost::shared_ptr<Character> > chars;

  const std::map<size_t, Common::Entity::Entityp>& entities = Server::getServer()->getEntityManager()->GetEntities();
  std::map<size_t, Common::Entity::Entityp>::const_iterator it;
  for (it = entities.begin(); it != entities.end(); it++)
  {
    boost::shared_ptr<Character> c = boost::shared_dynamic_cast<Character>(it->second);
    if (c && c->GetSpecies() == speciesId) 
      chars.push_back(c);
  }

  return chars;
}

size_t SpeciesManager::GetCurrentPopulation(size_t speciesId)
{
  return GetEntitiesOfSpecies(speciesId).size();
}

size_t SpeciesManager::GetMaxPopulation(size_t speciesId)
{
  Server* server = Server::getServer();

  SpeciesTable* table = server->GetTableManager()->Get<SpeciesTable>();
  SpeciesTableVOp s = table->GetSingle(speciesId);
  if (!s)
  {
    printf("E: Invalid speciesId %"SIZET"!\n", speciesId);
    throw "Invalid speciesId !";
  }
  return 0;
  //TODO return s->maxPopulation;
}

void SetMesh(boost::shared_ptr<Character> entity, size_t meshId)
{
  Server* server = Server::getServer();
  MeshesTable* mtable = server->GetTableManager()->Get<MeshesTable>();
  MeshesTableVOp m = mtable->GetSingle(meshId);
  if (!m)
  {
    printf("E: Invalid meshId %"SIZET"!\n", meshId);
    throw "Invalid meshId !";
  }
  entity->SetMeshName(m->factoryName);
  entity->SetFileName(m->fileName);
}

void AddInventory(boost::shared_ptr<Character> entity, size_t speciesId)
{
  Server* server = Server::getServer();

  SpeciesInventoryTable* table = Server::getServer()->GetTableManager()->Get<SpeciesInventoryTable>();
  SpeciesInventoryTableVOArray arr = table->Get(speciesId);
  SpeciesInventoryTableVOArray::const_iterator it;
  for (it = arr.begin(); it != arr.end(); it++)
  {
    if ( ProgressionManager::RollCheck((*it)->chance) )
    {
      // An item with a certain chance to be added succeeded.
      Entityp ent = server->getEntityManager()->CreateNew(Common::Entity::ItemEntityType);
      boost::shared_ptr<ItemEntity> item = boost::shared_dynamic_cast<ItemEntity>(ent);
      boost::shared_ptr<ItemTemplate> itemTemplate = Server::getServer()->GetItemTemplatesManager()->Get((*it)->itemTemplates_id);
      item->SetItemTemplate(itemTemplate);
      item->SetInWorld(false);
      item->SaveToDB();
      entity->GetInventory()->AddObjectAt((*it)->slot, item);
    }
  }
}

void SpeciesManager::Initialize(boost::shared_ptr<Character> c, size_t speciesId)
{
  Server* server = Server::getServer();

  SpeciesTable* table = server->GetTableManager()->Get<SpeciesTable>();
  SpeciesTableVOp s = table->GetSingle(speciesId);
  if (!s)
  {
    printf("E: Invalid speciesId %"SIZET"!\n", speciesId);
    throw "Invalid speciesId !";
  }

  c->SetSpecies(speciesId);
  c->SetName(s->name);
  SetMesh(c, s->meshes_id);
  AddInventory(c, speciesId);

  c->SetPosition(GetRandomPosition(speciesId));
}

boost::shared_ptr<PcEntity> SpeciesManager::CreatePCFromSpecies(size_t speciesId)
{
  Server* server = Server::getServer();

  Entityp ent = server->getEntityManager()->CreateNew(Common::Entity::PCEntityType);
  boost::shared_ptr<PcEntity> pc = boost::shared_dynamic_cast<PcEntity>(ent);
  if (!pc) return boost::shared_ptr<PcEntity>();

  Initialize(pc, speciesId);

  return pc;
}

boost::shared_ptr<NpcEntity> SpeciesManager::CreateNPCFromSpecies(size_t speciesId)
{
  Server* server = Server::getServer();

  Entityp ent = server->getEntityManager()->CreateNew(Common::Entity::NPCEntityType);
  boost::shared_ptr<NpcEntity> npc = boost::shared_dynamic_cast<NpcEntity>(ent);
  if (!npc) return boost::shared_ptr<NpcEntity>();

  Initialize(npc, speciesId);

  return npc;
}
