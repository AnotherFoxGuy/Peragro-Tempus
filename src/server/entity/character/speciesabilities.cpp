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

#include "character.h"
#include "speciesabilities.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-speciesabilities.h"

//#define lerp(t, a, b) ( a + t * (b - a) )
#define lerp(a, b, x) ( a + x * (b - a) )


SpeciesAbilities::SpeciesAbilities(CharacterEntity* entity)
{
  abilities = Server::getServer()->GetAbilitiesFactory()->Create(entity);
}

//TODO: move to speciesmgr instead
float SpeciesAbilities::GetAbilityXP(size_t speciesId, float age, const std::string& name)
{
  size_t abilityId = Server::getServer()->GetAbilitiesFactory()->GetID(name);

  SpeciesTable* table = Server::getServer()->GetTableManager()->Get<SpeciesAbilitiesTable>();
  SpeciesTableVOp s = table->GetSingle(speciesId);
  if (!s)
  {
    printf("E: Invalid speciesId %"SIZET"!\n", speciesId);
    throw "Invalid speciesId !";
  }

  SpeciesAbilitiesTable* tablesa = Server::getServer()->GetTableManager()->Get<SpeciesAbilitiesTable>();
  SpeciesAbilitiesTableVOp sa = tablesa->GetSingle(speciesId, abilityId);
  if (!sa) return 0.0f;

  return lerp(sa->minXP, sa->maxXP, age/s->maxAge);
}

float SpeciesAbilities::Get(const std::string& name)
{
  return GetAbilityXP(entity->GetSpecies(), entity->GetAge(), name) + abilities->Get(name);
}

void SpeciesAbilities::Set(const std::string& name, float xp)
{
  abilities->Set(name, xp);
}

void SpeciesAbilities::Add(const std::string& name, float xp)
{
  abilities->Add(name, xp);
}

void SpeciesAbilities::Sub(const std::string& name, float xp)
{
  abilities->Sub(name, xp);
}

size_t SpeciesAbilities::GetLevel(const std::string& name)
{
  return abilities->GetLevel(this->Get());
}

void SpeciesAbilities::LoadFromDB()
{
  abilities->LoadFromDB();
}

void SpeciesAbilities::SaveToDB()
{
  abilities->SaveToDB();
}