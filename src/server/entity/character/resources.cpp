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


#include "resources.h"

#include "common/network/serialiser.h"
#include "common/network/playermessages.h"
#include "server/network/connection.h"

#include "../entity.h"

#include "server/server.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-resourcetypes.h"
#include "src/server/database/table-resources.h"

#include "common/util/printhelper.h"

#include "server/entity/entitymanager.h"

Resources::Resources(ResourcesFactory* fact, Entity* entity, TableManager* db)
  : fact(fact), entity(entity),  db(db)
{
}

void Resources::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw "Invalid entity!";

  ResourcesTable* table = db->Get<ResourcesTable>();
  ResourcesTableVOArray arr = table->Get(entity->GetId());
  ResourcesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    //Add((*it)->resourceType_id, (*it)->value);
  }
}

void Resources::SaveResourceToDB(size_t resourceId)
{
  ResourcesTable* table = Server::getServer()->GetTableManager()->Get<ResourcesTable>();
  //table->Insert(entity->GetId(), resourceId, Get(resourceId));
}

void Resources::SaveToDB()
{
  //Already taken care of.
}

ResourcesFactory::ResourcesFactory(TableManager* db) : db(db)
{
  LoadFromDB();
}

void ResourcesFactory::LoadFromDB()
{
  ResourceTypesTable* table = db->Get<ResourceTypesTable>();
  ResourceTypesTableVOArray arr = table->GetAll();
  ResourceTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    //Add((*it)->id, (*it)->name);
    printf("Added Resources %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Resources> ResourcesFactory::Create(Entity* entity)
{
  boost::shared_ptr<Resources> resources(new Resources(this, entity, db));
  return resources;
}
