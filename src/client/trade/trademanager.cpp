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

#include "trademanager.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "common/event/eventmanager.h"

#include "client/gui/guimanager.h"
#include "client/gui/gui.h"

#include "client/trade/playerinventory.h"

namespace PT
{
  namespace Trade
  {
    TradeManager::TradeManager()
      : playerinventory(0)
    {
    }

    TradeManager::~TradeManager()
    {
      delete playerinventory;
    }

    bool TradeManager::Initialize()
    {
      SETUP_HANDLER
      REGISTER_LISTENER(TradeManager, PickUp, "trade.pickup")
      REGISTER_LISTENER(TradeManager, Drop, "trade.drop")

      // Create and Initialize the PlayerInventory.
      delete playerinventory;
      playerinventory = new PT::Trade::PlayerInventory ();
      if (!playerinventory->Initialize())
        return Report(PT::Error, "Failed to initialize PlayerInventory!");

      return true;
    }

    bool TradeManager::PickUp(iEvent& ev)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      unsigned int itemId = -1;
      ev.Retrieve("itemId", itemId);

      unsigned int variationId = -1;
      ev.Retrieve("variationId", variationId);

      const char* name = "";
      ev.Retrieve("name", name);

      const char* icon = "";
      ev.Retrieve("icon", icon);

      unsigned int slotId = -1;
      ev.Retrieve("slotId", slotId);

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;
      InventoryWindow* inventoryWindow = guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
      inventoryWindow->AddItem(itemId, variationId, name, icon, slotId);

      return true;
    }

    bool TradeManager::Drop(iEvent& ev)
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      unsigned int slotId = -1;
      ev.Retrieve("slotId", slotId);

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;
      InventoryWindow* inventoryWindow = guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
      inventoryWindow->RemoveItem(slotId);

      return true;
    }

  } // Trade namespace
} // PT namespace
