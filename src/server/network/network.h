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

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "server/server.h"

#include "server/network/tcp/tcpreceiver.h"
#include "server/network/tcp/tcpnetwork.h"
#include "server/network/userhandler.h"
#include "server/network/doorhandler.h"
#include "server/network/questhandler.h"
#include "server/network/skillhandler.h"
#include "server/network/tradehandler.h"
#include "server/network/entityhandler.h"
#include "server/network/chathandler.h"
#include "server/network/environmenthandler.h"
#include "server/network/bookhandler.h"

class Network
{
private:
  UserHandler user_handler;
  EntityHandler ent_handler;
  ChatHandler chat_handler;
  DoorHandler door_handler;
  QuestHandler quest_handler;
  SkillHandler skill_handler;
  TradeHandler trade_handler;
  EnvironmentHandler environment_handler;
  BookHandler book_handler;
  TcpNetwork tcp_nw;

public:
  Network(Server* server) 
  : user_handler(server), ent_handler(server), chat_handler(server),
    door_handler(server), quest_handler(server), skill_handler(server),
    trade_handler(server), environment_handler(server), book_handler(server),
    tcp_nw()
  {
    server->setNetwork(this);
  }

  void init(unsigned short port)
  {
    tcp_nw.init(port);
    tcp_nw.registerHandler(&user_handler);
    tcp_nw.registerHandler(&ent_handler);
    tcp_nw.registerHandler(&chat_handler);
    tcp_nw.registerHandler(&door_handler);
    tcp_nw.registerHandler(&quest_handler);
    tcp_nw.registerHandler(&skill_handler);
    tcp_nw.registerHandler(&trade_handler);
    tcp_nw.registerHandler(&environment_handler);
    tcp_nw.registerHandler(&book_handler);
    tcp_nw.start();
  }

  void getStats(size_t& sentbyte, size_t& recvbyte, size_t& timestamp)
  {
    tcp_nw.getStats(sentbyte, recvbyte, timestamp);
  }

  void shutdown()
  {
    tcp_nw.shutdown();
  }
};

#endif // _NETWORK_H_
