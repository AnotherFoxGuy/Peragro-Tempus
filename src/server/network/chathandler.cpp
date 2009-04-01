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
#include "server/group/chatmanager.h"

void ChatHandler::handleChat(GenericMessage* msg)
{
  boost::shared_ptr<PcEntity> ent = NetworkHelper::GetEntity(msg);
  if (!ent) return;

  const std::string name = ent->GetName();

  ChatMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  ChatMessage out_msg;
  out_msg.setSpeakerName(name);
  out_msg.setVolume(in_msg.getVolume());
  out_msg.setMessage(in_msg.getMessage());

  ByteStream bs;
  out_msg.serialise(&bs);

  if (in_msg.getVolume() == 0xFF) NetworkHelper::broadcast(bs);
  else
    NetworkHelper::localcast(bs, ent, in_msg.getVolume()*100);
  
  /* TODO: client decides how loud a message is.  If there is to be a
     policy to limit how far one can shout, here's the place to implement
     it. */
}

void ChatHandler::handleWhisperTo(GenericMessage* msg)
{
  boost::shared_ptr<PcEntity> ent = NetworkHelper::GetEntity(msg);
  if (!ent) return;

  const std::string name = ent->GetName();

  WhisperToMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  Server* server = Server::getServer();

  boost::shared_ptr<PcEntity> target = 
    boost::shared_dynamic_cast<PcEntity>(server->getEntityManager()->FindByName(*in_msg.getListenerName()));
  if (!target) return;

  ChatMessage out_msg;
  out_msg.setMessage(in_msg.getMessage());
  out_msg.setVolume(0); // whisper
  out_msg.setSpeakerName(name);

  ByteStream bs;
  out_msg.serialise(&bs);

  NetworkHelper::sendMessage(target.get(), bs);
}

void ChatHandler::handleGroup(GenericMessage* msg)
{
  boost::shared_ptr<PcEntity> ent = NetworkHelper::GetEntity(msg);
  if (!ent) return;

  GroupMessage in_msg;
  in_msg.deserialise(msg->getByteStream());

  ChatGroupSet* chman = ChatManager::getChatManager();
  if (!chman) return;
  ChatGroup* chgrp = chman->getChannel(*in_msg.getChannel());
  if (!chgrp) return;

  chgrp->process( ent.get(), in_msg.getMessage() );
}
