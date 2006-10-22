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

#ifndef _TRADE_PEER_H_
#define _TRADE_PEER_H_

#include "common/util/monitor.h"
#include "tradesession.h"

class Item;
class TradeSession;
class PcEntity;

class TradePeer
{
private:
  TradeSession* session;
  ptMonitor<PcEntity> entity;

  bool checkOffer(PcEntity* pc, Array<TradeSession::Offer>& offers, int item_id, unsigned int amount);

public:
  TradePeer() : session (0) {}
  ~TradePeer() { if (session) session->cancel(); }

  void setSession(TradeSession* session) { this->session = session; }
  TradeSession* getSession() { return session; }

  TradePeer* getOtherPeer();

  bool addToOffer(PcEntity* pc, int item_id, unsigned int amount);
  const Array<TradeSession::Offer>* getOffer();

  void clearOffer();
  void acceptOffer();
  void confirmOffer();

  void setEntity(const PcEntity* e);
  const PcEntity* getEntity();

  void debugPrint();
};

#endif // _TRADE_PEER_H_
