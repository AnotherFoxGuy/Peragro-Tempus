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

#ifndef _CONNECTIONHANDLER_H_
#define _CONNECTIONHANDLER_H_

#include "stdio.h"

#include "common/network/udp/udpmessagehandler.h"
#include "common/network/netmessage.h"

#include "client/client.h"

#include "common/network/udp/udpconnectionmessages.h"

#include "client/network/userhandler.h"

class ConnectionHandler : public UdpMessageHandler
{
private:
  Network* network;
  Client* client;

  UserHandler user_handler;

public:
  ConnectionHandler(Network* network, Client* client)
  : network(network), client(client), user_handler(network, client)
  {
  }

  void handle(GenericMessage* msg, SocketAddress& sock_addr)
  {
    char type = msg->getMsgType();
    if (type == MESSAGES::CONNECTION)
    {
      char id = msg->getMsgId();

      if (id == CONNECTION::RESPONSE) handleConnectionResponse(msg);
      else if (id == CONNECTION::PING) handlePing(msg);
    }
    else
    {
      user_handler.handle(msg);
    }
  }

  char getType()
  {
    return MESSAGES::CONNECTION;
  }

  void handleConnectionResponse(GenericMessage* msg);
  void handlePing(GenericMessage* ping_msg);
};

#endif // _CONNECTIONHANDLER_H_
