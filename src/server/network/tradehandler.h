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

#ifndef TRADEHANDLER_H
#define TRADEHANDLER_H

#include "common/network/nwtypes.h"
#include "common/network/messagehandler.h"

#include "common/network/trademessages.h"

class TradeHandler : public MessageHandler
{
public:
  TradeHandler()
  {
  }

  char getType() { return MESSAGES::TRADE; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::TRADE) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == TRADE::EXCHANGEREQUEST) handleExchangeRequest(msg);
    else if (id == TRADE::EXCHANGERESPONSE) handleExchangeResponse(msg);
    else if (id == TRADE::EXCHANGEOFFERSLIST) handleExchangeOffersList(msg);
    else if (id == TRADE::TRADEORDERLISTNPC) handleTradeOrderListNpc(msg);
    else if (id == TRADE::TRADECANCEL) handleTradeCancel(msg);
    else if (id == TRADE::TRADEOFFERACCEPT) handleTradeOfferAccept(msg);
    else if (id == TRADE::TRADECONFIRMREQUEST) handleTradeConfirmRequest(msg);
  }

  void handleExchangeRequest(GenericMessage* msg);
  void handleExchangeResponse(GenericMessage* msg);
  void handleExchangeOffersList(GenericMessage* msg);
  void handleTradeOrderListNpc(GenericMessage* msg);
  void handleTradeCancel(GenericMessage* msg);
  void handleTradeOfferAccept(GenericMessage* msg);
  void handleTradeConfirmRequest(GenericMessage* msg);
};

#endif // TRADEHANDLER_H
