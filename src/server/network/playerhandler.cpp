/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "network.h"
#include "networkhelper.h"

#include "server/entity/character.h"
#include "server/entity/entity.h"
#include "server/entity/itemmanager.h"

#include "server/spawner.h"

void PlayerHandler::handleInventoryMoveItemRequest(GenericMessage* msg)
{
  const Entity* user_ent = NetworkHelper::getEntity(msg);
  if (!user_ent) return;

  const Character* c_char = NetworkHelper::getCharacter(msg);
  if (!c_char) return;

  const char* error = 0;

  ptString name = user_ent->getName();

  InventoryMoveItemRequestMessage request_msg;
  request_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  unsigned char equip_slot = request_msg.getNewSlot();
  unsigned char invent_slot = request_msg.getOldSlot();

  printf("Received EquipRequest from: '%s' | '%d' to '%d' \n", *name, invent_slot, equip_slot);

  bool equip = false;
  if (equip_slot < 10 || invent_slot < 10)
  {
    equip = true;
  }

  if (equip_slot >= 30 || invent_slot >= 30)
  {
    error = "Invalid slot";
  }

  ptScopedMonitorable<Character> character (c_char);
  Inventory* inventory = character->getInventory();

  const InventoryEntry new_item = *inventory->getItem(invent_slot);
  Item* item = server->getItemManager()->findById(new_item.id);

  if (!item)
    error = "No such Item";

  // See if we have already an item in the equip slot.
  const InventoryEntry old_item = *inventory->getItem(equip_slot);
  Item* old = server->getItemManager()->findById(old_item.id);

  if (item && ! error)
  {
    // Take from the inventory slot and...
    inventory->takeItem(invent_slot);

    // ... (if we have) from the equip slot too.
    if (old) inventory->takeItem(equip_slot);

    // Then we add the new item to the equip slot and...
    inventory->addItem(new_item, equip_slot);

    // ... (if we have) the old item to the inventory.
    if (old) inventory->addItem(old_item, invent_slot);
  }

  InventoryMoveItemMessage response_msg;

  if (error)
  {
    response_msg.setError(ptString(error, strlen(error)));
  }

  response_msg.setOldSlot(invent_slot);
  response_msg.setNewSlot(equip_slot);

  ByteStream bs;
  response_msg.serialise(&bs);
  NetworkHelper::sendMessage(user_ent, bs);

  if (equip && !error)
  {
    if (invent_slot < 10)
    {
      EquipMessage response_msg;
      response_msg.setEntityId(user_ent->getId());
      response_msg.setItemId(old_item.id);
      response_msg.setSlotId(invent_slot);
      response_msg.setFileName(old?old->getFile():ptString::Null);
      response_msg.setMeshName(old?old->getMesh():ptString::Null);

      ByteStream bs;
      response_msg.serialise(&bs);
      Server::getServer()->broadCast(bs);
    }

    if (equip_slot < 10)
    {
      EquipMessage response_msg;
      response_msg.setEntityId(user_ent->getId());
      response_msg.setItemId(new_item.id);
      response_msg.setSlotId(equip_slot);
      response_msg.setFileName(item->getFile());
      response_msg.setMeshName(item->getMesh());

      ByteStream bs;
      response_msg.serialise(&bs);
      Server::getServer()->broadCast(bs);
    }
  }
}
