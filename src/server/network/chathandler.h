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

#ifndef _CHATHANDLER_H_
#define _CHATHANDLER_H_

#include "common/network/nwtypes.h"

#include "common/network/chatmessages.h"

class Server;

class ChatHandler : public MessageHandler
{
private:
  Server* server;

public:
  ChatHandler(Server* server)
  : server(server)
  {
  }

  char getType() { return MESSAGES::CHAT; }

  void handle(GenericMessage* msg)
  {
    char type = msg->getMsgType();
    if (type != MESSAGES::CHAT) assert("wrong message type");
    char id = msg->getMsgId();

    if (id ==  CHAT::SAY) handleSay(msg);
    else if (id ==  CHAT::SHOUT) handleShout(msg);
    else if (id ==  CHAT::WHISPERTO) handleWhisperTo(msg);
    else if (id ==  CHAT::PARTY) handleParty(msg);
    else if (id ==  CHAT::GUILD) handleGuild(msg);
    else if (id ==  CHAT::FAMILY) handleFamily(msg);
  }

  void handleSay(GenericMessage* msg);
  void handleShout(GenericMessage* msg);
  void handleWhisperTo(GenericMessage* msg);
  void handleParty(GenericMessage* msg);
  void handleGuild(GenericMessage* msg);
  void handleFamily(GenericMessage* msg);
};

#endif // _CHATHANDLER_H_
