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

#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include "stdio.h"

#include "common/network/tcp/tcpmessagehandler.h"
#include "common/network/netmessage.h"

#include "common/network/udp/udpconnectionmessages.h"

#include "client/network/userhandler.h"
#include "client/network/entityhandler.h"
#include "client/network/chathandler.h"
#include "client/network/doorhandler.h"
#include "client/network/resourcehandler.h"
#include "client/network/skillhandler.h"
#include "client/network/questhandler.h"
#include "client/network/tradehandler.h"
#include "client/network/environmenthandler.h"
#include "client/network/bookhandler.h"
#include "client/network/adminhandler.h"
#include "client/network/playerhandler.h"

class Network;

class ConnectionHandler : public TcpMessageHandler
{
private:
  Network* network;

  UserHandler user_handler;
  EntityHandler entity_handler;
  ChatHandler chat_handler;
  DoorHandler door_handler;
  SkillHandler skill_handler;
  ResourceHandler resource_handler;
  QuestHandler quest_handler;
  TradeHandler trade_handler;
  EnvironmentHandler environment_handler;
  BookHandler book_handler;
  AdminHandler admin_handler;
  PlayerHandler player_handler;

public:
  ConnectionHandler(Network* network)
    : network(network)
  {
  }

  void handle(GenericMessage* msg, int socket)
  {
    sawServer();

    char type = msg->getMsgType();
    if (type == MESSAGES::CONNECTION)
    {
      char id = msg->getMsgId();

      if (id == CONNECTION::RESPONSE) handleConnectionResponse(msg);
      else if (id == CONNECTION::PING) handlePing(msg);
    }
    else if (type == MESSAGES::USER) user_handler.handle(msg);
    else if (type == MESSAGES::ENTITY) entity_handler.handle(msg);
    else if (type == MESSAGES::CHAT) chat_handler.handle(msg);
    else if (type == MESSAGES::DOOR) door_handler.handle(msg);
    else if (type == MESSAGES::SKILL) skill_handler.handle(msg);
    else if (type == MESSAGES::RESOURCE) resource_handler.handle(msg);
    else if (type == MESSAGES::QUEST) quest_handler.handle(msg);
    else if (type == MESSAGES::TRADE) trade_handler.handle(msg);
    else if (type == MESSAGES::ENVIRONMENT) environment_handler.handle(msg);
    else if (type == MESSAGES::BOOK) book_handler.handle(msg);
    else if (type == MESSAGES::ADMIN) admin_handler.handle(msg);
    else if (type == MESSAGES::PLAYER) player_handler.handle(msg);
  }

  void sawServer();

  char getType()
  {
    return MESSAGES::CONNECTION;
  }

  void handleConnectionResponse(GenericMessage* msg);
  void handlePing(GenericMessage* ping_msg);
};

#endif // CONNECTIONHANDLER_H
