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
#include "common/network/udp/udpconnectionmessages.h"

#include "server/network/udp/connectionmanager.h"

class UdpConnectionHandler : public UdpMessageHandler
{
private:
  UdpConnectionManager* updConnMgr;
  MessageHandler* msg_h;
  Socket* socket;

public:
  UdpConnectionHandler(UdpConnectionManager* updConnMgr, Socket* socket)
  : updConnMgr(updConnMgr), socket(socket)
  {
  }

  void handle(GenericMessage* msg, SocketAddress& sock_addr)
  {
    char type = msg->getMsgType();
    char id = msg->getMsgId();

    if (type == 0 && id == CONNECTION::REQUEST) 
    {
      handleConnectionRequest(msg, &sock_addr);
    }
    else
    {
      UdpConnection* conn = updConnMgr->findBySocketAddress(&sock_addr);
      if (!conn) return; //No connection, no game.

      conn->seen();

      msg->setConnection(conn);

      if (type == 0 && id == CONNECTION::PONG) 
      {
        handlePong(msg);
      }
      else
      {
        msg_h->handle(msg);
      }
    }
  }

  char getType()
  {
    return MESSAGES::CONNECTION;
  }

  void setHandler(MessageHandler* msg_h)
  {
    this->msg_h = msg_h;
  }

private:
  void handleConnectionRequest(GenericMessage* msg, SocketAddress* sock_addr);

  void handlePong(GenericMessage* msg)
  {
    printf("Received PongMessage\n");
    // If the connection were null or if it weren't UdpConnection, this function wouldn't be called in the first place.
    ((UdpConnection*)msg->getConnection())->seen();
  }
};

#endif // _CONNECTIONHANDLER_H_
