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

#include "client/network/network.h"

#include "common/event/eventmanager.h"
#include "common/event/chatevent.h"

#include "client/pointer/pointer.h"

void ChatHandler::handleChat(GenericMessage* msg)
{
  ChatMessage chatmsg;
  chatmsg.deserialise(msg->getByteStream());
  const char* chattype = "chat.say";
  if (chatmsg.getVolume() == 0) chattype = "chat.whisper";

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> chatEvent = evmgr->CreateEvent(chattype, true);

  chatEvent->Add("nickName", *chatmsg.getSpeakerName());
  chatEvent->Add("message", chatmsg.getMessage());

  evmgr->AddEvent(chatEvent);
}

void ChatHandler::handleGroup(GenericMessage* msg)
{
  GroupMessage grpmsg;
  grpmsg.deserialise(msg->getByteStream());

  PT::Events::EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  csRef<iEvent> chatEvent = evmgr->CreateEvent("chat.group", true);

  chatEvent->Add("nickName", *grpmsg.getSpeakerName());
  chatEvent->Add("message", grpmsg.getMessage());

  evmgr->AddEvent(chatEvent);
}
