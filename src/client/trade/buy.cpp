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

#include "buy.h"

#include "common/event/eventmanager.h"
#include "common/event/tradeevent.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/gui/guimanager.h"
#include "client/gui/gui.h"


namespace PT
{
  namespace Trade
  {
    Buy::Buy () : guimanager(0), buyWindow(0)
    {
    }

    Buy::~Buy ()
    {
    }

    bool Buy::Initialize ()
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;
      guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;

      buyWindow = guimanager->GetWindow<BuyWindow>(BUYWINDOW);
      if (!buyWindow) return true;

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(Buy, TradeOffersList, "trade.general.offer.list.1") // 1 == buy

        return true;
    } // end Initialize()

    void Buy::SetTrading(bool value)
    {
      using namespace PT::Events;

      //trade.general.confirm/cancel is used for buy/sell aswell, so only listen to it when neccessary!
      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      if (value)
      {
        tradeConfirm.AttachNew(new EventHandler<Buy>(&Buy::TradeConfirmResponse, this));
        evmgr->AddListener("trade.general.confirm", tradeConfirm);

        tradeCancel.AttachNew(new EventHandler<Buy>(&Buy::TradeCancel, this));
        evmgr->AddListener("trade.general.cancel", tradeCancel);
      }
      else
      {
        evmgr->RemoveListener(tradeConfirm);
        tradeConfirm.Invalidate();

        evmgr->RemoveListener(tradeCancel);
        tradeCancel.Invalidate();
      }
    } // end SetTrading()

    bool Buy::TradeConfirmResponse(iEvent& ev)
    {
      using namespace PT::GUI::Windows;
      using namespace PT::Events;

      if (!Helper::HasError(&ev))
      {
        SetTrading(false); // No errors, exchange has finished.

        if (buyWindow->IsVisible())
        {
          buyWindow->AcceptTrade();
        }
      }
      else
      {
        OkDialogWindow* dialog = new OkDialogWindow(guimanager);
        dialog->SetText(Helper::GetError(&ev).c_str());
      }

      return true;
    } // end TradeConfirmResponse()

    bool Buy::TradeCancel(iEvent& ev)
    {
      Report(PT::Debug, "Exchange: ExchangeCancel.");

      buyWindow->CancelTrade();

      SetTrading(false);

      return true;
    } // end ExchangeCancel()

    bool Buy::TradeOffersList(iEvent& ev)
    {
      using namespace Events;

      //buyWindow->ClearItems();

      csRef<iEvent> list;
      if (ev.Retrieve("offersList", list) == csEventErrNone)
      {
        csRef<iEventAttributeIterator> offers = list->GetAttributeIterator();
        while (offers->HasNext())
        {
          csRef<iEvent> offer; list->Retrieve(offers->Next(), offer);

          unsigned int itemId = Helper::GetUInt(offer, "itemId");
          std::string name = Helper::GetString(offer, "name");
          std::string iconName = Helper::GetString(offer, "iconName");
          unsigned int price = Helper::GetUInt(offer, "price");

          buyWindow->AddItem(itemId, name.c_str(), iconName.c_str(), price);
        } // end while
      }
      else
        Report(PT::Error, "PlayerInventory failed to get inventory list!");

      return true;
    } // end ExchangeOffersList()


  } // Trade namespace
} // PT namespace
