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
#include "server/entity/character.h"
#include "server/entity/tradesession.h"

//#define DEBUG_TRADE

void TradeHandler::handleTradeRequest(GenericMessage* msg)
{
  const PcEntity* c_pc = NetworkHelper::getPcEntity(msg);
  if (!c_pc) return;

  PcEntity* pc = c_pc->getLock();

  TradePeer* this_peer = pc->getTradePeer();

  TradeRequestMessage message;
  message.deserialise(msg->getByteStream());

#ifdef DEBUG_TRADE
  printf("Received TradeRequest from %s\n", *pc->getCharacter()->getName());
  this_peer->debugPrint();
#endif

  int ent_id = message.getEntityId();

  if (pc->getEntity()->getId() == ent_id)
  {
    TradeResponseMessage resp;
    ptString error("You can't trade with yourself.", strlen("You can't trade with yourself."));
    resp.setError(error);
    ByteStream bs;
    resp.serialise(&bs);
    NetworkHelper::sendMessage(pc, bs);
    pc->freeLock();
    return;
  }

  if (this_peer->getSession() != 0)
  {
    TradeResponseMessage resp;
    ptString error("You are already trading with someone.", strlen("You are already trading with someone."));
    resp.setError(error);
    ByteStream bs;
    resp.serialise(&bs);
    NetworkHelper::sendMessage(pc, bs);
    return;
  }

  EntityManager* ent_mgr = server->getEntityManager();
  const Entity* peer_ent = ent_mgr->findById(ent_id);

  if (peer_ent && peer_ent->getType() == Entity::PlayerEntityType)
  {
    PcEntity* peer_pc = peer_ent->getPlayerEntity()->getLock();
    TradePeer* other_peer = peer_pc->getTradePeer();

#ifdef DEBUG_TRADE
    printf("Requesting Trade with %s\n", *peer_pc->getCharacter()->getName());
    other_peer->debugPrint();
#endif

    TradeSession* session = other_peer->getSession();
    if (session != 0)
    {
      TradeResponseMessage resp;
      ptString error("The other player is already trading with someone.", strlen("The other player is already trading with someone."));
      resp.setError(error);
      ByteStream bs;
      resp.serialise(&bs);
      NetworkHelper::sendMessage(pc, bs);
      return;
    }

    session = new TradeSession(this_peer);
    session->sendRequest(other_peer);

#ifdef DEBUG_TRADE
    printf("Relaying request\n");
    this_peer->debugPrint();
    other_peer->debugPrint();
#endif

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

#ifdef DEBUG_TRADE
  this_peer->debugPrint();
  other_peer->debugPrint();
#endif

  if (other_peer)
  {
    TradeResponseMessage message;
    message.deserialise(msg->getByteStream());

    ptString error = message.getError();

    other_peer->getSession()->sendResponse(error);
    NetworkHelper::sendMessage(other_peer->getEntity(), *msg->getByteStream());
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

#ifdef DEBUG_TRADE
  peer->debugPrint();
#endif

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

  peer->clearOffer();
  for (unsigned char i = 0; i < offer_req.getOffersCount(); i++)
  {
    peer->addToOffer(pc, offer_req.getItemId(i), offer_req.getAmount(i));
  }

  const Array<TradeSession::Offer>* real_offer = peer->getOffer();

  if (real_offer->getCount() != offer_req.getOffersCount())
    return; //something's wrong here... cheaters?

  offer.setOffersCount(real_offer->getCount());
  for (size_t i = 0; real_offer && i < real_offer->getCount(); i++)
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

#ifdef DEBUG_TRADE
  peer->debugPrint();
  peer->getOtherPeer()->debugPrint();
#endif

  TradeCancelMessage c_msg;
  ByteStream bs;
  c_msg.serialise(&bs);

  NetworkHelper::sendMessage(pc, bs);
  NetworkHelper::sendMessage(peer->getOtherPeer()->getEntity(), bs);

  peer->getSession()->cancel();

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

    // terminate trading session.
    peer->getSession()->cancel();
  }

  pc->freeLock();
}
