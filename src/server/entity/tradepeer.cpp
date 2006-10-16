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

#include "tradesession.h"
#include "tradepeer.h"

TradePeer* TradePeer::getOtherPeer()
{
  if (session == 0) return 0;
  else if (session->peer1 == this) return session->peer2;
  else if (session->peer2 == this) return session->peer1;
  else return 0;
}

void TradePeer::addToOffer(Item* item, unsigned int amount)
{
  TradeSession::Offer offer;
  offer.item = item;
  offer.amount = amount;

  if (session == 0) return;
  else if (session->peer1 == this)
  {
    session->offer1.add(offer);
  }
  else if (session->peer2 == this)
  {
    session->offer2.add(offer);
  }
  else return;
}
