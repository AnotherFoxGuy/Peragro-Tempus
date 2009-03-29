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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#include "client/network/network.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/pointer/pointer.h"

void PlayerHandler::handleInventoryList(GenericMessage* msg)
{
  InventoryListMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("trade.inventory.list", true);
    pEvent->Add("inventoryId", pmsg.getInventoryId());
    csRef<iEvent> list = evmgr->CreateEvent("inventoryList", true);
    for (unsigned char i = 0; i < pmsg.getInventoryCount(); i++)
    {
      std::stringstream itemName;
      itemName << "inventory" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("itemId", pmsg.getItemId(i));
      item->Add("slotId", pmsg.getSlotId(i));
      item->Add("name", *pmsg.getName(i)?*pmsg.getName(i):"");
      item->Add("iconName", *pmsg.getIconName(i)?*pmsg.getIconName(i):"");
      item->Add("description", *pmsg.getDescription(i)?*pmsg.getDescription(i):"");
      item->Add("weight", pmsg.getWeight(i));
      item->Add("equipType", *pmsg.getEquipType(i)?*pmsg.getEquipType(i):"");
      list->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("inventoryList", list);

    evmgr->AddEvent(pEvent);
  }

} // end handleInventoryList

void PlayerHandler::handleInventoryMoveItem(GenericMessage* msg)
{
  InventoryMoveItemMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("trade.inventory.move", true);
    pEvent->Add("oldSlot", pmsg.getOldSlot());
    pEvent->Add("oldInventoryId", pmsg.getOldInventoryId());
    pEvent->Add("newSlot", pmsg.getNewSlot());
    pEvent->Add("newInventoryId", pmsg.getNewInventoryId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleInventoryMoveItem

