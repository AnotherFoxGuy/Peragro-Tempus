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
#include "speciesvulnerabilities.h"

#include "src/server/database/tablemanager.h"
#include "src/server/database/table-species.h"
#include "src/server/database/table-speciesvulnerabilities.h"

#include "common/util/printhelper.h"

//#define lerp(t, a, b) ( a + t * (b - a) )
#define lerp(a, b, x) ( a + x * (b - a) )


SpeciesVulnerabilities::SpeciesVulnerabilities(Character* entity) : entity(entity)
{
  vulnerabilities = Server::getServer()->GetVulnerabilitiesFactory()->Create(entity);
}

//TODO: move to speciesmgr instead
float SpeciesVulnerabilities::GetVulnerabilityValue(size_t speciesId, float age, const std::string& name)
{
  size_t Id = Server::getServer()->GetVulnerabilitiesFactory()->GetID(name);

  SpeciesTable* table = Server::getServer()->GetTableManager()->Get<SpeciesTable>();
  SpeciesTableVOp s = table->GetSingle(speciesId);
  if (!s)
  {
    throw PT_EX(InvalidSpeciesAbilities("Invalid species id"))
      << SpeciesIdInfo(speciesId);
  }

  SpeciesVulnerabilitiesTable* tablesa = Server::getServer()->GetTableManager()->Get<SpeciesVulnerabilitiesTable>();
  SpeciesVulnerabilitiesTableVOp sa = tablesa->GetSingle(speciesId, Id);
  if (!sa) return 100.0f; // Default.

  return lerp(sa->minValue, sa->maxValue, age/s->maxAge);
}

float SpeciesVulnerabilities::Get(const std::string& name)
{
  return GetVulnerabilityValue(entity->GetSpecies(), entity->GetAge(), name) + vulnerabilities->Get(name);
}

void SpeciesVulnerabilities::Set(const std::string& name, float xp)
{
  vulnerabilities->Set(name, xp);
}

void SpeciesVulnerabilities::Add(const std::string& name, float xp)
{
  vulnerabilities->Add(name, xp);
}

void SpeciesVulnerabilities::Sub(const std::string& name, float xp)
{
  vulnerabilities->Sub(name, xp);
}

void SpeciesVulnerabilities::LoadFromDB()
{
  vulnerabilities->LoadFromDB();
}

void SpeciesVulnerabilities::SaveToDB()
{
  vulnerabilities->SaveToDB();
}
