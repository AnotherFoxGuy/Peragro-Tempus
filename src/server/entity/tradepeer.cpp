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

#include "server/entity/character.h"
#include "server/entity/item.h"
#include "server/entity/inventory.h"
#include "server/entity/pcentity.h"

TradePeer* TradePeer::getOtherPeer()
{
  if (session == 0) return 0;
  else if (session->peer1 == this) return session->peer2;
  else if (session->peer2 == this) return session->peer1;
  else return 0;
}

void TradePeer::checkOffer(PcEntity* pc, Array<TradeSession::Offer>& offers, int item_id, unsigned int amount)
{
  const Character* c_char = pc->getCharacter();
  if (!c_char) return;

  TradeSession::Offer& offer = TradeSession::Offer();
  offer.item_id = item_id;
  offer.amount = amount;

  Character* c = c_char->getLock();

  for (size_t i = 0; i<offers.getCount(); i++)
  {
    if (offers.get(i).item_id == item_id)
    {
      offer = offers.get(i);
      if (c->getInventory()->getAmount(item_id) >= amount + offer.amount)
      {
        offers.add(offer);
        c->freeLock();
        return;
      }
    }
  }

  if (c->getInventory()->getAmount(item_id) >= amount)
  {
    offers.add(offer);
  }
  c->freeLock();
}

void TradePeer::addToOffer(PcEntity* pc, int item_id, unsigned int amount)
{
  if (session == 0) return;
  else if (session->peer1 == this)
  {
    checkOffer(pc, session->offer1, item_id, amount);
  }
  else if (session->peer2 == this)
  {
    checkOffer(pc, session->offer1, item_id, amount);
  }
  else return;
}
