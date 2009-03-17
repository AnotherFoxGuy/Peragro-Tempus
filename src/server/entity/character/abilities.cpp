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
#include "abilities.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-abilitytypes.h"
#include "src/server/database/table-abilities.h"


Abilities::Abilities(AbilitiesFactory* fact, Entity* entity, TableManager* db)
  : Bases<10, float, 10>("Abilities", fact, entity, db)
{
}

void Abilities::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw "Invalid entity!";

  AbilitiesTable* table = db->Get<AbilitiesTable>();
  AbilitiesTableVOArray arr = table->Get(entity->GetId());
  AbilitiesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    size_t id = (*it)->AbilityTypes_id;
    bases[id] = boost::shared_ptr<Base> (new Base(id, (*it)->xp));
    printf("Added entity ability %s\n", fact->GetName(id).c_str());
  }
}

void Abilities::SaveToDB()
{
  AbilitiesTable* table = db->Get<AbilitiesTable>();
  std::map<size_t, boost::shared_ptr<Base> >::const_iterator it = bases.begin();
  for ( ; it != bases.end(); it++)
  {
    if (it->second->IsDirty())
    {
      table->Insert(entity->GetId(), it->first, it->second->Get());
    }
  }
}

AbilitiesFactory::AbilitiesFactory(TableManager* db) : BasesFactory(db) 
{
  LoadFromDB();
}

void AbilitiesFactory::LoadFromDB()
{
  AbilityTypesTable* table = db->Get<AbilityTypesTable>();
  AbilityTypesTableVOArray arr = table->GetAll();
  AbilityTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Add((*it)->id, (*it)->name);
    printf("Added Ability %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Abilities> AbilitiesFactory::Create(Entity* entity)
{
  boost::shared_ptr<Abilities> abilities(new Abilities(this, entity, db));
  return abilities;
}