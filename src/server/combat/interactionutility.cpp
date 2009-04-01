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

#include "interactionutility.h"

#include "server/server.h"

unsigned int InteractionUtility::GetStatValueForItem(boost::shared_ptr<ItemEntity> item,
                                                     const std::string& statName)
{
  /*
  Server *server = Server::getServer();
  const Stat* stat = server->getStatManager()->
    findByName(ptString(statName, strlen(statName)));
  if (!stat)
  {
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unable to find stat: %s\n", statName);
    return 0;
  }
  */
  //return item->getStats()->getAmount(stat);
  return 0;
}

unsigned int InteractionUtility::GetStatValue(boost::shared_ptr<Character> character,
                                              const std::string& statName)
{
  /*
  Server* server = Server::getServer();

  const Stat* stat = server->getStatManager()->findByName(ptString(statName,
                                                          strlen(statName)));

  if (!stat) {
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unable to find stat: %s\n", statName);
    return 0;
  }
  return character->getStats()->getAmount(stat);
  */
  return 0;
}

unsigned int InteractionUtility::GetStatValueForAllEquipedItems(boost::shared_ptr<Character> character,
                                                   const std::string& statName)
{
  unsigned int value = 0;
  boost::shared_ptr<Inventory> inventory = character->GetInventory();
  boost::shared_ptr<ItemEntity> item;

  if (!inventory)
  {
    return 0;
  }
/*
  for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
  {
    item = GetItem(character, slot);
    if (!item)
    {
      continue;
    }
    value += GetStatValueForItem(item, statName);
  }
*/
  return value;
}

boost::shared_ptr<ItemEntity> InteractionUtility::GetItem(boost::shared_ptr<Character> character,
                                                          unsigned char slot)
{
  /*
  Server *server = Server::getServer();
  Inventory* inventory = character->getInventory();

  if (!inventory)
  {
    return NULL;
  }

  if (slot >= inventory->NoSlot)
  {
    return NULL;
  }

  const InventoryEntry* entry = inventory->getItem(slot);
  if (!entry || entry->id == 0)
  {
    return NULL;
  }
  Item* item = server->getItemManager()->findById(entry->id);
  return item;
  */
  return boost::shared_ptr<ItemEntity>();
}

void InteractionUtility::IncreaseStatValue(boost::shared_ptr<Character> character,
                                      const std::string& statName,
                                      unsigned int increase)
{
  Server* server = Server::getServer();
  unsigned int statValue = increase;
/*
  const Stat* stat = server->getStatManager()->findByName(ptString(statName,
                                                          strlen(statName)));

  if (!stat) {
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unalbe to find stat: %s\n", statName);
  }
  statValue += character->getStats()->getAmount(stat);

  character->getStats()->setStat(stat, statValue);
  */
}

std::string InteractionUtility::GetXPString(const std::string& name)
{
  std::string str = name + "XP";
  return str;
}
