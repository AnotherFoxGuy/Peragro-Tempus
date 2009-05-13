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
#include "vulnerabilities.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-vulnerabilitytypes.h"
#include "src/server/database/table-vulnerabilities.h"

Vulnerabilities::Vulnerabilities(VulnerabilitiesFactory* fact, Entity* entity, TableManager* db)
  : Bases<int>("Vulnerabilities", fact, entity, db)
{
}

void Vulnerabilities::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw PT_EX(InvalidEntity("Invalid entity!"));

  VulnerabilitiesTable* table = db->Get<VulnerabilitiesTable>();
  VulnerabilitiesTableVOArray arr = table->Get(entity->GetId());
  VulnerabilitiesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    size_t id = (*it)->VulnerabilityTypes_id;
    bases[id] = boost::shared_ptr<Base> (new Base(id, (*it)->value));
    printf("Added entity vulnerability %s\n", fact->GetName(id).c_str());
  }
}

void Vulnerabilities::SaveToDB()
{
  VulnerabilitiesTable* table = db->Get<VulnerabilitiesTable>();
  std::map<size_t, boost::shared_ptr<Base> >::const_iterator it = bases.begin();
  for ( ; it != bases.end(); it++)
  {
    if (it->second->IsDirty())
    {
      table->Insert(entity->GetId(), it->first, it->second->Get());
    }
  }
}

VulnerabilitiesFactory::VulnerabilitiesFactory(TableManager* db) : BasesFactory(db)
{
  LoadFromDB();
}

void VulnerabilitiesFactory::LoadFromDB()
{
  VulnerabilityTypesTable* table = db->Get<VulnerabilityTypesTable>();
  VulnerabilityTypesTableVOArray arr = table->GetAll();
  VulnerabilityTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Add((*it)->id, (*it)->name);
    printf("Added Ability %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Vulnerabilities> VulnerabilitiesFactory::Create(Entity* entity)
{
  boost::shared_ptr<Vulnerabilities> vul(new Vulnerabilities(this, entity, db));
  return vul;
}

