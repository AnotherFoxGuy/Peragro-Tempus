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

#include "playerinventory.h"

#include "common/eventcs/eventmanager.h"
#include "common/eventcs/tradeevent.h"
#include "common/eventcs/entityevent.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/gui/guimanager.h"
#include "client/gui/inventory-gui.h"

using namespace Common::Inventory;

namespace PT
{
  namespace Trade
  {
    Item::Item(iEvent& ev)
    {
      using namespace PT::Events;
      entityId = Helper::GetUInt(&ev, "itemEntityId");
      name = Helper::GetString(&ev, "name");
      fileName = Helper::GetString(&ev, "fileName");
      meshFactName = Helper::GetString(&ev, "meshFactName");
      objectSize = PT::Events::EntityHelper::GetSize(&ev, "size");
      objectDescription = Helper::GetString(&ev, "description");
      weight = Helper::GetFloat(&ev, "weight");
      equipType = Helper::GetString(&ev, "equipType");

      //slotId
    }

    PlayerInventory::PlayerInventory ()
    {
      inventory = boost::shared_ptr<GridInventory>(new GridInventory("Inventory", ALLOW_ITEMS, 5, 4));
    }

    PlayerInventory::~PlayerInventory ()
    {
    }

    bool PlayerInventory::Initialize ()
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;
      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return false;
      InventoryWindow* inventoryWindow = guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
      if (!inventoryWindow) return false;

      if (!inventoryWindow->SetInventory(inventory)) return false;

      // Register listener for InventoryAddEvent.
      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(PlayerInventory, PickUp, "trade.inventory.pickup")
      PT_REGISTER_LISTENER(PlayerInventory, Drop, "trade.inventory.drop")
      //PT_REGISTER_LISTENER(PlayerInventory, Add, "trade.inventory.add")
      PT_REGISTER_LISTENER(PlayerInventory, List, "trade.inventory.list")
      PT_REGISTER_LISTENER(PlayerInventory, Move, "trade.inventory.move")

      return true;
    } // end Initialize()

    bool PlayerInventory::PickUp(iEvent& ev)
    {
      using namespace PT::Events;

      unsigned int itemId = -1;
      ev.Retrieve("itemEntityId", itemId);

      if (!Helper::HasError(&ev))
      {
        Add(ev);
      }
      else
        Report(PT::Notify, "You can't pick Item %d! Reason: '%s'.", itemId, Helper::GetError(&ev).c_str());

      return true;
    } // end PickUp()

    bool PlayerInventory::Drop(iEvent& ev)
    {
      using namespace PT::Events;

      Common::Inventory::PositionRef pos;
      pos = PT::Events::EntityHelper::GetPositionRef(&ev, "position");

      if (!Helper::HasError(&ev))
      {
        if (!inventory->RemoveObjectAt(pos)) Report(PT::Error, "Inventory error: Drop!");
      }
      else
        Report(PT::Notify, "You can't drop! Reason: '%s'.", Helper::GetError(&ev).c_str());

      return true;
    } // end Drop()

    bool PlayerInventory::List(iEvent& ev)
    {
      using namespace Events;

      csRef<iEvent> list;
      if (ev.Retrieve("inventoryList", list) == csEventErrNone)
      {
        csRef<iEventAttributeIterator> items = list->GetAttributeIterator();
        while (items->HasNext())
        {
          csRef<iEvent> item; list->Retrieve(items->Next(), item);
          Add(*item);
        } // end while
      }
      else
        Report(PT::Error, "PlayerInventory failed to get inventory list!");

      return true;
    } // end List()

    bool PlayerInventory::Add(iEvent& ev)
    {
      using namespace PT::Events;

      Common::Inventory::PositionRef pos;
      pos = PT::Events::EntityHelper::GetPositionRef(&ev, "position");

      boost::shared_ptr<Item> item(new Item(ev));

      Report(PT::Error, "PlayerInventory::Add %d,%d!", pos.column, pos.row);

      if (!inventory->AddObjectAt(pos, item)) Report(PT::Error, "Inventory error: Add!");

      return true;
    } // end Add()

    bool PlayerInventory::Move(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      Common::Inventory::PositionRef oldSlotId = PT::Events::EntityHelper::GetPositionRef(&ev, "oldSlot");
      Common::Inventory::PositionRef newSlotId = PT::Events::EntityHelper::GetPositionRef(&ev, "newSlot");

      if (!Helper::HasError(&ev))
      {
        if (!inventory->MoveObject(oldSlotId, newSlotId)) Report(PT::Error, "Inventory error: Move!");
      }
      else
        Report(PT::Notify, "You can't move object from %d,%d to %d,%d! Reason: '%s'.",
          oldSlotId.column, oldSlotId.row, newSlotId.column, newSlotId.row, Helper::GetError(&ev).c_str());

      return true;
    } // end Move()

  } // Trade namespace
} // PT namespace
