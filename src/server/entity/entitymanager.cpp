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

#include "entitymanager.h"

#include "common/entity/entitycallback.h"

#include "common/database/database.h"
#include "server/database/tablemanager.h"
#include "server/database/table-entities.h"
#include "server/database/table-characters.h"
#include "server/database/table-npcentities.h"

#include "user.h"

#include "itementity.h"
#include "pcentity.h"
#include "npcentity.h"
#include "doorentity.h"
#include "mountentity.h"

#include "server/server.h"
#include "server/colldet/colldet.h"

#include "common/util/preformattedinfo.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
  ConstIterator it = entities.begin();
  for ( ; it != entities.end(); it++)
  {
    if (it->second) it->second->RemoveListener(this);
  }
}

void EntityManager::LoadFromDB(EntityTable* table)
{
  //EntityTable* table = Server::getServer()->GetTableManager()->Get<EntityTable>();
  EntitiesTableVOArray arr = table->GetWorldEntities();
  EntitiesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Entityp entity = CreateNew((Common::Entity::EntityType)(*it)->EntityTypes_id, (*it)->id);
    entity->LoadFromDB();
    Add(entity);
    printf("Loading Entity %d %s\n", entity->GetId(), entity->GetName().c_str());
  }
}

Entityp EntityManager::CreateNew(Common::Entity::EntityType type, size_t id)
{
  size_t entityId = 0;
  if (id == Common::Entity::Entity::NoEntity)
  {
    entityId = Server::getServer()->GetTableManager()->Get<EntityTable>()->GetMaxId();
    entityId++;
  }
  else
    entityId = id;

  Entityp entity;
  switch (type)
  {
  case Common::Entity::PCEntityType: entity = Entityp(new PcEntity()); break;
  case Common::Entity::NPCEntityType: entity = Entityp(new NpcEntity()); break;
  case Common::Entity::DoorEntityType: entity = Entityp(new DoorEntity()); break;
  case Common::Entity::ItemEntityType: entity = Entityp(new ItemEntity()); break;
  case Common::Entity::MountEntityType: entity = Entityp(new MountEntity()); break;
  default:
    {
      throw PT_EX(PT::Exception("Invalid EntityType"))
        << PT_FORMAT_INFO("EntityType: %1%", % type);
    }
  }
  if (entity)
  {
    entity->SetId(entityId);
    // Reserve a spot.
    Server::getServer()->GetTableManager()->Get<EntityTable>()->Insert(entityId, (size_t)type);
  }

  return entity;
}

bool EntityManager::Add(Common::Entity::Entityp entity)
{
  bool success = Common::Entity::EntityManager::Add(entity);

  if (success)
  {
    using namespace Common::Entity;
    if ((entity->GetType() == PCEntityType)
      ||(entity->GetType() == NPCEntityType)
      ||(entity->GetType() == MountEntityType))
    {
      //Server::getServer()->getCollisionDetection()->addEntity(entity);
    }

    // It's a movable entity, so listen for it's movement.
    if ((entity->GetType() == PCEntityType)
      ||(entity->GetType() == NPCEntityType)
      ||(entity->GetType() == MountEntityType))
    {
      entity->AddListener(this);
    }

    NetworkAddEntity(entity);
  }

  return success;
}

void EntityManager::Remove(const Common::Entity::Entityp entity)
{
  if (!entity) return;

  // Stop listening for movement.
  entity->RemoveListener(this);

  Common::Entity::EntityManager::Remove(entity);
}

void EntityManager::NetworkAddEntity(const Common::Entity::Entityp entity)
{
  using namespace Common::Entity;

  // Notify other players of this entity. This includes sending self to yourself.
  // TODO: replace 100 with something configurable.
  std::list<Common::Entity::Entityp> result = Query(WFMath::Ball<3>(entity->GetPosition(), 100));
  std::list<Common::Entity::Entityp>::const_iterator it;
  for ( it=result.begin() ; it != result.end(); it++ )
  {
    if ((*it)->GetType() == PCEntityType)
    {
      PcEntity* e = dynamic_cast<PcEntity*>((*it).get());
      if (!e || !e->GetUser())
      {
        printf("E: Invalid PcEntity or no user set!!\n");
        return;
      }
      e->GetUser()->SendAddEntity(entity);
    }
  }

  // Send other entities to this player.
  if (entity->GetType() == PCEntityType)
  {
    PcEntity* e = dynamic_cast<PcEntity*>(entity.get());
    if (!e || !e->GetUser())
    {
      printf("E: Invalid PcEntity or no user set!!\n");
      return;
    }
    e->GetUser()->SendEntityDiff(result);
  }
}

void DontDelete(Common::Entity::Entity*){}

void EntityManager::Moved(WFMath::iShape* shape)
{
  Common::Entity::Entity* ent = dynamic_cast<Common::Entity::Entity*>(shape);
  if (ent) return;

  Common::Entity::Entityp entity(ent, DontDelete); //TODO: hack :/
  {
    LockType lock(cbMutex);
    std::list<Common::Entity::EntityCallback*>::iterator mit;
    for ( mit=callback_list.begin() ; mit != callback_list.end(); mit++ )
    {
      (*mit)->OnEntityMove(entity);
    }
  }

  using namespace Common::Entity;

  if (entity->GetType() == PCEntityType)
  {
    PcEntity* e = dynamic_cast<PcEntity*>(entity.get());
    if (!e || !e->GetUser())
    {
      printf("E: Invalid PcEntity or no user set!!\n");
      return;
    }

    // TODO: replace 100 with something configurable.
    std::list<Common::Entity::Entityp> result = Query(WFMath::Ball<3>(entity->GetPosition(), 100));
    e->GetUser()->SendEntityDiff(result);
  }
}

