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
  return character->GetEquipment()->Equiped("Right Hand");
}

unsigned int WeaponHelper::GetReach(boost::shared_ptr<ItemEntity> item)
{
  // TODO This should not be here, just till the DB is setup
  // to correctly set Reach to something...
  return 5;   
}

unsigned int WeaponHelper::GetHeft(boost::shared_ptr<ItemEntity> item)
{
  // TODO This should not be here, just till the DB is setup
  // to correctly set Heft to something...
  return 5;   
}

unsigned int WeaponHelper::GetBaseDamage(boost::shared_ptr<ItemEntity> item)
{  
  return 1; //TODO
}

unsigned int WeaponHelper::GetDamage(boost::shared_ptr<ItemEntity> item)
{  
  return GetBaseDamage(item); //TODO * item->GetQuality()/item->GetMaxQuality();
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

  //TODO: What when the item doesn't have any types?
  if (types.size() == 0)
    types.push_back(Type("Blunt", 1));

  return types;
}