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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"

#include "client/gui/guimanager.h"
#include "client/gui/gui.h"

namespace PT
{
  namespace Trade
  {
    TradeManager::TradeManager ()
    {
    }

    TradeManager::~TradeManager ()
    {
    }

    bool TradeManager::Initialize ()
    {
      using namespace PT::Events;

      // Register listener for TradePickEvent.
      EventHandler<TradeManager>* cbPickUp = new EventHandler<TradeManager>(&TradeManager::PickUp, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("trade.pickup", cbPickUp);

      // Register listener for TradePickEvent.
      EventHandler<TradeManager>* cbDrop = new EventHandler<TradeManager>(&TradeManager::Drop, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("trade.drop", cbDrop);

      return true;
    }

    bool TradeManager::PickUp(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      TradePickUpEvent* pickUpEv = GetTradeEvent<TradePickUpEvent*>(ev);
      if (!pickUpEv) return false;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;
      guimanager->GetInventoryWindow()->AddItem(pickUpEv->itemId, pickUpEv->variationId, pickUpEv->slotId);

      return true;
    }

    bool TradeManager::Drop(PT::Events::Eventp ev)
    {
      using namespace PT::Events;

      TradeDropEvent* dropEv = GetTradeEvent<TradeDropEvent*>(ev);
      if (!dropEv) return false;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;
      guimanager->GetInventoryWindow()->RemoveItem(dropEv->slotId);

      return true;
    }

  } // Trade namespace 
} // PT namespace 
