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

void ChatHandler::handleSay(GenericMessage* msg)
{
  SayMessage chatmsg;
  chatmsg.deserialise(msg->getByteStream());
  client->chat(0, chatmsg.getMessage(), *chatmsg.getSpeakerName());
}

void ChatHandler::handleWhisperFrom(GenericMessage* msg)
{
  WhisperFromMessage chatmsg;
  chatmsg.deserialise(msg->getByteStream());
  client->chat(1, chatmsg.getMessage(), *chatmsg.getSpeakerName());
}

void ChatHandler::handleShout(GenericMessage* msg)
{
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
