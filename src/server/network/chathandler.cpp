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

#include "server/entity/entitymanager.h"
#include "server/entity/pcentity.h"

void ChatHandler::handleSay(GenericMessage* msg)
{
  const PcEntity* ent = NetworkHelper::getPcEntity(msg);
  if (!ent) return;

  const ptString name = ent->getEntity()->getName();

  SayMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  SayMessage out_msg;
  out_msg.setSpeakerName(name);
  out_msg.setMessage(in_msg.getMessage());

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::broadcast(bs);
}

void ChatHandler::handleShout(GenericMessage* msg)
{
}

void ChatHandler::handleWhisperTo(GenericMessage* msg)
{
  const PcEntity* ent = NetworkHelper::getPcEntity(msg);
  if (!ent) return;

  const ptString name = ent->getEntity()->getName();

  WhisperToMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  const Entity* entity = server->getEntityManager()->findByName(in_msg.getListenerName());
  if (!entity || entity->getType() != Entity::PlayerEntityType) return;

  WhisperFromMessage out_msg;
  out_msg.setMessage(in_msg.getMessage());
  out_msg.setSpeakerName(name);

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::sendMessage(entity->getPlayerEntity(), bs);
}

void ChatHandler::handleParty(GenericMessage* msg)
{
}

void ChatHandler::handleGuild(GenericMessage* msg)
{
}

void ChatHandler::handleFamily(GenericMessage* msg)
{
}

