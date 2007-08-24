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
#include "tradehandler.h"

#include "client/gui/gui.h"
#include "client/gui/guimanager.h"

#include "client/entity/ptentitymanager.h"

#include "client/reporter/reporter.h"

void TradeHandler::handleTradeRequest(GenericMessage* msg)
{
  Report(PT::Debug, "TradeHandler: handleTradeRequest.");

  TradeRequestMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  PtEntity* ent = PointerLibrary::getInstance()->getEntityManager()->findPtEntById(trade_msg.getEntityId());
  if(!ent) return;

  char buffer[1024];
  sprintf(buffer, "Do you want to trade with %s?", ent->GetName().GetData());

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  TradeWindow* tradewindow = guimanager->GetTradeWindow();
  ConfirmDialogWindow* dialog = guimanager->CreateConfirmWindow();
  dialog->SetText(buffer);
  dialog->SetYesEvent(CEGUI::Event::Subscriber(&TradeWindow::OnYesRequest, tradewindow));
  dialog->SetNoEvent(CEGUI::Event::Subscriber(&TradeWindow::OnNoRequest, tradewindow)); 
}

void TradeHandler::handleTradeResponse(GenericMessage* msg)
{
  Report(PT::Debug, "TradeHandler: handleTradeResponse, Got TradeResponse!");

  TradeResponseMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (trade_msg.getError().isNull())
    guimanager->GetTradeWindow()->ShowWindow();
  else
  {
    OkDialogWindow* dialog = guimanager->CreateOkWindow();
    dialog->SetText(*trade_msg.getError());
  }
}

void TradeHandler::handleTradeConfirmResponse(GenericMessage* msg)
{
  Report(PT::Debug, "TradeHandler: handleTradeConfirmResponse.");

  TradeConfirmResponseMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (trade_msg.getError().isNull())
  {
    if (guimanager->GetTradeWindow() && guimanager->GetTradeWindow()->IsVisible())
    {
      guimanager->GetTradeWindow()->AcceptTrade();
    }
    else if (guimanager->GetBuyWindow() && guimanager->GetBuyWindow()->IsVisible())
    {
      guimanager->GetBuyWindow()->AcceptTrade();
    }
    else if (guimanager->GetSellWindow() && guimanager->GetSellWindow()->IsVisible())
    {
      guimanager->GetSellWindow()->AcceptTrade();
    }
  }
  else
  {
    OkDialogWindow* dialog = guimanager->CreateOkWindow();
    dialog->SetText(*trade_msg.getError());
  }
}

void TradeHandler::handleTradeOfferAccept(GenericMessage* msg)
{
  Report(PT::Debug, "TradeHandler: handleTradeOfferAccept.");

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  guimanager->GetTradeWindow()->SetAccept(2, true);
}

void TradeHandler::handleTradeCancel(GenericMessage* msg)
{
  Report(PT::Debug, "TradeHandler: handleTradeCancel.");

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  guimanager->GetTradeWindow()->CancelTrade();
}

void TradeHandler::handleTradeOffersListPvp(GenericMessage* msg)
{
  TradeOffersListPvpMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  Report(PT::Debug, "TradeHandler: About to add % items.", trade_msg.getOffersCount());

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  guimanager->GetTradeWindow()->SetAccept(2, false);
  guimanager->GetTradeWindow()->ClearItems();

  for (int i=0; i<trade_msg.getOffersCount(); i++)
  {
    Report(PT::Debug, "Item %d in slot %d", trade_msg.getItemId(i), trade_msg.getSlotId(i));
    guimanager->GetTradeWindow()->AddItem(2, trade_msg.getItemId(i), trade_msg.getSlotId(i));
  }
}

void TradeHandler::handleTradeOffersListNpc(GenericMessage* msg)
{
  TradeOffersListNpcMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  guimanager->GetNpcDialogWindow()->HideWindow();

  if (trade_msg.getIsBuy() == 1)
  {
    Report(PT::Debug, "TradeHandler: Got %d Buy Offers from NPC!", trade_msg.getOffersCount());
    for (unsigned char i = 0; i < trade_msg.getOffersCount(); i++)
    {
      Report(PT::Debug, " %d) Item %d \t %d money", i, trade_msg.getItemId(i), trade_msg.getPrice(i));
      guimanager->GetBuyWindow()->AddItem(trade_msg.getItemId(i), trade_msg.getPrice(i));
    }
  }
  else
  {
    Report(PT::Debug, "TradeHandler: Got %d Sell Offers from NPC!", trade_msg.getOffersCount());
    for (unsigned char i = 0; i < trade_msg.getOffersCount(); i++)
    {
      Report(PT::Debug, " %d) Item %d \t %d money", i, trade_msg.getItemId(i), trade_msg.getPrice(i));
      //guimanager->GetSellWindow()->AddItem(trade_msg.getItemId(i), trade_msg.getPrice(i));
    }
  }
}
