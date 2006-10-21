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
  const Entity* peer_ent = ent_mgr->findById(ent_id);

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

  if (peer->getSession() == 0)
  {
    pc->freeLock();
    return;
  }

  if (peer->getSession()->bothAccepted()) 
    return; // Too late for changes now!

  TradeOffersListPvpMessage offer_req;
  offer_req.deserialise(msg->getByteStream());

  TradeOffersListPvpMessage offer;
  offer.deserialise(msg->getByteStream());

  for (unsigned char i = 0; 0 < offer_req.getOffersCount(); i++)
  {
    peer->addToOffer(pc, offer_req.getItemId(i), offer_req.getAmount(i));
  }

  const Array<TradeSession::Offer>* real_offer = peer->getOffer();

  if (real_offer->getCount() != offer_req.getOffersCount())
    return; //something's wrong here... cheaters?

  offer.setOffersCount(real_offer->getCount());
  for (size_t i = 0; real_offer && 0 < real_offer->getCount(); i++)
  {
    offer.setItemId(i, real_offer->get(i).item_id);
    offer.setAmount(i, real_offer->get(i).amount);
    offer.setSlotId(i, offer_req.getSlotId(i));
  }

  ByteStream bs;
  offer.serialise(&bs);

  NetworkHelper::sendMessage(peer->getOtherPeer()->getEntity(), bs);

  pc->freeLock();
}

void TradeHandler::handleTradeCancel(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* peer = pc->getTradePeer();

  if (peer->getSession() == 0)
  {
    pc->freeLock();
    return;
  }

  peer->getSession()->cancel();

  TradeCancelMessage c_msg;
  ByteStream bs;
  c_msg.serialise(&bs);

  NetworkHelper::sendMessage(pc, bs);
  NetworkHelper::sendMessage(peer->getOtherPeer()->getEntity(), bs);

  pc->freeLock();
}

void TradeHandler::handleTradeOfferAccept(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* peer = pc->getTradePeer();

  if (peer->getSession() == 0)
  {
    pc->freeLock();
    return;
  }

  peer->acceptOffer();

  TradeOfferAcceptMessage accept_msg;
  ByteStream bs;
  accept_msg.serialise(&bs);
  NetworkHelper::sendMessage(peer->getOtherPeer()->getEntity(), bs);

  pc->freeLock();
}

void TradeHandler::handleTradeConfirmRequest(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* peer = pc->getTradePeer();

  if (peer->getSession() == 0)
  {
    pc->freeLock();
    return;
  }

  peer->confirmOffer();

  if (peer->getSession()->bothConfirmed())
  {
    peer->getSession()->exchange();

    TradeConfirmResponseMessage accept_msg;
    ByteStream bs;
    accept_msg.serialise(&bs);

    NetworkHelper::sendMessage(pc, bs);
    NetworkHelper::sendMessage(peer->getOtherPeer()->getEntity(), bs);
  }

  pc->freeLock();
}
