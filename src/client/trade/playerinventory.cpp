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

#include "client/event/eventmanager.h"
#include "client/event/tradeevent.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/gui/guimanager.h"
#include "client/gui/gui.h"


namespace PT
{
  namespace Trade
  {
    PlayerInventory::PlayerInventory ()
    {
    }

    PlayerInventory::~PlayerInventory ()
    {
    }

    bool PlayerInventory::Initialize ()
    {
      using namespace PT::Events;

      // Register listener for InventoryAddEvent.
      EventHandler<PlayerInventory>* cbAdd = new EventHandler<PlayerInventory>(&PlayerInventory::Add, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("trade.inventory.add", cbAdd);

      return true;
    }

    bool PlayerInventory::Add(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      unsigned int itemId = -1;
      ev.Retrieve("itemId", itemId);

      unsigned int variationId = -1;
      ev.Retrieve("variationId", variationId);

      unsigned int slotId = -1;
      ev.Retrieve("slotId", slotId);

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;
        InventoryWindow* inventoryWindow = guimanager->GetWindow<InventoryWindow>(INVENTORYWINDOW);
      inventoryWindow->AddItem(itemId, variationId, slotId);

      return true;
    }

  } // Trade namespace
} // PT namespace
