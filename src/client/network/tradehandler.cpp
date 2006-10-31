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

void TradeHandler::handleTradeRequest(GenericMessage* msg)
{
  printf("RESPONSE: handleTradeRequest\n");

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
  printf("RESPONSE: handleTradeResponse\n");

  TradeResponseMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  printf("Got TradeResponse!\n");

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (trade_msg.getError().isNull())
    guimanager->GetTradeWindow()->ShowWindow();
  else
  {
    OkDialogWindow* dialog = guimanager->CreateOkWindow();
    dialog->SetText(*trade_msg.getError());
  }
}

void TradeHandler::handleBuyItemResponseNpc(GenericMessage* msg)
{
}

void TradeHandler::handleTradeConfirmResponse(GenericMessage* msg)
{
  printf("RESPONSE: handleTradeConfirmResponse\n");

  TradeConfirmResponseMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  if (trade_msg.getError().isNull())
    guimanager->GetTradeWindow()->AcceptTrade();
  else
  {
    OkDialogWindow* dialog = guimanager->CreateOkWindow();
    dialog->SetText(*trade_msg.getError());
  }
}

void TradeHandler::handleTradeOfferAccept(GenericMessage* msg)
{
  printf("RESPONSE: handleTradeOfferAccept\n");

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  guimanager->GetTradeWindow()->SetAccept(2, true);
}

void TradeHandler::handleTradeCancel(GenericMessage* msg)
{
  printf("RESPONSE: handleTradeCancel\n");

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
  guimanager->GetTradeWindow()->CancelTrade();
}

void TradeHandler::handleTradeOffersListPvp(GenericMessage* msg)
{
  TradeOffersListPvpMessage trade_msg;
  trade_msg.deserialise(msg->getByteStream());

  printf("TradeHandler: \n");

  GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();

  guimanager->GetTradeWindow()->SetAccept(2, false);
  guimanager->GetTradeWindow()->ClearItems();

  for (int i=0; i<trade_msg.getOffersCount(); i++)
  {
    printf("Item %d in slot %d\n", trade_msg.getItemId(i), trade_msg.getSlotId(i));
    guimanager->GetTradeWindow()->AddItem(2, trade_msg.getItemId(i), trade_msg.getSlotId(i));
  }
}

void TradeHandler::handleTradeOffersListNpc(GenericMessage* msg)
{
}
