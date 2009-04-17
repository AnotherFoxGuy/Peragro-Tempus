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
#include "attributes.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-attributetypes.h"
#include "src/server/database/table-attributes.h"


Attributes::Attributes(AttributesFactory* fact, Entity* entity, TableManager* db)
  : Bases<int>("Attributes", fact, entity, db)
{
}

void Attributes::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw PT_EX(InvalidEntity("Entity id NoEntity for attributes"));

  AttributesTable* table = db->Get<AttributesTable>();
  AttributesTableVOArray arr = table->Get(entity->GetId());
  AttributesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    size_t id = (*it)->AttributeTypes_id;
    bases[id] = boost::shared_ptr<Base> (new Base(id, (*it)->value));
    printf("Added entity attribute %s\n", fact->GetName(id).c_str());
  }
}

void Attributes::SaveToDB()
{
  AttributesTable* table = db->Get<AttributesTable>();
  std::map<size_t, boost::shared_ptr<Base> >::const_iterator it = bases.begin();
  for ( ; it != bases.end(); it++)
  {
    if (it->second->IsDirty())
    {
      table->Insert(entity->GetId(), it->first, it->second->Get());
    }
  }
}

AttributesFactory::AttributesFactory(TableManager* db) : BasesFactory(db)
{
  LoadFromDB();
}

void AttributesFactory::LoadFromDB()
{
  AttributeTypesTable* table = db->Get<AttributeTypesTable>();
  AttributeTypesTableVOArray arr = table->GetAll();
  AttributeTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Add((*it)->id, (*it)->name);
    printf("Added Attribute %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Attributes> AttributesFactory::Create(Entity* entity)
{
  boost::shared_ptr<Attributes> attributes(new Attributes(this, entity, db));
  return attributes;
}

