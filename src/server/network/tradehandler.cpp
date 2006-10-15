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
#include "common/entity/tradesession.h"

void TradeHandler::handleTradeRequest(GenericMessage* msg)
{
  PcEntity* pc = NetworkHelper::getPcEntity(msg);
  if (!pc) return;

  TradePeer* this_peer = pc->getTradePeer();

  TradeRequestMessage message;
  message.deserialise(msg->getByteStream());

  int ent_id = message.getEntityId();

  EntityManager* ent_mgr = server->getEntityManager();
  CharacterEntity* peer_ent = ent_mgr->getCharEntity(ent_id);

  if (peer_ent == 0) return;

  if (peer_ent->getType() == Entity::PlayerEntity)
  {
    TradePeer* other_peer = ((PcEntity*) peer_ent)->getTradePeer();
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

    User* other_user = ((PcEntity*) peer_ent)->getUser();
    if (!other_user) return;

    Connection* other_conn = other_user->getConnection();
    if (!other_conn) return;

    message.setEntityId(pc->getId());

    ByteStream bs;
    message.serialise(&bs);

    other_conn->send(bs);
  }
}

void TradeHandler::handleTradeResponse(GenericMessage* msg)
{
  PcEntity* pc = NetworkHelper::getPcEntity(msg);
  if (!pc) return;

  TradePeer* this_peer = pc->getTradePeer();

  TradePeer* other_peer = this_peer->getOtherPeer();

  if (other_peer == 0) return;

  TradeResponseMessage message;
  message.deserialise(msg->getByteStream());

  ptString error = message.getError();

  this_peer->getSession()->sendResponse(error);
}

void TradeHandler::handleBuyItemRequestNpc(GenericMessage* msg)
{
}

