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

#include "exchange.h"

#include "common/event/eventmanager.h"
#include "common/event/tradeevent.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/gui/guimanager.h"
#include "client/gui/gui.h"

#include "client/network/network.h"
#include "common/network/trademessages.h"

#include "client/entity/entitymanager.h"
#include "common/entity/entity.h"


namespace PT
{
  namespace Trade
  {
    Exchange::Exchange () : guimanager(0), tradeWindow(0)
    {
    }

    Exchange::~Exchange ()
    {
    }

    bool Exchange::Initialize ()
    {
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;
      guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return true;

      tradeWindow = guimanager->GetWindow<TradeWindow>(TRADEWINDOW);
      if (!tradeWindow) return true;

      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(Exchange, ExchangeRequest, "trade.exchange.request")
      PT_REGISTER_LISTENER(Exchange, ExchangeResponse, "trade.exchange.response")
      PT_REGISTER_LISTENER(Exchange, ExchangeOfferAccept, "trade.exchange.offer.accept")
      PT_REGISTER_LISTENER(Exchange, ExchangeOffersList, "trade.exchange.offer.list")

        return true;
    } // end Initialize()

    void Exchange::SetTrading(bool value)
    {
      using namespace PT::Events;

      //trade.general.confirm/cancel is used for buy/sell aswell, so only listen to it when neccessary!
      EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
      if (value)
      {
        tradeConfirm.AttachNew(new EventHandler<Exchange>(&Exchange::ExchangeConfirmResponse, this));
        evmgr->AddListener("trade.general.confirm", tradeConfirm);

        tradeCancel.AttachNew(new EventHandler<Exchange>(&Exchange::ExchangeCancel, this));
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

    void Exchange::Request(unsigned int entityId)
    {
      ExchangeRequestMessage msg;
      msg.setEntityId(entityId);
      PointerLibrary::getInstance()->getNetwork()->send(&msg);
    } // end Request()

    bool Exchange::OnYesRequest(const CEGUI::EventArgs& args)
    {
      tradeWindow->ShowWindow();
      SetTrading(true);

      ExchangeResponseMessage msg;
      PointerLibrary::getInstance()->getNetwork()->send(&msg);

      return true;
    } // end OnYesRequest()

    bool Exchange::OnNoRequest(const CEGUI::EventArgs& args)
    {
      ExchangeResponseMessage msg;
      msg.setError(ptString("The other player declined the trade.",
        strlen("The other player declined the trade.")));
      PointerLibrary::getInstance()->getNetwork()->send(&msg);

      return true;
    } // end OnNoRequest()

    bool Exchange::ExchangeRequest(iEvent& ev)
    {
      using namespace PT::GUI::Windows;
      using namespace PT::Entity;
      using namespace PT::Events;

      unsigned int entityId = -1;
      ev.Retrieve("entityId", entityId);

      EntityManager* entityManager = PointerLibrary::getInstance()->getEntityManager();
      if (!entityManager) return true;
      Common::Entity::Entityp ent = entityManager->FindById(entityId);
      if (!ent) return true;

      std::stringstream out;
      out << "Do you want to trade with " << ent->GetName() << "?";

      ConfirmDialogWindow* dialog = new ConfirmDialogWindow(guimanager);
      dialog->SetText(out.str().c_str());
      dialog->SetYesEvent(CEGUI::Event::Subscriber(&Exchange::OnYesRequest, this));
      dialog->SetNoEvent(CEGUI::Event::Subscriber(&Exchange::OnNoRequest, this));

      return true;
    } // end ExchangeRequest()

    bool Exchange::ExchangeResponse(iEvent& ev)
    {
      using namespace PT::GUI::Windows;
      using namespace PT::Events;

      if (!Helper::HasError(&ev))
      {
        tradeWindow->ShowWindow();
        SetTrading(true);
      }
      else
      {
        OkDialogWindow* dialog = new OkDialogWindow(guimanager);
        dialog->SetText(Helper::GetError(&ev).c_str());
      }

      return true;
    } // end ExchangeResponse()

    bool Exchange::ExchangeConfirmResponse(iEvent& ev)
    {
      using namespace PT::GUI::Windows;
      using namespace PT::Events;

      if (!Helper::HasError(&ev))
      {
        SetTrading(false); // No errors, exchange has finished.

        if (tradeWindow->IsVisible())
        {
          tradeWindow->AcceptTrade();
        }
      }
      else
      {
        OkDialogWindow* dialog = new OkDialogWindow(guimanager);
        dialog->SetText(Helper::GetError(&ev).c_str());
      }

      return true;
    } // end ExchangeConfirmResponse()

    bool Exchange::ExchangeOfferAccept(iEvent& ev)
    {
      Report(PT::Debug, "Exchange: ExchangeOfferAccept.");

      tradeWindow->SetAccept(2, true);

      return true;
    } // end ExchangeOfferAccept()

    bool Exchange::ExchangeCancel(iEvent& ev)
    {
      Report(PT::Debug, "Exchange: ExchangeCancel.");

      tradeWindow->CancelTrade();

      SetTrading(false);

      return true;
    } // end ExchangeCancel()

    bool Exchange::ExchangeOffersList(iEvent& ev)
    {
      using namespace Events;

      tradeWindow->SetAccept(2, false);
      tradeWindow->ClearItems();

      csRef<iEvent> list;
      if (ev.Retrieve("offersList", list) == csEventErrNone)
      {
        csRef<iEventAttributeIterator> offers = list->GetAttributeIterator();
        while (offers->HasNext())
        {
          csRef<iEvent> offer; list->Retrieve(offers->Next(), offer);

          unsigned int itemId = Helper::GetUInt(offer, "itemId");
          unsigned int variation = Helper::GetUInt(offer, "variation");
          std::string name = Helper::GetString(offer, "name");
          std::string iconName = Helper::GetString(offer, "iconName");
          unsigned int slotId = Helper::GetUInt(offer, "slotId");

          tradeWindow->AddItem(2, itemId, slotId, name.c_str(), iconName.c_str());
        } // end while
      }
      else
        Report(PT::Error, "PlayerInventory failed to get inventory list!");

      return true;
    } // end ExchangeOffersList()


  } // Trade namespace
} // PT namespace
