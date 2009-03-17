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

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager() 
{
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
    printf("NAME %d %s\n", entity->GetId(), entity->GetName().c_str());
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
      printf("E: Invalid EntityType %d\n", type);
      throw "Invalid EntityType!";
    }
  }
  if (entity)
  {
    entity->SetId(entityId);
  }

  return entity;
}

bool EntityManager::Add(Common::Entity::Entityp entity)
{
  lock();
  bool success = Common::Entity::EntityManager::Add(entity);
  unlock();

  if (success)
  {
    using namespace Common::Entity;
    if (entity->GetType() == PCEntityType 
      || entity->GetType() == NPCEntityType)
    {
      //Server::getServer()->getCollisionDetection()->addEntity(entity);
    }

    // It's a movable entity, so listen for it's movement.
    if ((entity->GetType() == PCEntityType)
      ||(entity->GetType() == NPCEntityType)
      ||(entity->GetType() == MountEntityType))
    {
      entity->GetShape()->AddListener(this);
    }
    // Notify other entities that this one entered the world.
    Moved(entity->GetShape());


    std::list<Common::Entity::EntityCallback*>::iterator it;
    for ( it=callback_list.begin() ; it != callback_list.end(); it++ )
    {
      (*it)->OnEntityAdd(entity);
    }
  }

  return success;
}

void EntityManager::Remove(const Common::Entity::Entityp entity)
{
  std::list<Common::Entity::EntityCallback*>::iterator it;
  for ( it=callback_list.begin() ; it != callback_list.end(); it++ )
  {
    (*it)->OnEntityRemove(entity);
  }

  // Stop listening for movement.
  entity->GetShape()->RemoveListener(this);

  lock();
  Common::Entity::EntityManager::Remove(entity);
  unlock();
}

void EntityManager::AddEntityCallback(Common::Entity::EntityCallback* cb)
{
  lock();
  callback_list.remove(cb);
  callback_list.push_back(cb);
  unlock();
}

void EntityManager::RemoveEntityCallback(Common::Entity::EntityCallback* cb)
{
  lock();
  callback_list.remove(cb);
  unlock();
}

void DontDelete(Common::Entity::Entity*){}

void EntityManager::Moved(Octree::Shape* shape)
{
  Common::Entity::Entityp entity(shape->GetParent(), DontDelete); //TODO: hack :/
  std::list<Common::Entity::EntityCallback*>::iterator mit;
  for ( mit=callback_list.begin() ; mit != callback_list.end(); mit++ )
  {
    (*mit)->OnEntityMove(entity);
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
    std::list<Common::Entity::Entityp> result = Queryp(WFMath::Ball<3>(entity->GetPosition(), 100));
    e->GetUser()->SendEntityDiff(result);
  }
}