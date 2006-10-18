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

#ifndef _TRADEHANDLER_H_
#define _TRADEHANDLER_H_

#include "common/network/nwtypes.h"

#include "common/network/trademessages.h"

class Server;

class TradeHandler : public MessageHandler
{
private:
  Server* server;

public:
  TradeHandler(Server* server)
  : server(server)
  {
  }

  char getType() { return MESSAGES::TRADE; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::TRADE) assert("wrong message type");
    char id = msg->getMsgId();

    if (id == TRADE::TRADEREQUEST) handleTradeRequest(msg);
    else if (id == TRADE::TRADERESPONSE) handleTradeResponse(msg);
    else if (id == TRADE::BUYITEMREQUESTNPC) handleBuyItemRequestNpc(msg);
    else if (id == TRADE::TRADEOFFERSLISTPVP) handleTradeOffersListPvp(msg);
    else if (id == TRADE::TRADEOFFERACCEPT) handleTradeOfferAccept(msg);
    else if (id == TRADE::TRADECONFIRMREQUEST) handleTradeConfirmRequest(msg);
  }

  void handleTradeRequest(GenericMessage* msg);
  void handleTradeResponse(GenericMessage* msg);
  void handleBuyItemRequestNpc(GenericMessage* msg);
  void handleTradeOffersListPvp(GenericMessage* msg);
  void handleTradeOfferAccept(GenericMessage* msg);
  void handleTradeConfirmRequest(GenericMessage* msg);
};

#endif // _TRADEHANDLER_H_
