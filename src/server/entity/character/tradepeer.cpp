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

#include "server/entity/character/character.h"
#include "server/entity/character/inventory.h"
#include "server/entity/pcentity.h"
#include "common/util/printhelper.h"

TradePeer* TradePeer::getOtherPeer()
{
  if (session == 0) return 0;
  else if (session->peer1 == this) return session->peer2;
  else if (session->peer2 == this) return session->peer1;
  else return 0;
}

bool TradePeer::checkOffer(PcEntity* pc, Array<TradeSession::Offer>& offers, InventoryEntry* item)
{
  /*
  const Character* c_char = pc->getCharacter();
  if (!c_char) return false;

  ptScopedMonitorable<Character> c (c_char);

  unsigned int item_count = 0;

  for (size_t i = 0; i<offers.getCount(); i++)
  {
    if (offers.get(i).item == item)
    {
      item_count++;
    }
  }

  if (c->getInventory()->getTotalAmount(item->id, item->variation) > item_count)
  {
    TradeSession::Offer offer;
    offer.item = item;
    offer.amount = 1;
    offers.add(offer);
    return true;
  }
*/
  return false;
}

bool TradePeer::addToOffer(PcEntity* pc, InventoryEntry* item)
{
  if (session == 0) return false;

  if (session->peer1 == this)
  {
    if ( checkOffer(pc, session->offer1, item) )
      return true;
  }
  else if (session->peer2 == this)
  {
    if ( checkOffer(pc, session->offer2, item) )
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

void TradePeer::setEntity(PcEntity* e)
{
  entity = e;
}

PcEntity* TradePeer::getEntity()
{
  return entity;
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

void TradePeer::debugPrint()
{
  printf("====================================================\n");
  printf("TradePeer of %s\n", getEntity()->GetName());
  printf("----------------------------------------------------\n");
  printf("Session: %" PRIxPTR "\n", (intptr_t) session);
  if (session)
  {
    printf("Session peers: 1) %" PRIxPTR " \t 2) %" PRIxPTR "\n",
           (intptr_t) session->peer1, (intptr_t) session->peer2);
  }
  printf("----------------------------------------------------\n");
}
