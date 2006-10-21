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

bool TradePeer::checkOffer(PcEntity* pc, Array<TradeSession::Offer>& offers, int item_id, unsigned int amount)
{
  const Character* c_char = pc->getCharacter();
  if (!c_char) return false;

  TradeSession::Offer& offer = TradeSession::Offer();
  offer.item_id = item_id;
  offer.amount = amount;

  Character* c = c_char->getLock();

  for (size_t i = 0; i<offers.getCount(); i++)
  {
    if (offers.get(i).item_id == item_id)
    {
      offer = offers.get(i);
      if (c->getInventory()->getTotalAmount(item_id) >= amount + offer.amount)
      {
        offers.add(offer);
        c->freeLock();
        return true;
      }
    }
  }

  if (c->getInventory()->getTotalAmount(item_id) >= amount)
  {
    offers.add(offer);
    c->freeLock();
    return true;
  }

  c->freeLock();
  return false;
}

bool TradePeer::addToOffer(PcEntity* pc, int item_id, unsigned int amount)
{
  if (session == 0) return false;

  if (session->peer1 == this)
  {
    if ( checkOffer(pc, session->offer1, item_id, amount) )
      return true;
  }
  else if (session->peer2 == this)
  {
    if ( checkOffer(pc, session->offer2, item_id, amount) )
      return true;
  }
  
  return false;
}

const Array<TradeSession::Offer>* TradePeer::getOffer()
{
  if (!session) return 0;

  if (session->peer1 == this) 
    return &session->offer1;

  if (session->peer2 == this) 
    return &session->offer2;

  return 0;
}

void TradePeer::setEntity(const PcEntity* e)
{
  entity = e->getRef();
}

const PcEntity* TradePeer::getEntity()
{
  return entity.get();
}

void TradePeer::clearOffer()
{
  if (!session) return;

  if (session->bothAccepted()) return;

  if (session->peer1 == this)
  {
    session->accept_offer_1 = false;
    session->offer1.removeAll();
  }
  else if (session->peer2 == this)
  {
    session->accept_offer_2 = false;
    session->offer2.removeAll();
  }
}

void TradePeer::acceptOffer()
{
  if (!session) return;

  if (session->peer1 == this) session->accept_offer_2 = true;
  else if (session->peer2 == this) session->accept_offer_1 = true;
}

void TradePeer::confirmOffer()
{
  if (!session) return;

  if (session->peer1 == this) session->confirm_offer_2 = true;
  else if (session->peer2 == this) session->confirm_offer_1 = true;
}
