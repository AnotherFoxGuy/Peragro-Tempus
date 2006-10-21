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

#include "common/util/ptstring.h"

#include "server/entity/character.h"
#include "server/entity/inventory.h"
#include "server/entity/itemmanager.h"
#include "server/entity/pcentity.h"

#include "server/server.h"

const char* TradeSession::sendRequest(TradePeer* peer)
{
  if (peer2 && peer2->getSession() != 0)
  {
    return "is Busy";
  }
  peer2 = peer;
  peer2->setSession(this);
  peer1->setSession(this);
  return 0;
}

void TradeSession::sendResponse(const ptString& error)
{
  if (error == ptString::Null)
  {
    request_accepted = true;
  }
  else
  {
    peer1->setSession(0);
    peer2->setSession(0);
    delete this;
  }
}

void TradeSession::cancel()
{
  peer1->setSession(0);
  peer2->setSession(0);
  delete this;
}

void TradeSession::exchange()
{
  Character* char1 = peer1->getEntity()->getCharacter()->getLock();
  Inventory* inv1 = char1->getInventory();

  Character* char2 = peer2->getEntity()->getCharacter()->getLock();
  Inventory* inv2 = char2->getInventory();

  ItemManager* item_mgr = Server::getServer()->getItemManager();

  for (size_t i=0; i<offer1.getCount(); i++)
  {
    Offer& offer = offer1.get(i);
    Item* item = item_mgr->findById(offer.item_id);
    inv1->takeItem(item, offer.amount);
  }

  for (size_t i=0; i<offer2.getCount(); i++)
  {
    Offer& offer = offer2.get(i);
    Item* item = item_mgr->findById(offer.item_id);
    inv2->takeItem(item, offer.amount);
  }

  for (size_t i=0; i<offer1.getCount(); i++)
  {
    Offer& offer = offer1.get(i);
    Item* item = item_mgr->findById(offer.item_id);
    inv2->addItem(item, offer.amount);
  }

  for (size_t i=0; i<offer2.getCount(); i++)
  {
    Offer& offer = offer2.get(i);
    Item* item = item_mgr->findById(offer.item_id);
    inv1->addItem(item, offer.amount);
  }

  char1->freeLock();
  char2->freeLock();
}
