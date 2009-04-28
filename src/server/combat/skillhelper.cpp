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

#include "skillhelper.h"

#include "server/server.h"

unsigned int SkillHelper::GetSkillLevel(boost::shared_ptr<Character> character,
                                              const std::string& skillName)
{
  /*
  If you wanted to add bonus/buffs values
  this would be the place to do it.
  */
  return character->GetSkills()->GetLevel(skillName);
}

unsigned int SkillHelper::GetSkillLevelForAllEquipedItems(boost::shared_ptr<Character> character,
                                                                  const std::string& skillName)
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
      value += GetSkillLevelForItem(*it, skillName);
  }

  return value;
}

unsigned int SkillHelper::GetSkillLevelForItem(boost::shared_ptr<ItemEntity> item,
                                                        const std::string& skillName)
{
  //TODO
  //return item->GetSkills()->GetLevel(skillName);
  return 0;
}