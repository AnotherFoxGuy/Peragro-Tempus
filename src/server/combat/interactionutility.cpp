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

#include "server/entity/statmanager.h"
#include "interactionutility.h"

float InteractionUtility::GetStatValue(Character* lockedCharacter,
                                       const char* statName)
{
  Server* server = Server::getServer();

  const Stat* stat = server->getStatManager()->findByName(ptString(statName,
                                                          strlen(statName)));

  if (!stat) {
    server->getStatManager()->dumpAllStatNames();
    printf("BUG: Unalbe to find stat: %s\n", statName);
    return 0.0f;
  }
  return static_cast<float>(lockedCharacter->getStats()->getAmount(stat));
}

static float
InteractionUtility::GetStatValueForAllEquipedItems(Character* lockedCharacter,
                                                   const char* statName)
{
  float value = 0.0f;
  Inventory* inventory = lockedCharacter->getInventory();
  Item* item;

  if (!inventory)
  {
    return 0.0f;
  }

  for (unsigned char slot = 0; slot < inventory->NoSlot; slot++)
  {
    item = GetItem(lockedCharacter, slot);
    if (!item)
    {
      continue;
    }
    value += GetStatValueForItem(item, statName);
  }

  return value;
}
