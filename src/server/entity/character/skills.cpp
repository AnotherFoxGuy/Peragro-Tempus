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
#include "skills.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-skilltypes.h"
#include "src/server/database/table-skills.h"


Skills::Skills(SkillsFactory* fact, Entity* entity, TableManager* db)
  : Bases<3, float>("Skills", fact, entity, db)
{
}

void Skills::LoadFromDB()
{
  if(entity->GetId() == Common::Entity::Entity::NoEntity)
    throw "Invalid entity!";

  SkillsTable* table = db->Get<SkillsTable>();
  SkillsTableVOArray arr = table->Get(entity->GetId());
  SkillsTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    size_t id = (*it)->SkillTypes_id;
    bases[id] = boost::shared_ptr<Base> (new Base(id, (*it)->xp));
    printf("Added entity skill %s\n", fact->GetName(id).c_str());
  }
}

void Skills::SaveToDB()
{
  SkillsTable* table = db->Get<SkillsTable>();
  std::map<size_t, boost::shared_ptr<Base> >::const_iterator it = bases.begin();
  for ( ; it != bases.end(); it++)
  {
    if (it->second->IsDirty())
    {
      table->Insert(entity->GetId(), it->first, it->second->Get());
    }
  }
}

SkillsFactory::SkillsFactory(TableManager* db) : BasesFactory(db)
{
  LoadFromDB();
}

void SkillsFactory::LoadFromDB()
{
  SkillTypesTable* table = db->Get<SkillTypesTable>();
  SkillTypesTableVOArray arr = table->GetAll();
  SkillTypesTableVOArray::const_iterator it = arr.begin();
  for ( ; it != arr.end(); it++)
  {
    Add((*it)->id, (*it)->name);
    printf("Added Skill %s\n", (*it)->name.c_str());
  }
}

boost::shared_ptr<Skills> SkillsFactory::Create(Entity* entity)
{
  boost::shared_ptr<Skills> skills(new Skills(this, entity, db));
  return skills;
}