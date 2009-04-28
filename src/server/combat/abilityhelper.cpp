/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "abilityhelper.h"

#include "server/server.h"

unsigned int AbilityHelper::GetAbilityLevel(boost::shared_ptr<Character> character,
                                              const std::string& abilityName)
{
  /*
  If you wanted to add bonus/buffs values
  this would be the place to do it.
  */
  return character->GetAbilities()->GetLevel(abilityName);
}

unsigned int AbilityHelper::GetAbilityLevelForAllEquipedItems(boost::shared_ptr<Character> character,
                                                                  const std::string& abilityName)
{
  unsigned int value = 0;
  boost::shared_ptr<Equipment> equipment = character->GetEquipment();

  if (!equipment)
  {
    return 0;
  }

  const std::list<boost::shared_ptr<ItemEntity> > items; //TODO = equipment->GetAllItems();
  std::list<boost::shared_ptr<ItemEntity> >::const_iterator it;
  for (it = items.begin(); it != items.end(); it++)
  {
    if (*it)
      value += GetAbilityLevelForItem(*it, abilityName);
  }

  return value;
}

unsigned int AbilityHelper::GetAbilityLevelForItem(boost::shared_ptr<ItemEntity> item,
                                                        const std::string& abilityName)
{
  return item->GetAbilities()->GetLevel(abilityName);
}