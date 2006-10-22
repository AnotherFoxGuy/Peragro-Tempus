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

#ifndef _TRADE_SESSION_H_
#define _TRADE_SESSION_H_

#include "common/util/array.h"

class TradePeer;
class Item;
class ptString;
class NetMessage;

class TradeSession
{
public:
  class Offer
  {
  public:
    int item_id;
    unsigned int amount;
  };

private:
  TradePeer* peer1;
  TradePeer* peer2;

  bool request_accepted;

  Array<Offer> offer1;
  Array<Offer> offer2;

  bool accept_offer_1;
  bool accept_offer_2;

  bool confirm_offer_1;
  bool confirm_offer_2;

public:
  TradeSession(TradePeer* initiator)
  {
    peer1 = initiator;
    peer2 = 0;
    accept_offer_1 = false;
    accept_offer_2 = false;
    confirm_offer_1 = false;
    confirm_offer_2 = false;
  }

  void sendRequest(TradePeer* peer2);
  void sendResponse(const ptString& error);

  void cancel();

  bool bothAccepted() { return accept_offer_1 & accept_offer_2; }
  bool bothConfirmed() { return confirm_offer_1 & confirm_offer_2; }

  void exchange();

  friend class TradePeer;
};

#endif // _TRADE_SESSION_H_
