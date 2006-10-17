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
#include "networkhelper.h"
#include "tradehandler.h"
#include "server/entity/entitymanager.h"
#include "server/entity/pcentity.h"
#include "server/entity/tradesession.h"

void TradeHandler::handleTradeRequest(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* this_peer = pc->getTradePeer();

  TradeRequestMessage message;
  message.deserialise(msg->getByteStream());

  int ent_id = message.getEntityId();

  EntityManager* ent_mgr = server->getEntityManager();
  const Entity* peer_ent = ent_mgr->getEntity(ent_id);

  if (peer_ent && peer_ent->getType() == Entity::PlayerEntityType)
  {
    PcEntity* peer_pc = peer_ent->getPlayerEntity()->getLock();
    TradePeer* other_peer = peer_pc->getTradePeer();
    TradeSession* session = other_peer->getSession();

    if (session != 0)
    {
      // Error! Other player is already trading with someone
    }

    session = new TradeSession(this_peer);
    const char* error = session->sendRequest(other_peer);

    if (error != 0)
    {
      // Error! This player is already trading with someone!
    }

    message.setEntityId(pc->getEntity()->getId());

    ByteStream bs;
    message.serialise(&bs);

    NetworkHelper::sendMessage(peer_ent, bs);
    peer_pc->freeLock();
  }
  pc->freeLock();
}

void TradeHandler::handleTradeResponse(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* this_peer = pc->getTradePeer();

  TradePeer* other_peer = this_peer->getOtherPeer();

  if (other_peer)
  {
    TradeResponseMessage message;
    message.deserialise(msg->getByteStream());

    ptString error = message.getError();

    this_peer->getSession()->sendResponse(error);
  }
  pc->freeLock();
}

void TradeHandler::handleBuyItemRequestNpc(GenericMessage* msg)
{
}

void TradeHandler::handleTradeOffersListPvp(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* peer = pc->getTradePeer();

  TradeOffersListPvpMessage offer;
  offer.deserialise(msg->getByteStream());

  for (unsigned char i = 0; 0 < offer.getOffersCount(); i++)
  {
    peer->addToOffer(pc, offer.getItemId(i), offer.getAmount(i));
  }

  //peer->sendOffer();
  pc->freeLock();
}

void TradeHandler::handleTradeOfferAccept(GenericMessage* msg)
{
}

void TradeHandler::handleTradeConfirmRequest(GenericMessage* msg)
{
}
