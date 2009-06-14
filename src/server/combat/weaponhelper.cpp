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

#include "weaponhelper.h"

#include "server/entity/character/character.h"
#include "server/entity/itementity.h"

#include "server/server.h"

boost::shared_ptr<ItemEntity> WeaponHelper::GetWeapon(boost::shared_ptr<Character> character)
{
  return character->GetEquipment()->Equipped("Right Hand");
}

bool WeaponHelper::IsHeavy(boost::shared_ptr<Character> character, boost::shared_ptr<ItemEntity> item)
{
  // Weight is in grams, need kilograms, divide by 1000.
  return item->GetAttributes()->Get("Weight")/1000 > character->GetAbilities()->Get("Strength");
}

unsigned int WeaponHelper::GetReach(boost::shared_ptr<ItemEntity> item)
{
  return item->GetAttributes()->Get("Reach");
}

unsigned int WeaponHelper::GetHeft(boost::shared_ptr<ItemEntity> item)
{
  return item->GetAttributes()->Get("Heft");
}

unsigned int WeaponHelper::GetBaseDamage(boost::shared_ptr<ItemEntity> item)
{
  return item->GetAttributes()->Get("Damage");
}

unsigned int WeaponHelper::GetDamage(boost::shared_ptr<ItemEntity> item)
{
  return GetBaseDamage(item) * ((float)item->GetAttributes()->Get("Quality")/100.0f);
}

WeaponHelper::Types WeaponHelper::GetTypes(boost::shared_ptr<ItemEntity> item)
{
  Types types;

  if (!item) return types;

  boost::shared_ptr<Vulnerabilities> vuls = item->GetVulnerabilities();

  VulnerabilitiesFactory* fact = Server::getServer()->GetVulnerabilitiesFactory();
  const std::map<std::string, size_t>& names = fact->Get();

  std::map<std::string, size_t>::const_iterator it;
  for (it = names.begin(); it != names.end(); it++)
  {
    if (vuls->Get(it->first)) // Only add if value != 0
      types.push_back( Type(it->first, vuls->Get(it->first)) );
  }

  //If the item doesn't have any types, treat as blunt.
  if (types.size() == 0)
    types.push_back(Type("Blunt", 1));

  return types;
}

