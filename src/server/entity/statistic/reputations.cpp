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

#include "../entity.h"
#include "reputations.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-reputationtypes.h"
#include "src/server/database/table-reputations.h"


Reputations::Reputations(ReputationsFactory* fact, Entity* entity, TableManager* db)
  : Bases<size_t>("Reputations", fact, entity, db)
{
}

void Reputations::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw "Invalid entity!";

  ReputationsTable* table = db->Get<ReputationsTable>();
  ReputationsTableVOArray arr = table->Get(entity->GetId());
  ReputationsTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    size_t id = (*it)->ReputationTypes_id;
    bases[id] = boost::shared_ptr<Base> (new Base(id, (*it)->level));
    printf("Added entity reputation %s\n", fact->GetName(id).c_str());
  }
}

void Reputations::SaveToDB()
{
  ReputationsTable* table = db->Get<ReputationsTable>();
  std::map<size_t, boost::shared_ptr<Base> >::const_iterator it = bases.begin();
  for ( ; it != bases.end(); it++)
  {
    if (it->second->IsDirty())
    {
      table->Insert(entity->GetId(), it->first, it->second->Get());
    }
  }
}

ReputationsFactory::ReputationsFactory(TableManager* db) : BasesFactory(db)
{
  LoadFromDB();
}

void ReputationsFactory::LoadFromDB()
{
  ReputationTypesTable* table = db->Get<ReputationTypesTable>();
  ReputationTypesTableVOArray arr = table->GetAll();
  ReputationTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Add((*it)->id, (*it)->name);
    printf("Added Reputation %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Reputations> ReputationsFactory::Create(Entity* entity)
{
  boost::shared_ptr<Reputations> reputations(new Reputations(this, entity, db));
  return reputations;
}

